#include "vtr_db_extractor/utils.hpp"


namespace fs = std::filesystem;


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/vertices";
    std::string bag_name = "vertices_0.db3";


    BagExtractor extractor(bag_directory, bag_name);
    vtr_pose_graph_msgs::msg::Vertex temp_msg;
    auto messages = extractor.extract_messages(-1, "vertices",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;


    // VertexBagExtractor extractor(bag_directory);
    // auto messages = extractor.extract_messages(-1, "vertices");
    // std::cout << "Found " << messages.size() << " messages" << std::endl;

    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        uint64_t temp = messages[i].data.vertex_time.nanoseconds_since_epoch;
        std::cout << "Timestamp: " << temp << std::endl;
        std::cout << "Time Range : " << messages[i].data.time_range.t1 << " , " << messages[i].data.time_range.t2  << std::endl;
        std::cout << "Vertex ID : " << messages[i].data.id << std::endl;

        vtr::common::timing::time_point time_of_day = vtr::common::timing::toChrono(messages[i].data.time_range.t1);
        std::cout << "Time of Day: " << vtr::common::timing::toIsoString(time_of_day) << std::endl;

    }
    rclcpp::shutdown();
    return 0;
}