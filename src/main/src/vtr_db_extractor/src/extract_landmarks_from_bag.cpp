#include "vtr_db_extractor/utils.hpp"


void store_messages_to_file( std::vector<vtr_vision_msgs::msg::ChannelLandmarks> ChannelLandmarks, const std::string& output_dir, int max_messages = 5) {
    // Create output directory if it doesn't exist
    fs::create_directories(output_dir);
    std::cout << "-----" << std::endl;
    
    // Iterate through the messages
    for (int i = 0; i < max_messages; ++i) {

        std::vector<vtr_vision_msgs::msg::HVec3> vector_points = ChannelLandmarks[i].points;
        std::vector<bool> valid = ChannelLandmarks[i].valid;
        
        // Create filename with index
        std::string filename = output_dir + "/points_" + std::to_string(i).insert(0, 4 - std::to_string(i).length(), '0') + ".txt";
        
        std::cout << "Storing message " << i + 1 << " to file: " << filename << std::endl;
        std::ofstream file(filename);
        if (file.is_open()) {

            for (int j =0; j<int(valid.size()); j++)
            {
                if (valid[j]) 
                {
                    file << vector_points[j].x << ", " << vector_points[j].y << ", " << vector_points[j].z  << std::endl;
                }
            } 

            file.close();
            // std::cout << "Saved matrix " << i << " to " << filename << std::endl;
        } else {
            std::cerr << "Failed to open file: " << filename << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/data/stereo_landmarks";
    std::string bag_name = "stereo_landmarks_0.db3";
    std::string output_directory = "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/landmark_results";
    
    BagExtractor extractor(bag_directory, bag_name);
    // vtr_vision_msgs::msg::ChannelLandmarks temp_msg ;
    vtr_vision_msgs::msg::RigLandmarks temp_msg;
    auto messages = extractor.extract_messages(-1, "stereo_landmarks",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;

    std::vector<vtr_vision_msgs::msg::ChannelLandmarks> channelLandmarks;
    // std::vector<vtr_vision_msgs::msg::HVec3> Channelpoints;

    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        channelLandmarks.push_back(messages[i].data.channels[0]);
        // Channelpoints.push_back(messages[i].data.channels[0].points);

        // std::cout << "Type: " << typeid(messages[i].data.channels).name() << std::endl;
        // for (const auto& point : messages[i].data.channels[0].points) {
        //     std::cout << "Point: (" << point.x << ", " << point.y << ", " << point.z << ", " << point.w << ")" << std::endl;
        // }
    }
    // Store messages to file
    clearOutputFolder(output_directory); 
    std::cout << "Storing messages to file..." << std::endl;
    store_messages_to_file(channelLandmarks, output_directory, messages.size());

    rclcpp::shutdown();
    return 0;
}