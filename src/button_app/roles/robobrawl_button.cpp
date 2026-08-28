
#include "robobrawl_button.hpp"

namespace robobrawl {

  void Button::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {

    robobrawl::Identity active_dev_id = dev_id;
    
    
    Robobrawl_Button::init(context, dev_id);


    // initialize some messages things 
    
    
    update_timer_.start();
  }

  // -------------------------------------------------------
  void Button::init_device_info()
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
  Button::update_state()
  {
    // as in like for the port sttaus, might just not be used, cause listeners.... or this is gui in a wierd way

  } 

  // ------------------------------------------------------- ( not a thing for control prob )
  // bool
  // Control::authorization_required( const tms::EnergyStartStopRequest& ess_request )
  // {
  // }


  // -------------------------------------------------------
  void Button::do_continuous()
  {
    Robobrawl_Button::do_continuous();
    
    if ( transition_timer_.is_running() ) {
      if ( transition_timer_.elapsed_msec() > 10.0f * 1000 ) {
        // yeah like I could use this but ehhhh
      }
    }
  }
  
};
