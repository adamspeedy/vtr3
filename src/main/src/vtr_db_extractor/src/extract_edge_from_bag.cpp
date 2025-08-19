#include "vtr_db_extractor/utils.hpp"


void store_messages_to_file(const std::vector<lgmath::se3::TransformationWithCovariance>& messages, std::vector<uint64_t> from_vertex_ids, std::vector<uint64_t> to_vertex_ids, std::vector<uint32_t> edge_types, const std::string& output_dir, int max_messages = 5) {
    // Create output directory if it doesn't exist
    fs::create_directories(output_dir);
    std::cout << "-----" << std::endl;
    
    // Iterate through the messages
    //size_t max_to_process = std::min(messages.size(), static_cast<size_t>(5));
    for (int i = 0; i < max_messages; ++i) {
        //const auto& data = messages[i].data;
        const auto& transform = messages[i];  //data.t_world_robot.xi;
        const auto& from_vertex_id = from_vertex_ids[i];
        const auto& to_vertex_id = to_vertex_ids[i];
        const auto& edge_type = edge_types[i];
        
        // Create filename with index
        std::string filename = output_dir + "/edge_" + std::to_string(i).insert(0, 4 - std::to_string(i).length(), '0') + ".txt";
        
        // Save the matrix
        std::ofstream file(filename);
        if (file.is_open()) {
            //for (const auto& val : transform) {
            file << transform << std::endl;
            file << "from_vertex_id: " << from_vertex_id << std::endl;
            file << "to_vertex_id: " << to_vertex_id << std::endl;
            file << "edge_type: " << edge_type << std::endl;
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
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/edges";
    std::string output_directory = "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/edge_results";
    std::string bag_name = "edges_0.db3";

    BagExtractor extractor(bag_directory, bag_name);

    vtr_pose_graph_msgs::msg::Edge temp_msg;
    auto messages = extractor.extract_messages(-1, "edges",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    std::vector<lgmath::se3::TransformationWithCovariance> transformed_messages;
    std::vector<uint64_t> from_vertex_ids;
    std::vector<uint64_t> to_vertex_ids;
    std::vector<uint32_t> edge_types;
    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        std::cout << "From ID: " << messages[i].data.from_id << std::endl;
        from_vertex_ids.push_back(messages[i].data.from_id);
        std::cout << "To ID: " << messages[i].data.to_id << std::endl;
        to_vertex_ids.push_back(messages[i].data.to_id);
        edge_types.push_back(messages[i].data.type.type);
        // uint32_t test_edge = messages[i].data.type.type;
        std::cout << "edge_type: " << messages[i].data.type.type << std::endl;
        // std::cout << "Edge Type: " << std::string(messages[i].data.type.type) << std::endl;

        std::vector<double> temp = messages[i].data.t_to_from.xi;
        bool tempbool = messages[i].data.t_to_from.cov_set;
        std::cout << "Covariance set: " << (tempbool ? "true" : "false") << std::endl;

        Eigen::Matrix<double, 6, 1> eigen_vec;
        for (int j=0;j<6; j++) {
            eigen_vec(j)=temp[j];
        }

        std::cout << std::endl;
        auto msg = lgmath::se3::TransformationWithCovariance(Eigen::Matrix<double, 6, 1>(eigen_vec));
        transformed_messages.push_back(msg);
        std::cout << msg << std::endl;
        //std::cout << "message" << temp  << std::endl;
        std::cout << "Fin" << std::endl;
    }
    
    clearOutputFolder(output_directory); 
    std::cout << "Storing messages to file..." << std::endl;
    store_messages_to_file(transformed_messages, from_vertex_ids, to_vertex_ids, edge_types, output_directory, messages.size());

    
    
    if (messages.size() > 10) {
        std::cout << "\n... and " << (messages.size() - 10) << " more messages" << std::endl;
        std::cout << "stored images" << std::endl;
    }
    rclcpp::shutdown();
    return 0;
}