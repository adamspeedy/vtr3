#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "vtr_common/utils/macros.hpp"

namespace vtr {
namespace vision {
namespace visualize {

class ImagePublisher {
public:
PTR_TYPEDEFS(ImagePublisher);
  /**
   * @brief Constructor
   * @param node ROS2 node for creating publishers
   */
  explicit ImagePublisher(rclcpp::Node::SharedPtr node);
  
  /**
   * @brief Destructor
   */
  ~ImagePublisher() = default;

  /**
   * @brief Publish a single image to a ROS2 topic
   * @param image OpenCV image to publish
   * @param topic_name Name of the topic to publish to
   * @param frame_id Frame ID for the image header (default: "camera")
   */
  void publishImage(const cv::Mat& image, 
                   const std::string& topic_name, 
                   const std::string& frame_id = "camera");

  /**
   * @brief Publish multiple images with a common prefix
   * @param images Map of image titles to OpenCV images
   * @param topic_prefix Prefix for all topic names
   * @param frame_id Frame ID for the image headers
   */
  void publishImages(const std::map<std::string, cv::Mat>& images,
                    const std::string& topic_prefix = "/visualization",
                    const std::string& frame_id = "camera");

  /**
   * @brief Check if the publisher is initialized and ready
   * @return true if ready, false otherwise
   */
  bool isReady() const;

  /**
   * @brief Set the base topic prefix for all published images
   * @param prefix Base topic prefix (default: "/visualization")
   */
  void setTopicPrefix(const std::string& prefix);

  /**
   * @brief Enable or disable publishing
   * @param enabled true to enable publishing, false to disable
   */
  void setEnabled(bool enabled);

  /**
   * @brief Check if publishing is enabled
   * @return true if enabled, false otherwise
   */
  bool isEnabled() const;

private:
  /**
   * @brief Sanitize topic name to be ROS2 compliant
   * @param name Original topic name
   * @return Sanitized topic name
   */
  std::string sanitizeTopicName(const std::string& name);

  /**
   * @brief Get or create a publisher for the given topic
   * @param topic_name Topic name
   * @return Shared pointer to the publisher
   */
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr 
    getOrCreatePublisher(const std::string& topic_name);

  rclcpp::Node::SharedPtr node_;
  std::map<std::string, rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr> publishers_;
  std::string topic_prefix_;
  bool enabled_;
  mutable std::mutex mutex_;
};

}
} // namespace vision
} // namespace vtr