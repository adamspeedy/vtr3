/**
 * \file surf_configuration.hpp
 * \brief
 * \details
 *
 * \author Adam Speed-Andrews (African Robotics Unit)
 */
#pragma once

#include <vtr_vision/features/matcher/asrl_feature_matcher.hpp>

namespace vtr {
namespace vision {
// SURF Detector params
struct SURFConfiguration {
  double hessianThreshold_;
  bool use_GPU_descriptors_;
  int num_detector_features_;
  int num_binned_features_;
  int nOctaves_;
  int nOctaveLayers_;
  bool extended_;
  double keypointRatio_;
  bool upright_;
  int x_bins_;
  int y_bins_;
  int num_threads_;
  // Stereo Matcher Configuration
  ASRLFeatureMatcher::Config stereo_matcher_config_;
};

}  // namespace vision
}  // namespace vtr

