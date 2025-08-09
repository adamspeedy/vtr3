#include "vtr_db_extractor/utils.hpp"



int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/data/stereo_visualization_images";
    std::string bag_name = "stereo_visualization_images_0.db3";
    std::string output_directory = "/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/images";

    BagExtractor extractor(bag_directory, bag_name);
    // vtr_vision_msgs::msg::ChannelLandmarks temp_msg ;
    vtr_vision_msgs::msg::Image temp_msg;
    auto messages = extractor.extract_messages(-1, "stereo_visualization_images",  temp_msg);
    std::cout << "Found " << messages.size() << " messages" << std::endl;


    clearOutputFolder(output_directory); 

    for (size_t i = 0; i < messages.size(); ++i) {
        std::cout << "\nMessage " << i+1 << ":" << std::endl;
        //std::cout << "Timestamp: " << messages[i].data.stamp << std::endl;
        // std::cout << "resolution : " << messages[i].data.height << " , " << messages[i].data.width  << std::endl;
        // std::cout << "encoding : " << messages[i].data.encoding << std::endl;
        // std::cout << "data: " << messages[i].data.data.size() << std::endl;

        auto image_msg = sensor_msgs::msg::Image();
        image_msg.header.frame_id = "camera_frame";
        image_msg.height = messages[i].data.height;
        image_msg.width = messages[i].data.width;
        image_msg.encoding = messages[i].data.encoding;
        image_msg.is_bigendian = messages[i].data.is_bigendian;
        image_msg.step = messages[i].data.step;
        image_msg.data = messages[i].data.data;

        uint64 temp = messages[i].data.stamp.nanoseconds_since_epoch;
        std::cout << "Timestamp: " << temp << std::endl;

        try {
            // cv::Mat cv_image= cv::Mat(cv::Size(image_msg.width, image_msg.height), CV_8UC3, (void *)data.data());
            cv::Mat image(cv::Size(image_msg.width, image_msg.height), CV_8UC3);
            std::memcpy(image.data, image_msg.data.data(), image_msg.data.size());

            // SAVE IMAGE
            std::string filename = output_directory + "/image_" + std::to_string(i).insert(0, 4 - std::to_string(i).length(), '0') + ".png";
            cv::imwrite(filename, image);

            // cv::imshow("Image", image);
            // cv::waitKey(0);

            
        } catch (cv_bridge::Exception& e) {
            std::cerr << "cv_bridge exception: " << e.what() << std::endl;
        }
        }

    rclcpp::shutdown();
    return 0;
}