
#include "robobrawl_device.hpp"

namespace robobrawl {

  void Robobrawl_Device::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    Robobrawl_BaseDevice::init(context, dev_id);
  }

  void Robobrawl_Device::clear()
  {
    Robobrawl_BaseDevice::clear();
  }

  void Robobrawl_Device::handle_claim_system(const robobrawl::ClaimSystem& claim)
  {
    
  }

  void Robobrawl_Device::set_active_controller(robobrawl::Identity active_controller)
  {
    this->controller_id_ = std::move(active_controller);
  }

  void Robobrawl_Device::publish_controller_state()
  {

  }
}
