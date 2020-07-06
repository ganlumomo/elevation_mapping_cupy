#pragma once

#include <Eigen/Core>

#include <grid_map_core/GridMap.hpp>
#include <opencv2/core/mat.hpp>

#include "convex_plane_decomposition/ransac/RansacPlaneExtractor.hpp"
#include "convex_plane_decomposition/SegmentedPlanesMap.h"

#include "SlidingWindowPlaneExtractorParameters.h"

namespace convex_plane_decomposition {
namespace sliding_window_plane_extractor {

class SlidingWindowPlaneExtractor {
 public:
  SlidingWindowPlaneExtractor(const SlidingWindowPlaneExtractorParameters& parameters,
      const ransac_plane_extractor::RansacPlaneExtractorParameters& ransacParameters);

  void runExtraction(const grid_map::GridMap& map, const std::string& layer_height);

  const SegmentedPlanesMap& getSegmentedPlanesMap() const { return segmentedPlanesMap_; }

  const cv::Mat& getBinaryLabeledImage() const { return binaryImagePatch_; }

 private:
  bool isGloballyPlanar(const Eigen::Vector3d& normalVectorPlane, const Eigen::Vector3d& supportVectorPlane,
                        const std::vector<ransac_plane_extractor::PointWithNormal>& points_with_normal) const;

  std::pair<Eigen::Vector3d, double> computeNormalAndErrorForWindow(const Eigen::MatrixXf& windowData) const;
  bool isLocallyPlanar(const Eigen::Vector3d& localNormal, double meanError) const;

  int getLinearIndex(int row, int col) const { return row + col * map_->getSize()[0]; };

  void computePlaneParametersForLabel(int label);

  void extractPlaneParametersFromLabeledImage();

  void runSegmentation();

  void runSlidingWindowDetector();

  SlidingWindowPlaneExtractorParameters parameters_;
  ransac_plane_extractor::RansacPlaneExtractorParameters ransacParameters_;

  const grid_map::GridMap* map_;
  std::string elevationLayer_;

  std::vector<Eigen::Vector3d> surfaceNormals_;

  cv::Mat binaryImagePatch_;
  SegmentedPlanesMap segmentedPlanesMap_;
};
}  // namespace sliding_window_plane_extractor
}  // namespace convex_plane_decomposition
