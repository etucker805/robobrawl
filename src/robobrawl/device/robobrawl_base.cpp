

#include "robobrawl_device.hpp"

namespace robobrawl {

  void TMS_BaseDevice::init(TmsContext& context, const robobrawl::Identity& dev_id)
  {
    device_id_ = dev_id;
    init_device_info();


    device_.init(context, device_id_);
    heartbeat_.init(context, device_id_);
    reply_.init(context, device_id_);
    diagnostics_.init(context, device_id_);
  }

  void TMS_BaseDevice::clear()
  {
    device_.clear();
    heartbeat_.clear();
    reply_.clear();
    diagnostics_.clear();
  }

  void TMS_BaseDevice::publish_device_info()
  {
    device_.publish(device_info_);
  }

}
