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

#include "vtr_tactic_msgs/msg/localization_result.hpp"
#include "vtr_common_msgs/msg/lie_group_transform.hpp"
//#define EIGEN_DONT_VECTORIZE

namespace fs = std::filesystem;

class ROS2BagExtractor {
public:
    //EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    ROS2BagExtractor(const std::string& bag_path) : bag_path_(bag_path) {
        std::string db_path = bag_path + "/localization_result_0.db3";
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
    
    ~ROS2BagExtractor() {
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
        vtr_tactic_msgs::msg::LocalizationResult data;
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
        rclcpp::Serialization<vtr_tactic_msgs::msg::LocalizationResult> serialization;
        
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int64_t timestamp = sqlite3_column_int64(stmt, 0);
            const void* data = sqlite3_column_blob(stmt, 1);
            int data_size = sqlite3_column_bytes(stmt, 1);
            
            // Create a ROS message
            vtr_tactic_msgs::msg::LocalizationResult msg;
            
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

void store_messages_to_file(const std::vector<lgmath::se3::TransformationWithCovariance>& messages, const std::string& output_dir, int max_messages = 5) {
    // Create output directory if it doesn't exist
    fs::create_directories(output_dir);
    std::cout << "-----" << std::endl;
    
    // Iterate through the messages
    //size_t max_to_process = std::min(messages.size(), static_cast<size_t>(5));
    for (int i = 0; i < max_messages; ++i) {
        //const auto& data = messages[i].data;
        const auto& transform = messages[i];  //data.t_world_robot.xi;
        
        // Create filename with index
        std::string filename = output_dir + "/odom__" + std::to_string(i).insert(0, 4 - std::to_string(i).length(), '0') + ".txt";
        
        // Save the matrix
        std::ofstream file(filename);
        if (file.is_open()) {
            //for (const auto& val : transform) {
            file << transform << std::endl;
            //}
            file.close();
            std::cout << "Saved matrix " << i << " to " << filename << std::endl;
        } else {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
    }
}


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/data/localization_result";
    ROS2BagExtractor extractor(bag_directory);
    auto messages = extractor.extract_messages(-1, "localization_result");
    std::cout << "Found " << messages.size() << " messages" << std::endl;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    std::vector<lgmath::se3::TransformationWithCovariance> transformed_messages;
    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        std::cout << "Timestamp: " << messages[i].timestamp << std::endl;
        std::vector<double> temp = messages[i].data.t_robot_vertex.xi;
        std::cout << "Elements of temp: ";
        Eigen::Matrix<double, 6, 1> eigen_vec;
        for (int j=0;j<6; j++) {
            std::cout << temp[j] << " ";
            eigen_vec(j)=temp[j];
        }
        std::cout << "end" << std::endl;
        std::cout << std::endl;
        auto msg = lgmath::se3::TransformationWithCovariance(Eigen::Matrix<double, 6, 1>(eigen_vec));
        transformed_messages.push_back(msg);
        std::cout << msg << std::endl;
        //std::cout << "message" << temp  << std::endl;
        std::cout << "Fin" << std::endl;
    }
    if (messages.size() > 10) {
        std::cout << "\n... and " << (messages.size() - 10) << " more messages" << std::endl;
    }
    // Store messages to file
    // std::cout << "Storing messages to file..." << std::endl;
    // store_messages_to_file(transformed_messages, "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/odom_poses", messages.size());

    rclcpp::shutdown();
    return 0;
}