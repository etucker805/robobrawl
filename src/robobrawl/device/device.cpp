
#include "device.hpp"

namespace robobrawl {

  void Robobrawl_Device::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    Robobrawl_BaseDevice::init(context, dev_id);
    ac_state_.init(context, device_id_);
  }

  void Robobrawl_Device::clear()
  {
    ac_state_.clear();
    Robobrawl_BaseDevice::clear();
  }

  void Robobrawl_Device::set_active_controller(robobrawl::Identity active_controller)
  {
    this->controller_id_ = std::move(active_controller);
  }

  void Robobrawl_Device::publish_controller_state()
  {
    ac_state_.publish(this->controller_id_);
  }
}
