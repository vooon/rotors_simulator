/*
 * Copyright 2015 Fadri Furrer, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Michael Burri, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Mina Kamel, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Janosch Nikolic, ASL, ETH Zurich, Switzerland
 * Copyright 2015 Markus Achtelik, ASL, ETH Zurich, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef ROTORS_GAZEBO_PLUGINS_MAVLINK_INTERFACE_H
#define ROTORS_GAZEBO_PLUGINS_MAVLINK_INTERFACE_H

#include <boost/bind.hpp>
#include <Eigen/Eigen>
#include <gazebo/common/common.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <mav_msgs/CommandMotorSpeed.h>
#include <mav_msgs/MotorSpeed.h>
#include <ros/callback_queue.h>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <stdio.h>

#include "rotors_gazebo_plugins/common.h"
#include <mavros/utils.h>

namespace gazebo {

// Default values
static const std::string kDefaultNamespace = "";

// This just proxies the motor commands from command/motor_speed to the single motors via internal
// ConsPtr passing, such that the original commands don't have to go n_motors-times over the wire.
static const std::string kDefaultMotorVelocityReferencePubTopic = "gazebo/command/motor_speed";
static const std::string kDefaultMavlinkControlSubTopic = "mavlink/to";

static const std::string kDefaultImuTopic = "imu";
static const std::string kDefaultMavlinkHilSensorPubTopic = "mavlink/from";

class GazeboMavlinkInterface : public ModelPlugin {
 public:
  GazeboMavlinkInterface()
      : ModelPlugin(),
        received_first_referenc_(false),
        namespace_(kDefaultNamespace),
        motor_velocity_reference_pub_topic_(kDefaultMotorVelocityReferencePubTopic),
        hil_sensor_mavlink_pub_topic_(kDefaultMavlinkHilSensorPubTopic),
        imu_sub_topic_(kDefaultImuTopic),
        mavlink_control_sub_topic_(kDefaultMavlinkControlSubTopic),
        node_handle_(NULL){}
  ~GazeboMavlinkInterface();

  void Publish();

 protected:
  void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf);
  void OnUpdate(const common::UpdateInfo& /*_info*/);

 private:

  bool received_first_referenc_;
  Eigen::VectorXd input_reference_;

  std::string namespace_;
  std::string motor_velocity_reference_pub_topic_;
  std::string mavlink_control_sub_topic_;
  std::string link_name_;

  ros::NodeHandle* node_handle_;
  ros::Publisher motor_velocity_reference_pub_;
  ros::Subscriber mav_control_sub_;

  physics::ModelPtr model_;
  physics::WorldPtr world_;

  /// \brief Pointer to the update event connection.
  event::ConnectionPtr updateConnection_;

  boost::thread callback_queue_thread_;
  void QueueThread();
  void CommandMotorMavros(const mav_msgs::CommandMotorSpeedPtr& input_reference_msg);
  void MavlinkControlCallback(const mavros::Mavlink::ConstPtr &rmsg);
  void ImuCallback(const sensor_msgs::ImuConstPtr& imu_msg);


  unsigned _rotor_count;
  struct {
    float control[8];
  } inputs; 

  ros::Subscriber imu_sub_;
  ros::Publisher hil_sensor_pub_;

  std::string hil_sensor_mavlink_pub_topic_;
  std::string imu_sub_topic_;
  
  common::Time last_time_;
  common::Time last_gps_time_;
  double gps_update_interval_;

  mavlink_hil_sensor_t hil_sensor_msg_;
  mavlink_hil_gps_t hil_gps_msg_;

  math::Vector3 gravity_W_;
  math::Vector3 velocity_prev_W_;
  math::Vector3 mag_W_; 
};
}

#endif // ROTORS_GAZEBO_PLUGINS_MAVLINK_INTERFACE_H
