#include "vtr_db_extractor/utils.hpp"


void store_messages_to_file( std::vector<vtr_pose_graph_msgs::msg::Vertex> Vertices, const std::string& output_dir, int max_messages = 5) {
    // Create output directory if it doesn't exist
    fs::create_directories(output_dir);
    std::cout << "-----" << std::endl;

    std::string filename = output_dir + "/graph_vertex" + std::to_string(0).insert(0, 4 - std::to_string(0).length(), '0') + ".txt";
        
    std::cout << "Storing message " << 1 << " to file: " << filename << std::endl;
    std::ofstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < max_messages; ++i) {
            vtr_pose_graph_msgs::msg::Vertex vertex = Vertices[i];

            file << vertex.id << std::endl;
        }


        file.close();
    } else {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
}


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/vertices";
    std::string bag_name = "vertices_0.db3";
    std::string output_directory = "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/graph_vertices";
    

    BagExtractor extractor(bag_directory, bag_name);
    vtr_pose_graph_msgs::msg::Vertex temp_msg;
    auto messages = extractor.extract_messages(-1, "vertices",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;

    std::vector<vtr_pose_graph_msgs::msg::Vertex> graph_vertices;
    // VertexBagExtractor extractor(bag_directory);
    // auto messages = extractor.extract_messages(-1, "vertices");
    // std::cout << "Found " << messages.size() << " messages" << std::endl;

    for (size_t i = 0; i < messages.size(); ++i) {
        graph_vertices.push_back(messages[i].data);
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        uint64_t temp = messages[i].data.vertex_time.nanoseconds_since_epoch;
        std::cout << "Timestamp: " << temp << std::endl;
        std::cout << "Time Range : " << messages[i].data.time_range.t1 << " , " << messages[i].data.time_range.t2  << std::endl;
        std::cout << "Vertex ID : " << messages[i].data.id << std::endl;

        vtr::common::timing::time_point time_of_day = vtr::common::timing::toChrono(messages[i].data.time_range.t1);
        std::cout << "Time of Day: " << vtr::common::timing::toIsoString(time_of_day) << std::endl;

    }
    clearOutputFolder(output_directory); 
    std::cout << "Storing messages to file..." << std::endl;
    store_messages_to_file(graph_vertices, output_directory, messages.size());

    rclcpp::shutdown();
    return 0;
}