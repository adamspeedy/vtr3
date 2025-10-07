// ImagePublisher.cpp

#include <cv_bridge/cv_bridge.hpp>
#include <std_msgs/msg/header.hpp>
#include <algorithm>
#include <cctype>
#include <vtr_vision/image_publisher.hpp>

namespace vtr {
namespace vision {
namespace visualize {

ImagePublisher::ImagePublisher(rclcpp::Node::SharedPtr node)
    : node_(node), topic_prefix_("/visualization"), enabled_(true) {
  if (!node_) {
    throw std::invalid_argument("Node pointer cannot be null");
  }
}

void ImagePublisher::publishImage(const cv::Mat& image, 
                                 const std::string& topic_name,
                                 const std::string& frame_id) {
  std::lock_guard<std::mutex> lock(mutex_);
  
  if (!enabled_ || !node_ || image.empty()) {
    return;
  }

  try {
    // Get or create publisher
    auto publisher = getOrCreatePublisher(topic_name);
    
    // Determine encoding based on image type
    std::string encoding;
    if (image.channels() == 1) {
      encoding = "mono8";
    } else if (image.channels() == 3) {
      encoding = "bgr8";  // OpenCV uses BGR by default
    } else if (image.channels() == 4) {
      encoding = "bgra8";
    } else {
      RCLCPP_WARN(node_->get_logger(), 
                  "Unsupported image format with %d channels for topic %s", 
                  image.channels(), topic_name.c_str());
      return;
    }

    // Convert OpenCV image to ROS2 message
    auto msg = cv_bridge::CvImage(std_msgs::msg::Header(), encoding, image).toImageMsg();
    msg->header.stamp = node_->get_clock()->now();
    msg->header.frame_id = frame_id;
    
    // Publish the image
    publisher->publish(*msg);
    
  } catch (const std::exception& e) {
    RCLCPP_ERROR(node_->get_logger(), 
                 "Failed to publish image to topic %s: %s", 
                 topic_name.c_str(), e.what());
  }
}

void ImagePublisher::publishImages(const std::map<std::string, cv::Mat>& images,
                                  const std::string& topic_prefix,
                                  const std::string& frame_id) {
  for (const auto& [title, image] : images) {
    std::string full_topic = topic_prefix + "/" + sanitizeTopicName(title);
    publishImage(image, full_topic, frame_id);
  }
}

bool ImagePublisher::isReady() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return node_ != nullptr && enabled_;
}

void ImagePublisher::setTopicPrefix(const std::string& prefix) {
  std::lock_guard<std::mutex> lock(mutex_);
  topic_prefix_ = prefix;
}

void ImagePublisher::setEnabled(bool enabled) {
  std::lock_guard<std::mutex> lock(mutex_);
  enabled_ = enabled;
}

bool ImagePublisher::isEnabled() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return enabled_;
}

std::string ImagePublisher::sanitizeTopicName(const std::string& name) {
  std::string sanitized = name;
  
  // Replace invalid characters with underscores
  std::replace_if(sanitized.begin(), sanitized.end(), 
                  [](char c) { 
                    return c == '/' || c == ' ' || c == '-' || c == '.' || 
                           c == '(' || c == ')' || c == '[' || c == ']';
                  }, '_');
  
  // Remove consecutive underscores
  auto end = std::unique(sanitized.begin(), sanitized.end(),
                        [](char a, char b) { return a == '_' && b == '_'; });
  sanitized.erase(end, sanitized.end());
  
  // Ensure it starts with a letter or underscore
  if (!sanitized.empty() && !std::isalpha(sanitized[0]) && sanitized[0] != '_') {
    sanitized = "_" + sanitized;
  }
  
  // Remove trailing underscore
  if (!sanitized.empty() && sanitized.back() == '_') {
    sanitized.pop_back();
  }
  
  return sanitized;
}

rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr 
ImagePublisher::getOrCreatePublisher(const std::string& topic_name) {
  auto it = publishers_.find(topic_name);
  if (it != publishers_.end()) {
    return it->second;
  }

  // Create new publisher
  auto publisher = node_->create_publisher<sensor_msgs::msg::Image>(topic_name, 10);
  publishers_[topic_name] = publisher;
  
  RCLCPP_DEBUG(node_->get_logger(), "Created publisher for topic: %s", topic_name.c_str());
  return publisher;
}

} // namespace visualize
} // namespace vision
} // namespace vtr