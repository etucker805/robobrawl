#include "robobrawl_storage.hpp"

namespace robobrawl {
  void TMS_Storage::init(TmsContext& context, const robobrawl::Identity& dev_id)
  {
    TMS_Device::init(context, dev_id);
    port_state_.init(context, dev_id);

  }

  void TMS_Storage::init(TmsContext& context, const robobrawl::Identity& dev_id, bool ac_active)
  {
    ac_active_ = ac_active;
    TMS_Device::init(context, dev_id);
    port_state_.init(context, dev_id);
    
  }
  
  void TMS_Storage::clear() 
  {
    ess_.clear();
    
    acls_state_.clear();
    TMS_Device::clear();
  }

  // -------------------------------------------------------
  void TMS_Storage::handle_ate_reply(const robobrawl::AuthorizationToEnergizeReply& reply)
  {


  }

  void TMS_Storage::do_startup()
  {
    device_.publish(device_info_);
    heartbeat_.publish();            // write every time
    diagnostics_.publish();          // will write() only if changes detected
    port_state_.publish();           // will write() only if changes detected

    
    publish_controller_state();
  }

  void TMS_Storage::do_continuous()
  {
    diagnostics_.publish();          // will write() only if changes detected
    port_state_.publish();           // will write() only if changes detected
  }

  void TMS_Storage::do_medium()
  {
    heartbeat_.publish();            // write every time
    ess_state_.publish();            // will write() only if changed (or change pending)

  }

  void TMS_Storage::do_slow()
  {
    storage_update_.publish();
  }

};
