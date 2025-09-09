#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "vtr_navigation_msgs/msg/mission_command.hpp"
#include "vtr_navigation_msgs/msg/goal_handle.hpp"

using MissionCommandMsg = vtr_navigation_msgs::msg::MissionCommand;


class RepeatPublisher : public rclcpp::Node
{
public:
    RepeatPublisher() : Node("add_goal_publisher")
    {
        auto qos = rclcpp::QoS(rclcpp::KeepLast(10));
        qos.reliable();                   
        qos.transient_local();         
        qos.deadline(std::chrono::seconds(5));

        // publisher_ = this->create_publisher<std_msgs::msg::String>("my_topic", qos);
        publisher_ = this->create_publisher<MissionCommandMsg>("/vtr/mission_command", qos);

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&RepeatPublisher::publish_and_exit, this));
            
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
        
        publish_goal(50);
        
        rclcpp::sleep_for(std::chrono::milliseconds(500));
        
        RCLCPP_INFO(this->get_logger(), "Message published, shutting down...");
        timer_->cancel();
        rclcpp::shutdown();
    }

    void publish_goal(int vertex_id)
    {
        MissionCommandMsg msg;
        msg.type = MissionCommandMsg::BEGIN_GOALS;
        msg.vertex = 0;
        msg.pause = false;
        msg.goal_handle.type = vtr_navigation_msgs::msg::GoalHandle::IDLE;
        msg.goal_handle.waypoints = {vertex_id};

        RCLCPP_INFO(this->get_logger(), "Publishing ADD_GOAL message for vertex %d", vertex_id);
        publisher_->publish(msg);
    }

    rclcpp::Publisher<MissionCommandMsg>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_time_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<RepeatPublisher>());
    rclcpp::shutdown();
    return 0;
}
























// int main(int argc, char * argv[])
// {
//     rclcpp::init(argc, argv);
//     auto node = rclcpp::Node::make_shared("command_publisher");
//     auto publisher = node->create_publisher<MissionCommandMsg>("/vtr/mission_command", 10);

    
//     MissionCommandMsg msg = MissionCommandMsg();
//     // msg.type = MissionCommandMsg::ADD_GOAL;
//     // msg.vertex =0;
//     // msg.pause = false;
//     // msg.goal_handle.type = vtr_navigation_msgs::msg::GoalHandle::REPEAT;
//     // msg.goal_handle.waypoints = {50};

//     // RCLCPP_INFO(node->get_logger(), "Publishing message");
//     // publisher->publish(msg);

//     msg.type = MissionCommandMsg::BEGIN_GOALS;
//     msg.vertex =0;
//     msg.pause = false;
//     msg.goal_handle.type = vtr_navigation_msgs::msg::GoalHandle::IDLE;
//     msg.goal_handle.waypoints = {0};

//     RCLCPP_INFO(node->get_logger(), "Publishing message");
//     publisher->publish(msg);

//     // Give ROS time to send before shutdown
//     rclcpp::spin_some(node);

//     rclcpp::shutdown();
//     return 0;
// }