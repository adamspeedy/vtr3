#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "vtr_navigation_msgs/msg/mission_command.hpp"
#include "vtr_navigation_msgs/msg/goal_handle.hpp"
#include "vtr_scene_graph_interface/utils.hpp"
// #include <matplot/matplot.h>


using MissionCommandMsg = vtr_navigation_msgs::msg::MissionCommand;


class AddGoalPublisher : public rclcpp::Node
{
public:
    AddGoalPublisher() : Node("add_goal_publisher")
    {

        this->declare_parameter("vertex", 0);
        this->declare_parameter("goal3d", std::vector<double>{0.0, 0.0, 0.0});
        this->declare_parameter("goal2d", std::vector<double>{0.0, 0.0});

        vertex_ = this->get_parameter("vertex").as_int();
        goal3_ = this->get_parameter("goal3d").as_double_array();
        goal2_ = this->get_parameter("goal2d").as_double_array();

        if (vertex_!=0)
        {
            RCLCPP_INFO(this->get_logger(), "Vertex %ld recieved", vertex_);
        }
        else if (goal3_!=std::vector<double>{0.0, 0.0, 0.0})
        {
            RCLCPP_INFO(this->get_logger(), "looking for best location");
            // get_closest_vertex();
            std::vector<Eigen::Matrix4d> positions_ = get_vertices();
            vertex_ = getClosestVertex3d(positions_, goal3_);
            // visualize(positions_, goal3_);
        }
        else if (goal2_!=std::vector<double>{0.0, 0.0})
        {
            RCLCPP_INFO(this->get_logger(), "looking for best location");
            // get_closest_vertex();
            std::vector<Eigen::Matrix4d> positions_ = get_vertices();
            vertex_ = getClosestVertex2d(positions_, goal2_);
            // visualize(positions_, goal2_);
        }

        auto qos = rclcpp::QoS(rclcpp::KeepLast(10));
        qos.reliable();                   
        qos.transient_local();         
        qos.deadline(std::chrono::seconds(5));

        // publisher_ = this->create_publisher<std_msgs::msg::String>("my_topic", qos);
        publisher_ = this->create_publisher<MissionCommandMsg>("/vtr/mission_command", qos);

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&AddGoalPublisher::publish_and_exit, this));
            
        start_time_ = this->now();
    }


private:
    void publish_and_exit()
    {
        auto elapsed = this->now() - start_time_;
        
        if (elapsed.seconds() < 1.0) {
            RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, 
                               "Waiting for discovery... (%.1fs)", elapsed.seconds());
            return;
        }
        
        if (publisher_->get_subscription_count() == 0) {
            RCLCPP_WARN(this->get_logger(), "No subscribers found, publishing anyway...");
        }
        
        publish_goal(vertex_);
        
        rclcpp::sleep_for(std::chrono::milliseconds(500));
        
        RCLCPP_INFO(this->get_logger(), "Message published, shutting down...");
        timer_->cancel();
        rclcpp::shutdown();
    }

    void publish_goal(uint64_t vertex_id)
    {
        MissionCommandMsg msg;
        msg.type = MissionCommandMsg::ADD_GOAL;
        msg.vertex = 0;
        msg.pause = false;
        msg.goal_handle.type = vtr_navigation_msgs::msg::GoalHandle::REPEAT;
        msg.goal_handle.waypoints = {vertex_id};

        RCLCPP_INFO(this->get_logger(), "Publishing ADD_GOAL message for vertex %ld", vertex_id);
        publisher_->publish(msg);
    }

    rclcpp::Publisher<MissionCommandMsg>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_time_;
    uint64_t vertex_;
    std::vector<double> goal2_;
    std::vector<double> goal3_;
    std::string bag_directory = "/home/adam/Desktop/CurrentBranch/graph/edges";
    std::string bag_name = "edges_0.db3";



    std::vector<Eigen::Matrix4d> get_vertices()
    {
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

            if (messages[i].data.type.type==0 && messages[i].data.to_id<10000)
            {
                from_vertex_ids.push_back(messages[i].data.from_id);
                to_vertex_ids.push_back(messages[i].data.to_id);
                edge_types.push_back(messages[i].data.type.type);

                std::vector<double> temp = messages[i].data.t_to_from.xi;

                Eigen::Matrix<double, 6, 1> eigen_vec;
                for (int j=0;j<6; j++) {
                    eigen_vec(j)=temp[j];
                }
                auto msg = lgmath::se3::TransformationWithCovariance(Eigen::Matrix<double, 6, 1>(eigen_vec));
                transformed_messages.push_back(msg);
            }
        }
        //sorting the edges into the correct order, idx has the indeces of the correct order
        std::vector<size_t> idx(from_vertex_ids.size());
        std::iota(idx.begin(), idx.end(), 0); 
        std::sort(idx.begin(), idx.end(), [&](size_t i, size_t j) {
            return from_vertex_ids[i] < from_vertex_ids[j];
        });
        std::cout << "Vertices: " << from_vertex_ids.size() << std::endl;
        for (size_t k = 0; k < 5; ++k) {
            std::cout << "idx " << k << " -> " <<idx[k]  << std::endl;
        }
        std::vector<Eigen::Matrix4d> positions;
        // positions.push_back({0.0 , 0.0 , 0.0});
        Eigen::Matrix4d I4 = Eigen::Matrix4d::Identity();
        // auto identity = lgmath::se3::TransformationWithCovariance(Eigen::Matrix<double, 4, 4>(I4));
        auto identity = Eigen::Matrix<double, 4, 4>(I4);
        identity(0, 0) = -1;
        positions.push_back(identity);
        for (size_t j = 1; j < transformed_messages.size(); ++j){
            auto calc= positions[j-1]*transformed_messages[j].matrix();
            positions.push_back(calc);
            // std::cout << "Position " << j << ": \n" << positions[j-1] << std::endl;
        }
        return positions;
    }

    int getClosestVertex3d(std::vector<Eigen::Matrix4d> positions, std::vector<double> goal)
    {
        double min_distance = std::numeric_limits<double>::max();
        int closest_vertex = -1;

        for (size_t i = 0; i < positions.size(); ++i) {
            double dx = positions[i](0, 3) - goal[0];
            double dy = positions[i](1, 3) - goal[1];
            double dz = positions[i](2, 3) - goal[2];
            double distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            if (distance < min_distance) {
                min_distance = distance;
                closest_vertex = static_cast<int>(i);
            }
        }
        closest_vertex+=1; //edges to vertices 

        RCLCPP_INFO(this->get_logger(), "Closest vertex to goal is %d with distance %.3f", closest_vertex, min_distance);
        return closest_vertex;
    }
    
    int getClosestVertex2d(std::vector<Eigen::Matrix4d> positions, std::vector<double> goal)
    {
        double min_distance = std::numeric_limits<double>::max();
        int closest_vertex = -1;

        for (size_t i = 0; i < positions.size(); ++i) {
            double dx = positions[i](0, 3) - goal[0];
            double dy = positions[i](1, 3) - goal[1];
            double distance = std::sqrt(dx * dx + dy * dy);

            if (distance < min_distance) {
                min_distance = distance;
                closest_vertex = static_cast<int>(i);
            }
        }
        closest_vertex+=1; //edges to vertices 
        RCLCPP_INFO(this->get_logger(), "Closest vertex to goal is %d with distance %.3f", closest_vertex, min_distance);
        return closest_vertex;
    }

    // void visualize(std::vector<Eigen::Matrix4d> positions, std::vector<double> goal)
    // {
    //     std::vector<double> x = {1.0,2.0,3.0};
    //     std::vector<double> y = {1.0,2.0,3.0};
    //     matplot::scatter(x, y);
    //     matplot::show();
    //     // plt::plot(x, y, "r-");  // red line
    //     // plt::scatter(x, y, 50); // scatter points
    //     // plt::xlabel("X");
    //     // plt::ylabel("Y");
    //     // plt::title("Trajectory");

    //     // // Show popup window
    //     // plt::show();
    // }

};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AddGoalPublisher>());
    rclcpp::shutdown();
    return 0;
}