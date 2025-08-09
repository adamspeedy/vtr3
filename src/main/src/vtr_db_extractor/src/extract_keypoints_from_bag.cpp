#include "vtr_db_extractor/utils.hpp"


void store_messages_to_file( std::vector<vtr_vision_msgs::msg::ChannelObservations> channelObservations, const std::string& output_dir, int max_messages = 5) {
    // Create output directory if it doesn't exist
    fs::create_directories(output_dir);
    std::cout << "-----" << std::endl;
    
    // Iterate through the messages
    for (int i = 0; i < max_messages; ++i) {
        
        std::vector<vtr_vision_msgs::msg::Observations> observations = channelObservations[i].cameras;
        std::vector<vtr_vision_msgs::msg::Keypoint> kp1 = channelObservations[i].cameras[0].keypoints;
        std::vector<vtr_vision_msgs::msg::Keypoint> kp2 = channelObservations[i].cameras[0].keypoints;
        geometry_msgs::msg::Vector3 points1 = channelObservations[i].cameras[0].keypoints[0].position;
        geometry_msgs::msg::Vector3 points2 = channelObservations[i].cameras[1].keypoints[0].position;

        std::cout << "Number of cameras: " << observations.size() << std::endl;
        std::cout << "Number of keypoints 1: " << kp1.size() << std::endl;
        std::cout << "Number of keypoints 2: " << kp2.size() << std::endl;
        std::cout << "Point 1: " << points1.x << std::endl;
        std::cout << "Point 2: " << points2.x << std::endl;



        // std::vector<vtr::vision_msgs::Observations> observations = Observations[i].landmarks;
        // std::vector<vtr_vision_msgs::msg::HVec3> vector_points = channelObservations[i].cameras;

        // std::vector<bool> valid = ChannelLandmarks[i].valid;
        
        // Create filename with index
        std::string filename = output_dir + "/keypoint_" + std::to_string(i).insert(0, 4 - std::to_string(i).length(), '0') + ".txt";
        
        std::cout << "Storing message " << i + 1 << " to file: " << filename << std::endl;
        std::ofstream file(filename);
        if (file.is_open()) {

            for (int j =0; j<static_cast<int>(kp1.size()); j++)
            {
                // if (valid[j]) 
                // {
                file << kp1[j].position.x << ", " << kp1[j].position.y << ", " << kp1[j].position.z  << std::endl;
                // }
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
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/data/stereo_observations";
    std::string bag_name = "stereo_observations_0.db3";
    std::string output_directory = "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/keypoint_results";
    
    BagExtractor extractor(bag_directory, bag_name);
    // vtr_vision_msgs::msg::ChannelLandmarks temp_msg ;
    vtr_vision_msgs::msg::RigObservations temp_msg;
    auto messages = extractor.extract_messages(-1, "stereo_observations",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;

    std::vector<vtr_vision_msgs::msg::ChannelObservations> channelObservations;
    // std::vector<vtr_vision_msgs::msg::HVec3> Channelpoints;

    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        channelObservations.push_back(messages[i].data.channels[0]); 
        // Channelpoints.push_back(messages[i].data.channels[0].points);

        // std::cout << "Type: " << typeid(messages[i].data.channels).name() << std::endl;
        // for (const auto& point : messages[i].data.channels[0].points) {
        //     std::cout << "Point: (" << point.x << ", " << point.y << ", " << point.z << ", " << point.w << ")" << std::endl;
        // }
    }
    // Store messages to file
    clearOutputFolder(output_directory); 
    std::cout << "Storing messages to file..." << std::endl;
    store_messages_to_file(channelObservations, output_directory, messages.size());

    rclcpp::shutdown();
    return 0;
}