
#include "robobrawl_device.hpp"

namespace robobrawl {

  void TMS_Device::init(TmsContext& context, const robobrawl::Identity& dev_id)
  {
    TMS_BaseDevice::init(context, dev_id);
    ac_state_.init(context, device_id_);
  }

  void TMS_Device::clear()
  {
    ac_state_.clear();
    TMS_BaseDevice::clear();
  }

  void TMS_Device::set_active_controller(robobrawl::Identity active_controller)
  {
    this->controller_id_ = std::move(active_controller);
  }

  void TMS_Device::publish_controller_state()
  {
    ac_state_.publish(this->controller_id_);
  }
}
