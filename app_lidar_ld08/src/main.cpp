// Copyright 2021 ROBOTIS CO., LTD.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: LD Robot, Will Son

#include <stdio.h>
#include <iostream>
#include <ufr.h>
#include <cstring>

#include "../include/cmd_interface_linux.h"
#include "../include/lipkg.h"
#include "../include/transform.h"


int main(int argc, char ** argv)
{
  //fgb rclcpp::init(argc, argv);
  //fgb auto node = rclcpp::Node::make_shared("laser_scan_publisher");
  //fgb rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr lidar_pub;

  LiPkg * pkg;
  std::string product;
  int32_t ver = 8;
  pkg = new LD08_LiPkg;

  CmdInterfaceLinux cmd_port(ver);
  std::vector<std::pair<std::string, std::string>> device_list;
  std::string port_name;
  cmd_port.GetCmdDevices(device_list);
  for (auto n : device_list) {
    std::cout << n.first << "    " << n.second << std::endl;
    if (strstr(n.second.c_str(), "CP2102")) {
      port_name = n.first;
    }
  }

  if (port_name.empty() == false) {
    std::cout << "FOUND LDS-02" << product << std::endl;
    cmd_port.SetReadCallback(
      [&pkg](const char * byte, size_t len) {
        if (pkg->Parse((const uint8_t *)(byte), len)) {
          pkg->AssemblePacket();
        }
      });

    if (cmd_port.Open(port_name)) {
      std::cout << "LDS-02" << product << " started successfully " << std::endl;
    }

    // char topic_name[20]={0};
    // strcat(topic_name,product.c_str());
    // strcat(topic_name,"/LDLiDAR");
    //fgb lidar_pub = node->create_publisher<sensor_msgs::msg::LaserScan>(
    //fgb   "scan", rclcpp::QoS(rclcpp::SensorDataQoS())
    //fgb );

    link_t pub = ufr_publisher("@new ros_humble @coder ros_humble:laser_scan @topic /scan");

    while ( ufr_loop_ok() ) {
      if (pkg->IsFrameReady()) {
        // pkg->setStamp(node->now());
        // lidar_pub->publish(pkg->GetLaserScan());
        printf("opa\n");
        pkg->ResetFrameReady();
      }
    }
  } else {
    std::cout << "Can't find LDS-02" << product << std::endl;
  }

  return 0;
}
