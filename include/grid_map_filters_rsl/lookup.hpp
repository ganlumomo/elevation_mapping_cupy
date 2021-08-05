/**
 * @file        lookup.hpp
 * @authors     Fabian Jenelten, Ruben Grandia
 * @date        04.08, 2021
 * @affiliation ETH RSL
 * @brief       Extracting information from a gridmap
 */

#pragma once

// grid map.
#include <grid_map_core/grid_map_core.hpp>

namespace grid_map {
namespace lookup {

struct LookupResult {
  bool isValid{false};
  float value{0.0};
  grid_map::Position position{0.0, 0.0};
};

/**
 * Finds the maximum value between two points in a map
 *
 * @param position1 : starting point of the lookup line.
 * @param position2 : ending point of the lookup line.
 * @param gridMap : map object for map information.
 * @param data : map data to find the maximum in.
 * @return validity, value, and location of the maximum. A result is flagged as invalid if there are no finite values found.
 */
LookupResult maxValueBetweenLocations(const grid_map::Position& position1, const grid_map::Position& position2,
                                      const grid_map::GridMap& gridMap, const grid_map::Matrix& data);

}  // namespace lookup
}  // namespace grid_map