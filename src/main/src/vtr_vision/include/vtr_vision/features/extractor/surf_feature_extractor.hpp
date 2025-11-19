/**
 * \file surf_feature_extractor.hpp
 * \brief
 * \details
 *
 * \author Adam Speed-Andrews (African Robotics Unit)
 */
#pragma once

#include <memory>
#include <vector>

#include <opencv2/core/version.hpp>
#include <opencv2/xfeatures2d.hpp>
// #if defined(HAVE_OPENCV_CUDAFEATURES2D)
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
// #endif
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vtr_vision/features/extractor/base_feature_extractor.hpp>
#include <vtr_vision/features/extractor/surf_configuration.hpp>


typedef cv::UMat uMat;

namespace vtr {
namespace vision {

/////////////////////////////////////////////////////////////////////////
/// @class SurfFeatureExtractor
/// @brief Feature extractor for the OpenCV Surf Feature Extractor.
/// @details This class accepts greyscale images and computes Surf features
///          on either mono images or stereo pairs with left-right matching.
/////////////////////////////////////////////////////////////////////////
class SurfFeatureExtractor : public BaseFeatureExtractor {
 public:
  /////////////////////////////////////////////////////////////////////////
  /// @param Surf configuration
  SurfFeatureExtractor(){};

  /////////////////////////////////////////////////////////////////////////
  virtual ~SurfFeatureExtractor(){};

  /////////////////////////////////////////////////////////////////////////
  /// @brief Initializes the underlying Surf engine.
  void initialize(const SURFConfiguration& config);

  /////////////////////////////////////////////////////////////////////////
  /// @brief Extracts a list of descriptors and keypoints from a single image.
  /// @param[in] image The input image.
  /// @return A Frame consisting of a list of keypoints and a list of
  /// descriptors, corresponding to the keypoints.
  virtual Features extractFeatures(const cv::Mat& image);

  /////////////////////////////////////////////////////////////////////////
  /// @brief Extracts a list of descriptors and keypoints from a set of
  ///        two rectified stereo images.
  /// @param[in] left The collection of input images.
  /// @param[in] right The collection of input images.
  /// @return A StereoFrame consisting of two Frames and a list of
  /// correspondences as a MatchList
  virtual ChannelFeatures extractStereoFeatures(const cv::Mat& left,
                                                const cv::Mat& right);

  /////////////////////////////////////////////////////////////////////////
  /// @brief Extracts a list of descriptors and keypoints from a set of
  ///        two rectified stereo images.
  /// @param[in] left The collection of input images.
  /// @param[in] right The collection of input images.
  /// @return A StereoFrame consisting of two Frames and a list of
  /// correspondences as a MatchList
  virtual ChannelFeatures extractStereoFeaturesDisp(const cv::Mat& left,
                                                    const cv::Mat& disp);

  /////////////////////////////////////////////////////////////////////////
  /// Extracts a list of descriptors and keypoints from a set of
  ///   two rectified stereo images.
  /// @param[in] left the left image.
  /// @param[in] right the right image.
  /// @return the extracted features that have been pruned and match-aligned.
  virtual ChannelFeatures extractStereoFeaturesDispExtra(const cv::Mat &left,
                                                         const cv::Mat &disp,
                                                         const cv::Mat 
                                                         &keypoints,
                                                         const cv::Mat 
                                                         &descriptors,
                                                         const cv::Mat 
                                                         &scores);

  /////////////////////////////////////////////////////////////////////////
  /// @brief Extracts a list of descriptors and keypoints from a single image.
  /// @param[in] image the input image.
  /// @return the extracted features (keypoints, descriptors, info)
  virtual ChannelExtra extractFeaturesExtra(const cv::Mat &image);


 private:
  /////////////////////////////////////////////////////////////////////////
  /// @brief Detect features on an image using the default Surf
  /// detector
  void detectWithSurf(const uMat& image, Keypoints& keypoints,
                     const cv::Mat& mask);

  /////////////////////////////////////////////////////////////////////////
  /// @brief Bin keypoints
  void binKeypoints(const cv::Size& size, Keypoints& keypoints);

  /////////////////////////////////////////////////////////////////////////
  /// @brief The Surf configuration.
  /////////////////////////////////////////////////////////////////////////
  SURFConfiguration config_;

  cv::Ptr<cv::xfeatures2d::SURF> detector_;


#if defined(HAVE_OPENCV_CUDAFEATURES2D) && CV_MINOR_VERSION > 1
  cv::Ptr<cv::cuda::SURF_CUDA> cudadetector_;
#endif  // defined(HAVE_OPENCV_CUDAFEATURES2D)
};

}  // namespace vision
}  // namespace vtr
