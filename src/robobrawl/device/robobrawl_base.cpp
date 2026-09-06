

#include "robobrawl_device.hpp"

namespace robobrawl {

  void Robobrawl_BaseDevice::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    device_id_ = dev_id;
    init_device_info();

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
