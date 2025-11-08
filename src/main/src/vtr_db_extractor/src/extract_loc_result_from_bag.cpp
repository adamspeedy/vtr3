#include "vtr_db_extractor/utils.hpp"


void store_messages_to_file(const std::vector<lgmath::se3::TransformationWithCovariance>& messages, std::vector<int> vertex_ids, std::vector<uint64_t> vertex_timestamps,std::vector<uint64_t> timestamps, const std::string& output_dir, int max_messages = 5) {
    // Create output directory if it doesn't exist
    fs::create_directories(output_dir);
    std::cout << "-----" << std::endl;
    
    // Iterate through the messages
    //size_t max_to_process = std::min(messages.size(), static_cast<size_t>(5));
    for (int i = 0; i < max_messages; ++i) {
        //const auto& data = messages[i].data;
        const auto& transform = messages[i];  //data.t_world_robot.xi;
        const auto& vertex_id = vertex_ids[i];
        const auto& vertex_timestamp = vertex_timestamps[i];
        const auto& timestamp = timestamps[i];
        
        // Create filename with index
        std::string filename = output_dir + "/T_r_vertex_" + std::to_string(i).insert(0, 4 - std::to_string(i).length(), '0') + ".txt";
        
        // Save the matrix
        std::ofstream file(filename);
        if (file.is_open()) {
            //for (const auto& val : transform) {
            file << transform << std::endl;
            file << "vertex_id: " << vertex_id << std::endl;
            file << "vertex_timestamp: " << vertex_timestamp << std::endl;
            file << "timestamp: " << timestamp << std::endl;
            //}
            file.close();
            // std::cout << "Saved matrix " << i << " to " << filename << std::endl;
        } else {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
    }
}


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/data/localization_result";
    std::string output_directory = "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/loc_results";
    std::string bag_name = "localization_result_0.db3";
    
    BagExtractor extractor(bag_directory, bag_name);
    vtr_tactic_msgs::msg::LocalizationResult temp_msg;
    auto messages = extractor.extract_messages(-1, "localization_result",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;

    // EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    std::vector<lgmath::se3::TransformationWithCovariance> transformed_messages;
    std::vector<uint64_t> timestamps;
    std::vector<uint64_t> vertex_timestamps;
    std::vector<int> vertex_ids;
    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        std::cout << "Timestamp: " << messages[i].data.vertex_timestamp << std::endl;
        vertex_timestamps.push_back(messages[i].data.vertex_timestamp);
        timestamps.push_back(messages[i].data.timestamp);
        std::cout << "Vertex ID: " << messages[i].data.vertex_id << std::endl;
        vertex_ids.push_back(messages[i].data.vertex_id);
        std::vector<double> temp = messages[i].data.t_robot_vertex.xi;

        bool tempbool = messages[i].data.t_robot_vertex.cov_set;
        std::cout << "Covariance set: " << (tempbool ? "true" : "false") << std::endl;

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
    
    // Store messages to file
    clearOutputFolder(output_directory); 
    std::cout << "Storing messages to file..." << std::endl;
    store_messages_to_file(transformed_messages, vertex_ids, vertex_timestamps, timestamps, output_directory, messages.size());

    
    
    if (messages.size() > 10) {
        std::cout << "\n... and " << (messages.size() - 10) << " more messages" << std::endl;
        std::cout << "stored images" << std::endl;
    }
    rclcpp::shutdown();
    return 0;
}