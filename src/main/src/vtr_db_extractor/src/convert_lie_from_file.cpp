#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "lgmath.hpp"


#include "vtr_common_msgs/msg/lie_group_transform.hpp"
#include "vtr_common/conversions/ros_lgmath.hpp"
#include "vtr_common/conversions/tf2_ros_eigen.hpp"
#include "vtr_tactic_msgs/msg/odometry_result.hpp"

#include "vtr_common_msgs/msg/lie_group_transform.hpp"


Eigen::Matrix<double, 6, 1> readMatrix6x1FromFile(const std::string& filename) {
    Eigen::Matrix<double, 6, 1> matrix;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return matrix;
    }
    // Temporary vector to store values
    std::vector<double> values;

    // Read the entire file into a string
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        double value;
        
        // Extract values from the line
        while (ss >> value) {
            values.push_back(value);
            
            // Skip any delimiter (comma, space, etc.)
            if (ss.peek() == ',' || ss.peek() == ' ') {
                ss.ignore();
            }
        }
    }
    for (int i = 0; i < 6; ++i) {
        matrix(i) = values[i];
    }
    
    file.close();
    return matrix;
}




int main(int argc, char* argv[]) {
    std::string filename;
    
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cout << "Enter the filename: ";
        std::cin >> filename;
    }
    
    Eigen::Matrix<double, 6, 1> matrix = readMatrix6x1FromFile(filename);
    
    // Display the array
    std::cout << "Array read from file:" << std::endl;
    std::cout << "Matrix read from file:" << std::endl;
    std::cout << matrix << std::endl;


    // vtr_common_msgs::msg::LieGroupTransform temp;
    // temp.xi = array;
    // temp.cov_set = false;



    auto msg = lgmath::se3::TransformationWithCovariance(Eigen::Matrix<double, 6, 1>(matrix));
    std::cout << msg << std::endl;

//   else {
//     Eigen::Matrix<double, 6, 6> cov;
//     for (int row = 0; row < 6; ++row)
//       for (int col = 0; col < 6; ++col) cov(row, col) = msg.cov[row * 6 + col];
//     return lgmath::se3::TransformationWithCovariance(Eigen::Matrix<double, 6, 1>(msg.xi.data()), cov);
//   }


    
    //common::conversions::fromROSMsg(storable.t_robot_vertex, T_robot_vertex);




    return 0;
}