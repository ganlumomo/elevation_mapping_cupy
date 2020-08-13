//
// Created by rgrandia on 10.07.20.
//

#include "signed_distance_field/SignedDistanceField.h"

#include "signed_distance_field/DistanceDerivatives.h"
#include "signed_distance_field/SignedDistance2d.h"

namespace signed_distance_field {

SignedDistanceField::SignedDistanceField(const grid_map::GridMap& gridMap, const std::string& elevationLayer, double minHeight,
                                         double maxHeight) {
  assert(maxHeight >= minHeight);
  grid_map::Position mapOriginXY;
  gridMap.getPosition(Eigen::Vector2i(0, 0), mapOriginXY);
  Eigen::Vector3d gridOrigin(mapOriginXY.x(), mapOriginXY.y(), minHeight);

  // Take minimum of two layers to enable finite difference in Z direction
  const auto numZLayers = static_cast<size_t>(std::max(std::ceil((maxHeight - minHeight) / gridMap.getResolution()), 2.0));
  const size_t numXrows = gridMap.getSize().x();
  const size_t numYrows = gridMap.getSize().y();
  Gridmap3dLookup::size_t_3d gridsize = {numXrows, numYrows, numZLayers};

  gridmap3DLookup_ = Gridmap3dLookup(gridsize, gridOrigin, gridMap.getResolution());

  data_.reserve(gridmap3DLookup_.linearSize());
  computeSignedDistance(gridMap.get(elevationLayer));
}

double SignedDistanceField::atPosition(const Eigen::Vector3d& position) const {
  const auto nodeIndex = gridmap3DLookup_.nearestNode(position);
  const Eigen::Vector3d nodePosition = gridmap3DLookup_.nodePosition(nodeIndex);
  const node_data_t nodeData = data_[gridmap3DLookup_.linearIndex(nodeIndex)];
  const Eigen::Vector3d jacobian = derivative(nodeData);
  return distance(nodeData) + jacobian.dot(position - nodePosition);
}

Eigen::Vector3d SignedDistanceField::derivativeAtPosition(const Eigen::Vector3d& position) const {
  const auto nodeIndex = gridmap3DLookup_.nearestNode(position);
  const node_data_t nodeData = data_[gridmap3DLookup_.linearIndex(nodeIndex)];
  return derivative(nodeData);
}

std::pair<double, Eigen::Vector3d> SignedDistanceField::distanceAndDerivativeAt(const Eigen::Vector3d& position) const {
  const auto nodeIndex = gridmap3DLookup_.nearestNode(position);
  const Eigen::Vector3d nodePosition = gridmap3DLookup_.nodePosition(nodeIndex);
  const node_data_t nodeData = data_[gridmap3DLookup_.linearIndex(nodeIndex)];
  const Eigen::Vector3d jacobian = derivative(nodeData);
  return {distance(nodeData) + jacobian.dot(position - nodePosition), jacobian};
}

void SignedDistanceField::computeSignedDistance(const grid_map::Matrix& elevation) {
  const auto gridOriginZ = static_cast<float>(gridmap3DLookup_.gridOrigin_.z());
  const auto resolution = static_cast<float>(gridmap3DLookup_.resolution_);

  // First layer: forward difference in z
  grid_map::Matrix currentLayer = signed_distance_field::signedDistanceAtHeight(elevation, gridOriginZ, resolution);
  grid_map::Matrix nextLayer = signed_distance_field::signedDistanceAtHeight(elevation, gridOriginZ + resolution, resolution);
  grid_map::Matrix dz = signed_distance_field::layerFiniteDifference(currentLayer, nextLayer, resolution);  // dz / layer = +resolution
  grid_map::Matrix dy = signed_distance_field::columnwiseCentralDifference(currentLayer, -resolution);      // dy / dcol = -resolution
  grid_map::Matrix dx = signed_distance_field::rowwiseCentralDifference(currentLayer, -resolution);         // dx / drow = -resolution
  emplacebackLayerData(currentLayer, dx, dy, dz);

  // Middle layers: central difference in z
  for (size_t layerZ = 1; layerZ + 1 < gridmap3DLookup_.gridsize_.z; ++layerZ) {
    grid_map::Matrix previousLayer = std::move(currentLayer);
    currentLayer = std::move(nextLayer);
    nextLayer = signed_distance_field::signedDistanceAtHeight(elevation, gridOriginZ + (layerZ + 1) * resolution, resolution);

    dz = signed_distance_field::layerCentralDifference(previousLayer, nextLayer, resolution);
    dy = signed_distance_field::columnwiseCentralDifference(currentLayer, -resolution);
    dx = signed_distance_field::rowwiseCentralDifference(currentLayer, -resolution);
    emplacebackLayerData(currentLayer, dx, dy, dz);
  }

  // Last layer: backward difference in z
  grid_map::Matrix previousLayer = std::move(currentLayer);
  currentLayer = std::move(nextLayer);
  dz = signed_distance_field::layerCentralDifference(previousLayer, currentLayer, resolution);
  dy = signed_distance_field::columnwiseCentralDifference(currentLayer, -resolution);
  dx = signed_distance_field::rowwiseCentralDifference(currentLayer, -resolution);
  emplacebackLayerData(currentLayer, dx, dy, dz);
}

void SignedDistanceField::emplacebackLayerData(const grid_map::Matrix& signedDistance, const grid_map::Matrix& dx,
                                               const grid_map::Matrix& dy, const grid_map::Matrix& dz) {
  for (size_t colY = 0; colY < gridmap3DLookup_.gridsize_.y; ++colY) {
    for (size_t rowX = 0; rowX < gridmap3DLookup_.gridsize_.x; ++rowX) {
      data_.emplace_back(node_data_t{signedDistance(rowX, colY), dx(rowX, colY), dy(rowX, colY), dz(rowX, colY)});
    }
  }
}

pcl::PointCloud<pcl::PointXYZI> SignedDistanceField::asPointCloud() const {
  pcl::PointCloud<pcl::PointXYZI> points;
  points.reserve(gridmap3DLookup_.linearSize());
  for (size_t layerZ = 0; layerZ < gridmap3DLookup_.gridsize_.z; ++layerZ) {
    for (size_t colY = 0; colY < gridmap3DLookup_.gridsize_.y; ++colY) {
      for (size_t rowX = 0; rowX < gridmap3DLookup_.gridsize_.x; ++rowX) {
        const auto index = gridmap3DLookup_.linearIndex({rowX, colY, layerZ});
        const auto p = gridmap3DLookup_.nodePosition({rowX, colY, layerZ});
        pcl::PointXYZI point;
        point.x = p.x();
        point.y = p.y();
        point.z = p.z();
        point.intensity = data_[index][0];
        points.push_back(point);
      }
    }
  }
  return points;
}

}  // namespace signed_distance_field
