

#include "robobrawl_device.hpp"

namespace robobrawl {

  void Robobrawl_BaseDevice::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    device_id_ = dev_id;
    init_device_info();

    //setup  state to string map
    state_to_string_map[States::STATE_IDLE_ON] ="STATE_IDLE_ON";
    state_to_string_map[States::STATE_IDLE_OFF] ="STATE_IDLE_OFF"; 
    state_to_string_map[States::STATE_READY_WAIT] ="STATE_READY_WAIT"; 
    state_to_string_map[States::STATE_BLUE_READY] ="STATE_BLUE_READY"; 
    state_to_string_map[States::STATE_BOTH_READY] ="STATE_BOTH_READY"; 
    state_to_string_map[States::STATE_COUNTDOWN_3_2_1] ="STATE_COUNTDOWN_3_2_1";
    state_to_string_map[States::STATE_INDIVIDUAL_OVERRIDE] ="STATE_INDIVIDUAL_OVERRIDE"; 
    state_to_string_map[States::STATE_MATCH_EMERGENCY_STOP] ="STATE_MATCH_EMERGENCY_STOP"; 
    state_to_string_map[States::STATE_MATCH_TIME_EXPIRE] ="STATE_MATCH_TIME_EXPIRE"; 
    state_to_string_map[States::STATE_ORANGE_READY] ="STATE_ORANGE_READY";
    state_to_string_map[States::STATE_PAUSED_MATCH] ="STATE_PAUSED_MATCH"; 
    state_to_string_map[States::STATE_RUNNING_MATCH] ="STATE_RUNNING_MATCH"; 
    state_to_string_map[States::STATE_TAPOUT_BLUE] ="STATE_TAPOUT_BLUE"; 
    state_to_string_map[States::STATE_TAPOUT_ORANGE] ="STATE_TAPOUT_ORANGE"; 
    state_to_string_map[States::STATE_UNSTICK_BLUE] ="STATE_UNSTICK_BLUE"; 
    state_to_string_map[States::STATE_UNSTICK_ORANGE] ="STATE_UNSTICK_ORANGE"; 
    state_to_string_map[States::STATE_WIN_BLUE] ="STATE_WIN_BLUE"; 
    state_to_string_map[States::STATE_WIN_ORANGE] ="STATE_WIN_ORANGE"; 

    try {
      device_.init(context, device_id_);
    }
    catch (const std::exception& error) {
      throw std::runtime_error("DeviceInfo writer initialization failed: " + std::string(error.what()));
    }

    try {
      heartbeat_.init(context, device_id_);
    }
    catch (const std::exception& error) {
      throw std::runtime_error("Heartbeat writer initialization failed: " + std::string(error.what()));
    }

    try {
      reply_.init(context, device_id_);
    }
    catch (const std::exception& error) {
      throw std::runtime_error("Reply writer initialization failed: " + std::string(error.what()));
    }

    
  }

  void Robobrawl_BaseDevice::clear()
  {
    device_.clear();
    heartbeat_.clear();
    reply_.clear();
  }

  void Robobrawl_BaseDevice::publish_device_info()
  {
    device_.publish(device_info_);
  }

}
