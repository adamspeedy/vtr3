/**
 * \file surf_feature_extractor.cpp
 * \brief Source file for the ASRL vision package
 * \details
 *
 * \author Adam Speed-Andrews (African Robotics Unit)
 */
#include <cmath>

#include <vtr_logging/logging.hpp>
#include <vtr_vision/features/extractor/surf_configuration.hpp>
#include <vtr_vision/features/extractor/surf_feature_extractor.hpp>

namespace vtr {
namespace vision {

using SFE = SurfFeatureExtractor;

void SFE::initialize(const SURFConfiguration &config) {
  config_ = config;

  detector_ = cv::xfeatures2d::SURF::create(
      config_.hessianThreshold_, config_.nOctaves_, config_.nOctaveLayers_,
        config_.extended_, config_.upright_);

    if (config_.use_GPU_descriptors_){
    cudadetector_ = cv::cuda::SURF_CUDA::create(
        config_.hessianThreshold_, config_.nOctaves_, config_.nOctaveLayers_,
        config_.extended_, config_.keypointRatio_, config_.upright_);
    }
}



/////////////////////////////////////////////////////////////////////////
/// @brief Detect features on an image using the default SURF
/// detector
void SFE::detectWithSurf(const uMat &image, Keypoints &keypoints,
                        const cv::Mat &mask) {
  (void)mask;  /// \todo unused

  // reserve keypoints
  keypoints.reserve(config_.num_detector_features_);

  // detect keypoints (don't generate descriptors yet).
  detector_->detect(image, keypoints, cv::noArray());

  // bin the keypoints
  binKeypoints(image.size(), keypoints);
}

void SFE::binKeypoints(const cv::Size &size, Keypoints &keypoints) {
  // sanity check
  config_.x_bins_ = std::max(1, config_.x_bins_);
  config_.y_bins_ = std::max(1, config_.y_bins_);

  // figure out the number of buckets we need
  unsigned num_buckets = config_.x_bins_ * config_.y_bins_;

  // slightly inflate the number per bucket that we need
  unsigned desired_num_per_bucket =
      1.2 * config_.num_binned_features_ / num_buckets;

  // determine how many pixels there are in each bin
  float pixels_per_bin_x = float(size.width) / config_.x_bins_;
  float pixels_per_bin_y = float(size.height) / config_.y_bins_;

  // keep a record of the keypoints we bin
  std::map<std::pair<unsigned, unsigned>, std::vector<cv::KeyPoint *> >
      binned_keypoints;

  // make a new keypoints container
  Keypoints keypoints_new;
  keypoints_new.reserve(keypoints.size());

  for (auto &keypoint : keypoints) {
    unsigned bx = std::floor(keypoint.pt.x / pixels_per_bin_x);
    unsigned by = std::floor(keypoint.pt.y / pixels_per_bin_y);
    std::pair<unsigned, unsigned> pair(bx, by);
    binned_keypoints[pair].reserve(config_.num_detector_features_ /
                                   num_buckets);
    binned_keypoints[pair].push_back(&keypoint);
  }

  for (int ii = 0; ii < config_.x_bins_; ii++) {
    for (int jj = 0; jj < config_.y_bins_; jj++) {
      std::pair<unsigned, unsigned> pair(ii, jj);
      if (binned_keypoints[pair].size() > desired_num_per_bucket) {
        std::sort(binned_keypoints[pair].begin(), binned_keypoints[pair].end(),
                  [&](const cv::KeyPoint *k1, const cv::KeyPoint *k2) {
                    return k1->response > k2->response;
                  });
        binned_keypoints[pair].resize(desired_num_per_bucket);
      }
      for (unsigned kk = 0; kk < binned_keypoints[pair].size(); kk++) {
        // force keypoints to be upright if the config calls for it
        if (config_.upright_) {
          binned_keypoints[pair][kk]->angle = -1;
        }
        keypoints_new.push_back(*binned_keypoints[pair][kk]);
      }
    }
  }

  // now place the upper cap on the number of features
  std::sort(keypoints_new.begin(), keypoints_new.end(),
            [&](const cv::KeyPoint &k1, const cv::KeyPoint &k2) {
              return k1.response > k2.response;
            });
  if ((int)keypoints_new.size() > config_.num_binned_features_) {
    keypoints_new.resize(config_.num_binned_features_);
  }

  // clear out and copy new keypoints
  keypoints.clear();
  keypoints = keypoints_new;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Extracts a list of descriptors and keypoints from a single image.
Features SFE::extractFeatures(const cv::Mat &image) {
  //CLOG(DEBUG, "stereo.matcher") << "Extracting features from image" ;
  uMat uimage;
  image.copyTo(uimage);
  // create a new empty frame
  Features frame;
  frame.keypoints.reserve(config_.num_detector_features_);
  frame.feat_infos.reserve(config_.num_detector_features_);
  frame.feat_type.impl = FeatureImpl::OPENCV_SURF;
  frame.feat_type.upright = config_.upright_;

  // make an empty mask. the feature detectors need this but we don't use it
  cv::Mat mask;


  // create a new feature detector
  if (config_.use_GPU_descriptors_) {
// #if defined(HAVE_OPENCV_CUDAFEATURES2D)
    // we have to sort by the octave or the GPU descriptors get messed up for some reason
    std::sort(frame.keypoints.begin(), frame.keypoints.end(),
              [&](const cv::KeyPoint &k1, const cv::KeyPoint &k2) {
                return k1.octave < k2.octave;
              });
    // the GPU can only compute descriptors one at a time, so need a mutex here
    {
      std::lock_guard<std::mutex> lock(__gpu_mutex__);
      cv::cuda::GpuMat gpuimage(image);
      cv::cuda::GpuMat img, gpuKeypoints, gpuDescriptors;
      cudadetector_->detectWithDescriptors(gpuimage, cv::cuda::GpuMat(), gpuKeypoints, gpuDescriptors, false);

      gpuDescriptors.download(frame.descriptors);
      // for (unsigned i = 0; i < frame.descriptors.rows; i++) {
      //     CLOG(DEBUG, "stereo.features") << "Descriptors: "<< i <<" : " << frame.descriptors.row(i);
      // }
      CLOG(DEBUG, "stereo.features") << "number of Descriptors found: " << frame.descriptors.size();
      
      cudadetector_->downloadKeypoints(gpuKeypoints, frame.keypoints);
      CLOG(DEBUG, "stereo.features") << "number of keypoints found: " << frame.keypoints.size();
    }

  } else {
    detectWithSurf(uimage, frame.keypoints, mask);
    detector_->compute(uimage, frame.keypoints, frame.descriptors);
  }

  // fill out the feature infos (this is after descriptor computation,
  // as some keypoints may be removed by the descriptor computation)
  for (unsigned kk = 0; kk < frame.keypoints.size(); kk++) {
    double sigma = std::pow(config_.nOctaveLayers_, frame.keypoints[kk].octave); //just a placeholder for now
    double sigma_squared = sigma * sigma;
    bool laplacian_bit = static_cast<int>(frame.keypoints[kk].response) & 0x1;
    frame.feat_infos.emplace_back();
    FeatureInfo &info = frame.feat_infos.back();
    info.laplacian_bit = laplacian_bit;
    info.precision = 1.0 / sigma_squared;
    info.covariance(0, 0) = sigma_squared;
    info.covariance(1, 1) = sigma_squared;
    info.covariance(0, 1) = 0.0;
    info.covariance(1, 0) = 0.0;
  }

  frame.feat_type.dims = 64; //frame.descriptors.step1();
  frame.feat_type.bytes_per_desc = 64*sizeof(float);  //frame.descriptors.step1();
  return frame;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Extracts a list of descriptors and keypoints from a set of two
/// rectified stereo images.
ChannelFeatures SFE::extractStereoFeatures(const cv::Mat &left_img,
                                           const cv::Mat &right_img) {
  //CLOG(DEBUG, "stereo.matcher") << "Running stereo feature matching" ;
  // create a new empty frame
  ChannelFeatures features_temp;
  features_temp.cameras.reserve(2);

  // make two asynchronous calls to extract the features
  Features (BaseFeatureExtractor::*extract_func)(const cv::Mat &) =
      &BaseFeatureExtractor::extractFeatures;
  auto handle_left =
      std::async(std::launch::async, extract_func, this, left_img);
  auto handle_right =
      std::async(std::launch::async, extract_func, this, right_img);
  features_temp.cameras.push_back(handle_left.get());
  features_temp.cameras.push_back(handle_right.get());

  // We need to match for the stereo case
  vtr::vision::ASRLFeatureMatcher::Config matcher_config = config_.stereo_matcher_config_;
  matcher_config.descriptor_match_thresh_ = config_.stereo_matcher_config_.stereo_descriptor_match_thresh_;
  // don't ask me how but the feature matcher seems to still get all of the parameters from the config.

  // set the number of threads
  matcher_config.num_threads_ = 8;
  vtr::vision::ASRLFeatureMatcher matcher(matcher_config);

  // create the new empty channel features, copy descriptor type info
  ChannelFeatures features;
  features.cameras.resize(2);
  auto cam_rng = {0, 1};
  for (auto &i : cam_rng)
    features.cameras[i].feat_type = features_temp.cameras[i].feat_type;

  // perform matching
  SimpleMatches matches;
  if (calib_.rectified || calib_.extrinsics.size() < 2) {
    // if the rig is rectified or not set, just do regular stereo matching
    matches = matcher.matchStereoFeatures(features_temp.cameras[0],
                                          features_temp.cameras[1]);
    // CLOG(DEBUG, "stereo.matcher") << "Rig is rectified" ;
  } else {
    // if not rectified, we need to do epipolar matching
    auto tf = calib_.extrinsics[0].inverse() * calib_.extrinsics[1];
    // CLOG(DEBUG, "stereo.matcher") << "rig is not rectified" ;
    // pre-cache some data to make the operations easier to read
    CameraIntrinsic &K0 = calib_.intrinsics[0];
    CameraIntrinsic &K1 = calib_.intrinsics[1];
    Eigen::Matrix3d K1it = K1.transpose().inverse();
    Eigen::Vector3d KrtT = K0 * tf.C_ba().transpose() * tf.r_ba_ina();

    // turn KrtT into skew symmetric form
    Eigen::Matrix3d KrtTcross;
    KrtTcross << 0, -KrtT(2), KrtT(1), KrtT(2), 0, -KrtT(0), -KrtT(1), KrtT(0),
        0;

    // make the fundamental matrix
    Eigen::Matrix3d F = K1it * tf.C_ba() * K0.transpose() * KrtTcross;

    // do matching
    matches = matcher.matchFeatures(features_temp.cameras[0],
                                    features_temp.cameras[1], F,
                                    matcher_config.stereo_x_tolerance_min_,
                                    matcher_config.stereo_x_tolerance_max_,
                                    matcher_config.stereo_y_tolerance_,
                                    ASRLFeatureMatcher::CheckType::EPIPOLE);
  }

  // reserve the corresponding vectors to match
  const auto &desc_cols = features_temp.cameras[0].descriptors.cols;
  const auto &desc_cvtype = features_temp.cameras[0].descriptors.type();
  for (unsigned j : cam_rng) {
    Features &j_feat = features.cameras[j];
    j_feat.keypoints.reserve(matches.size());
    j_feat.feat_infos.reserve(matches.size());
    j_feat.descriptors = cv::Mat(matches.size(), desc_cols, desc_cvtype);
  }

  // our stereo frames have the special requirement that all
  // features much have a match, and the matches index-aligned
  for (unsigned i = 0; i < matches.size(); i++) {
    for (unsigned j : cam_rng) {
      const auto &temp_idx = j == 0 ? matches[i].first : matches[i].second;
      Features &j_feat = features.cameras[j];
      Features &j_temp_feat = features_temp.cameras[j];
      j_feat.keypoints.push_back(j_temp_feat.keypoints[temp_idx]);
      j_feat.feat_infos.push_back(j_temp_feat.feat_infos[temp_idx]);
      j_temp_feat.descriptors.row(temp_idx).copyTo(j_feat.descriptors.row(i));
    }
  }

  return features;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Extracts a list of descriptors and keypoints from a set of two
/// rectified stereo images.
ChannelFeatures SFE::extractStereoFeaturesDisp(
    const cv::Mat &left_img, const cv::Mat &disp) {
    ChannelFeatures features_temp;
    //CLOG(DEBUG, "stereo.matcher") << "extract channel features disp running" ;
    return features_temp;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Extracts a list of descriptors and keypoints from a set of two
/// rectified stereo images.
ChannelFeatures SFE::extractStereoFeaturesDispExtra(
    const cv::Mat &left_img, const cv::Mat &disp, const cv::Mat &keypoints,
    const cv::Mat &descriptors, const cv::Mat &scores) {
    ChannelFeatures features_temp;
    return features_temp;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Extracts a list of descriptors and keypoints from a set of two
/// rectified stereo images.
ChannelExtra SFE::extractFeaturesExtra(const cv::Mat &left_img) {
    ChannelExtra extra_temp;
    return extra_temp;
}

}  // namespace vision
}  // namespace vtr
