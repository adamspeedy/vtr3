#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <map>
#include <algorithm>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/serialization.hpp"
#include "lgmath.hpp"
#include <vtr_common/timing/utils.hpp>
#include <vtr_vision_msgs/msg/rig_landmarks.hpp>
#include <vtr_vision_msgs/msg/channel_landmarks.hpp>
#include <vtr_vision_msgs/msg/h_vec3.hpp>
#include <vtr_vision_msgs/msg/rig_observations.hpp>
#include <vtr_vision_msgs/msg/keypoint.hpp>
#include <geometry_msgs/msg/vector3.hpp>

#include "vtr_pose_graph_msgs/msg/edge_type.hpp"
#include "vtr_pose_graph_msgs/msg/edge_mode.hpp"
#include "vtr_pose_graph_msgs/msg/edge.hpp"
#include "vtr_common_msgs/msg/lie_group_transform.hpp"
#include "vtr_tactic_msgs/msg/odometry_result.hpp"

#include <vtr_pose_graph_msgs/msg/vertex.hpp>
#include <vtr_pose_graph_msgs/msg/timestamp.hpp>
#include <vtr_pose_graph_msgs/msg/timestamp_range.hpp>
#include "vtr_tactic_msgs/msg/localization_result.hpp"

#include "sensor_msgs/msg/image.hpp"
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.hpp> 
#include <vtr_vision_msgs/msg/time_stamp.hpp>
#include <vtr_vision_msgs/msg/image.hpp>


namespace fs = std::filesystem;

struct TopicInfo {
        std::string name;
        std::string type;
};
   
template<typename T>
struct MessageData {
    int64_t timestamp;
    T data;
};

class BagExtractor {
public:

    BagExtractor(const std::string& bag_path, const std::string& bag_name);

    ~BagExtractor();

    template <typename T> 
    std::vector<MessageData<T>> extract_messages(int topic_id = -1, const std::string& topic_name = "", const T& msg = T{}); 
    // std::vector<MessageData> extract_messages(int topic_id , const std::string& topic_name, const T& temp_msg);
    
    
    const std::map<int, TopicInfo>& get_topics_map() const { return topics_; }

private:
    void get_topics();
    
    std::string bag_path_;              ///< Path to the bag file
    sqlite3* db_ = nullptr;             ///< SQLite database connection
    YAML::Node metadata_;               ///< Metadata from the bag file
    std::map<int, TopicInfo> topics_;   ///< Available topics in the bag
};




template <typename T> std::vector<MessageData<T>> BagExtractor::extract_messages(int topic_id , const std::string& topic_name , const T& temp_msg) {
    int target_topic_id = topic_id;
    (void)temp_msg; // Suppress unused variable warning
    
    // Find topic ID if name was provided
    if (topic_id == -1 && !topic_name.empty()) {
        auto it = std::find_if(topics_.begin(), topics_.end(), 
                                [&topic_name](const auto& pair) {
                                    return pair.second.name == topic_name;
                                });
        
        if (it != topics_.end()) {
            target_topic_id = it->first;
        } else {
            std::cerr << "Error: Topic '" << topic_name << "' not found in the bag file" << std::endl;
            return {};
        }
    }
    
    // Get the topic type
    std::string topic_type = topics_[target_topic_id].type;
    
    // Query to get messages
    std::string query = "SELECT timestamp, data FROM messages WHERE topic_id = ? ORDER BY timestamp ASC";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db_) << std::endl;
        return {};
    }
    
    sqlite3_bind_int(stmt, 1, target_topic_id);
    
    std::vector<MessageData<T>> messages;
    
    // Create serialization infrastructure
    // rclcpp::Serialization<vtr_vision_msgs::msg::RigLandmarks> serialization;
    rclcpp::Serialization<T> serialization;
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int64_t timestamp = sqlite3_column_int64(stmt, 0);
        const void* data = sqlite3_column_blob(stmt, 1);
        int data_size = sqlite3_column_bytes(stmt, 1);
        
        // Create a ROS message
        // vtr_vision_msgs::msg::RigLandmarks msg;
        T msg;
        
        // Create a serialized message
        rclcpp::SerializedMessage serialized_msg(data_size);
        memcpy(serialized_msg.get_rcl_serialized_message().buffer, data, data_size);
        serialized_msg.get_rcl_serialized_message().buffer_length = data_size;
        
        try {
            // Deserialize
            serialization.deserialize_message(&serialized_msg, &msg);
            
            MessageData<T> msg_data{timestamp, msg};
            messages.push_back(msg_data);
        } catch (const rclcpp::exceptions::RCLError& e) {
            std::cerr << "Error deserializing message: " << e.what() << std::endl;
        }
    }
    
    sqlite3_finalize(stmt);
    return messages;
}



void clearOutputFolder(const std::string& folderPath);
