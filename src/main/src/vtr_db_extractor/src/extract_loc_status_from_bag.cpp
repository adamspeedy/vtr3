#include "vtr_db_extractor/utils.hpp"



void store_messages_to_file( std::vector<uint64_t> keyframe_time_, std::vector<uint64_t> query_id_ ,std::vector<uint64_t> map_id_, 
    std::vector<bool> loc_success_, std::vector<double> comp_time_ , std::vector<uint32_t> temporal_depth_, 
    std::vector<uint32_t> window_num_vertices_, std::vector<std::vector<uint32_t>>  inliers_ , const std::string& output_dir_, int max_messages_ = 5)
    {
    fs::create_directories(output_dir_);
    std::cout << "-----" << std::endl;
    
    for (int i = 0; i < max_messages_; ++i) {
        // const auto& transform = messages_[i];  //data.t_world_robot.xi;
        const auto& keyframe_time = keyframe_time_[i];
        const auto& query_id = query_id_[i];
        const auto& map_id = map_id_[i];
        const auto& loc_success = loc_success_[i];
        const auto& comp_time = comp_time_[i];
        const auto& temporal_depth = temporal_depth_[i];
        const auto& window_num_vertices = window_num_vertices_[i];
        const auto& inliers = inliers_[i];
        
        std::string filename = output_dir_ + "/loc_" + std::to_string(i).insert(0, 5- std::to_string(i).length(), '0') + ".txt";
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "keyframe_time: " << keyframe_time << std::endl;
            file << "query_id: " << query_id << std::endl;
            file << "map_id: " << map_id << std::endl;
            file << "loc_success: " << loc_success << std::endl;
            file << "comp_time: " << comp_time << std::endl;
            file << "temporal_depth: " << temporal_depth << std::endl;
            file << "window_num_vertices: " << window_num_vertices << std::endl;
            for (int l=0; l<static_cast<int>(inliers.size()); l++)
            {
                file << "inliers: " <<  inliers[l] << std::endl;
            }
            file.close();
        } else {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
    }
}


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/data/results_localization";
    std::string output_directory = "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/loc_status";
    std::string bag_name = "results_localization_0.db3";
    
    BagExtractor extractor(bag_directory, bag_name);
    vtr_vision_msgs::msg::LocalizationStatus temp_msg;
    auto messages = extractor.extract_messages(-1, "results_localization",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;

    // EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    // std::vector<lgmath::se3::TransformationWithCovariance> transformed_messages;
    std::vector<uint64_t> keyframe_time;
    std::vector<uint64_t> query_id;
    std::vector<uint64_t> map_id;
    std::vector<bool> loc_success;
    std::vector<std::vector<uint32_t>> inliers;    
    std::vector<double> comp_time;
    std::vector<uint32_t> temporal_depth;
    std::vector<uint32_t> window_num_vertices;

    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;

        std::cout << "Keyframe time " << messages[i].data.keyframe_time << std::endl;
        keyframe_time.push_back(messages[i].data.keyframe_time);

        query_id.push_back(messages[i].data.query_id);
        std::cout << "Query ID: " << messages[i].data.query_id << std::endl;

        map_id.push_back(messages[i].data.map_id);
        std::cout << "Map ID: " << messages[i].data.map_id << std::endl;

        loc_success.push_back(messages[i].data.success);
        std::cout << "Success Flag: " << messages[i].data.success << std::endl;

        comp_time.push_back(messages[i].data.localization_computation_time_ms);
        std::cout << "Comp Time Flag: " << messages[i].data.localization_computation_time_ms << std::endl;

        temporal_depth.push_back(messages[i].data.window_temporal_depth);
        std::cout << "Window Temporal Depth: " << messages[i].data.window_temporal_depth << std::endl;

        window_num_vertices.push_back(messages[i].data.window_num_vertices);
        std::cout << "Window num Vertices: " << messages[i].data.window_num_vertices << std::endl;

        inliers.push_back(messages[i].data.inlier_channel_matches);
        for (int k=0; k< 5; k++) {
            std::cout << "Inlier" << k << ": " << messages[i].data.inlier_channel_matches[k] << std::endl;
        }
        // std::cout << "Inlier: " << messages[i].data.inlier_channel_matches << std::endl;

         // This detail is commended out for most graphs, come back and double check if we are saving the results
        // std::vector<double> temp = messages[i].data.t_query_map.xi;
        // bool tempbool = messages[i].data.t_query_map.cov_set;
        // std::cout << "Covariance set: " << (tempbool ? "true" : "false") << std::endl;

        // std::cout << "Elements of T_q_m: ";
        // Eigen::Matrix<double, 6, 1> eigen_vec;
        // for (int j=0;j<6; j++) {
        //     std::cout << temp[j] << " ";
        //     eigen_vec(j)=temp[j];
        // }
        // std::cout << "end" << std::endl;
        // std::cout << std::endl;
        // auto msg = lgmath::se3::TransformationWithCovariance(Eigen::Matrix<double, 6, 1>(eigen_vec));
        // transformed_messages.push_back(msg);
        // std::cout << msg << std::endl;
        //std::cout << "message" << temp  << std::endl;
        std::cout << "Fin" << std::endl;
    }
    
    // Store messages to file
    clearOutputFolder(output_directory); 
    std::cout << "Storing messages to file..." << std::endl;
    store_messages_to_file(keyframe_time, query_id, map_id, loc_success, comp_time, 
        temporal_depth, window_num_vertices, inliers ,output_directory  ,messages.size());

    if (messages.size() > 10) {
        std::cout << "\n... and " << (messages.size() - 10) << " more messages" << std::endl;
        std::cout << "stored images" << std::endl;
    }
    rclcpp::shutdown();
    return 0;
}