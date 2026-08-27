

#include "robobrawl_device.hpp"

namespace robobrawl {

  void Robobrawl_BaseDevice::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    device_id_ = dev_id;
    init_device_info();


    device_.init(context, device_id_);
    heartbeat_.init(context, device_id_);
    reply_.init(context, device_id_);
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
