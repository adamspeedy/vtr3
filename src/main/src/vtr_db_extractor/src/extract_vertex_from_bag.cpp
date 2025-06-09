#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/serialization.hpp"
#include "lgmath.hpp"
// #include <vtr_common/timing/stopwatch.hpp>
#include <vtr_common/timing/utils.hpp>
#include <vtr_pose_graph_msgs/msg/vertex.hpp>
#include <vtr_pose_graph_msgs/msg/timestamp.hpp>
#include <vtr_pose_graph_msgs/msg/timestamp_range.hpp>


namespace fs = std::filesystem;

class VertexBagExtractor {
public:
    //EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    VertexBagExtractor(const std::string& bag_path) : bag_path_(bag_path) {
        std::string db_path = bag_path + "/vertices_0.db3";
        std::string metadata_path = bag_path + "/metadata.yaml";
        
        std::cout << "DB Path: " << db_path << std::endl;
        
        // Check if paths exist
        if (!fs::exists(db_path)) {
            std::cerr << "Error: Database file not found at " << db_path << std::endl;
            exit(1);
        }
        if (!fs::exists(metadata_path)) {
            std::cerr << "Error: Metadata file not found at " << metadata_path << std::endl;
            exit(1);
        }
        // Load metadata
        try {
            metadata_ = YAML::LoadFile(metadata_path);
        } catch (const YAML::Exception& e) {
            std::cerr << "Error loading YAML: " << e.what() << std::endl;
            exit(1);
        }
        // Connect to the database
        int rc = sqlite3_open(db_path.c_str(), &db_);
        if (rc) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db_) << std::endl;
            exit(1);
        }
        // Get available topics
        get_topics();
    }
    
    ~VertexBagExtractor() {
        if (db_) {
            sqlite3_close(db_);
        }
    }
    
    struct TopicInfo {
        std::string name;
        std::string type;
    };
    
    struct MessageData {
        int64_t timestamp;
        vtr_pose_graph_msgs::msg::Vertex data;
    };
    
    std::vector<MessageData> extract_messages(int topic_id = -1, const std::string& topic_name = "") {
        int target_topic_id = topic_id;
        
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
        std::string query = "SELECT timestamp, data FROM messages WHERE topic_id = ?";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db_) << std::endl;
            return {};
        }
        
        sqlite3_bind_int(stmt, 1, target_topic_id);
        
        std::vector<MessageData> messages;
        
        // Create serialization infrastructure
        rclcpp::Serialization<vtr_pose_graph_msgs::msg::Vertex> serialization;
        
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int64_t timestamp = sqlite3_column_int64(stmt, 0);
            const void* data = sqlite3_column_blob(stmt, 1);
            int data_size = sqlite3_column_bytes(stmt, 1);
            
            // Create a ROS message
            vtr_pose_graph_msgs::msg::Vertex msg;
            
            // Create a serialized message
            rclcpp::SerializedMessage serialized_msg(data_size);
            memcpy(serialized_msg.get_rcl_serialized_message().buffer, data, data_size);
            serialized_msg.get_rcl_serialized_message().buffer_length = data_size;
            
            try {
                // Deserialize
                serialization.deserialize_message(&serialized_msg, &msg);
                
                MessageData msg_data{timestamp, msg};
                messages.push_back(msg_data);
            } catch (const rclcpp::exceptions::RCLError& e) {
                std::cerr << "Error deserializing message: " << e.what() << std::endl;
            }
        }
        
        sqlite3_finalize(stmt);
        return messages;
    }
    
    private:
        void get_topics() {
            const char* query = "SELECT id, name, type FROM topics";
            sqlite3_stmt* stmt;
            
            int rc = sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                std::cerr << "SQL error: " << sqlite3_errmsg(db_) << std::endl;
                return;
            }
            
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                int topic_id = sqlite3_column_int(stmt, 0);
                std::string topic_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                std::string topic_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                
                topics_[topic_id] = {topic_name, topic_type};
            }
            
            sqlite3_finalize(stmt);
        }

        std::string bag_path_;
        sqlite3* db_ = nullptr;
        YAML::Node metadata_;
        std::map<int, TopicInfo> topics_;
};



int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/temp/vision/graph/vertices";
    VertexBagExtractor extractor(bag_directory);
    auto messages = extractor.extract_messages(-1, "vertices");
    std::cout << "Found " << messages.size() << " messages" << std::endl;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    std::vector<lgmath::se3::TransformationWithCovariance> transformed_messages;
    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        int temp = messages[i].data.vertex_time.nanoseconds_since_epoch;
        std::cout << "Timestamp: " << temp << std::endl;
        std::cout << "Time Range : " << messages[i].data.time_range.t1 << " , " << messages[i].data.time_range.t2  << std::endl;
        std::cout << "Vertex ID : " << messages[i].data.id << std::endl;

        vtr::common::timing::time_point time_of_day = vtr::common::timing::toChrono(messages[i].data.time_range.t1);
        std::cout << "Time of Day: " << vtr::common::timing::toIsoString(time_of_day) << std::endl;

    }
    rclcpp::shutdown();
    return 0;
}