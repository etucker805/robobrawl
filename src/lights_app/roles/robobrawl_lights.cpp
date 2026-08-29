
#include "robobrawl_lights.hpp"

namespace robobrawl {

  void Lights::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {

    robobrawl::Identity active_dev_id = dev_id;
    
    
    Robobrawl_Lights::init(context, dev_id);


    // initialize some messages things 
    
    
    update_timer_.start();
  }

  // -------------------------------------------------------
  void Lights::init_device_info()
  {

    // -----------------------------------------------------------
    // Initialize the DeviceInfo with appropriate info

    device_info_.deviceId(this->device_id_);
    device_info_.role(robobrawl::DeviceRole::ROLE_CONTROL);
    // I think thats all I got... 

  
  }

  // -------------------------------------------------------
  // invoked ~100Hz
  void
  Lights::update_state()
  {
    // as in like for the port sttaus, might just not be used, cause listeners.... or this is gui in a wierd way

  } 

  // ------------------------------------------------------- ( not a thing for control prob )
  // bool
  // Control::authorization_required( const tms::EnergyStartStopRequest& ess_request )
  // {
  // }


  // -------------------------------------------------------
  void Lights::do_continuous()
  {
    Robobrawl_Lights::do_continuous();
    
    if ( transition_timer_.is_running() ) {
      if ( transition_timer_.elapsed_msec() > 10.0f * 1000 ) {
        // yeah like I could use this but ehhhh
      }
    }
  }
  
};
