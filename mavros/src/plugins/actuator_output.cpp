/**
 * @brief ActuatorControl plugin
 * @file actuator_control.cpp
 * @author Marcel Stüttgen <stuettgen@fh-aachen.de>
 *
 * @addtogroup plugin
 * @{
 */
/*
 * Copyright 2015 Marcel Stüttgen <stuettgen@fh-aachen.de>
 *
 * This file is part of the mavros package and subject to the license terms
 * in the top-level LICENSE file of the mavros repository.
 * https://github.com/mavlink/mavros/tree/master/LICENSE.md
 */

#include <mavros/mavros_plugin.h>

#include <mavros_msgs/ActuatorControl.h>

namespace mavros {
namespace std_plugins {
/**
 * @brief ActuatorOutput plugin
 *
 * Receives actuator output from FCU controller.
 */
class ActuatorOutputPlugin : public plugin::PluginBase {
public:
	ActuatorOutputPlugin() : PluginBase(),
		nh("~")
	{ }

	void initialize(UAS &uas_) override
	{
		PluginBase::initialize(uas_);

		target_actuator_output_pub = nh.advertise<mavros_msgs::ActuatorControl>("target_actuator_output", 10);
		// actuator_control_sub = nh.subscribe("actuator_control", 10, &ActuatorControlPlugin::actuator_control_cb, this);
	}

	Subscriptions get_subscriptions() override
	{
		return {
			       make_handler(&ActuatorOutputPlugin::handle_actuator_output_target),
		};
	}

private:
	ros::NodeHandle nh;

	ros::Publisher target_actuator_output_pub;
	// ros::Subscriber actuator_control_sub;

	/* -*- rx handlers -*- */

	void handle_actuator_output_target(const mavlink::mavlink_message_t *msg, mavlink::common::msg::ACTUATOR_OUTPUT_STATUS &actuator_output_target)
	{
        std::cout << "Received ACTUATOR_OUTPUT_STATUS message with time: " << actuator_output_target.time_usec << std::endl;
		auto actuator_control_target_msg = boost::make_shared<mavros_msgs::ActuatorControl>();
		actuator_control_target_msg->header.stamp = m_uas->synchronise_stamp(actuator_output_target.time_usec);

		// actuator_control_target_msg->group_mix = actuator_output_target.group_mlx;
		const auto &arr = actuator_output_target.actuator;
		std::copy(arr.cbegin(), arr.cend(), actuator_control_target_msg->controls.begin());

		target_actuator_output_pub.publish(actuator_control_target_msg);
	}

	/* -*- callbacks -*- */

	// void actuator_control_cb(const mavros_msgs::ActuatorControl::ConstPtr &req) {
	// 	//! about groups, mixing and channels: @p https://pixhawk.org/dev/mixing
	// 	//! message definiton here: @p https://mavlink.io/en/messages/common.html#SET_ACTUATOR_CONTROL_TARGET
	// 	mavlink::common::msg::SET_ACTUATOR_CONTROL_TARGET act{};
	//
	// 	act.time_usec = req->header.stamp.toNSec() / 1000;
	// 	act.group_mlx = req->group_mix;
	// 	act.target_system = m_uas->get_tgt_system();
	// 	act.target_component = m_uas->get_tgt_component();
	// 	std::copy(req->controls.begin(), req->controls.end(), act.controls.begin());	// std::array = boost::array
	//
	// 	UAS_FCU(m_uas)->send_message_ignore_drop(act);
	// }
};
}	// namespace std_plugins
}	// namespace mavros

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(mavros::std_plugins::ActuatorOutputPlugin, mavros::plugin::PluginBase)
