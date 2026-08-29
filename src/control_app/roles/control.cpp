
#include "control.hpp"

namespace robobrawl {

  void Control::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {

    robobrawl::Identity active_dev_id = dev_id;
    
    
    Robobrawl_Control::init(context, dev_id);


    // initialize some messages things 
    
    known_devices.add_client(this);
    update_timer_.start();
  }

  // -------------------------------------------------------
  void Control::init_device_info()
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
  Control::update_state()
  {
    // as in like for the port sttaus, might just not be used, cause listeners.... or this is gui in a wierd way
    // I think this just isnt used ( and can be deleted but I have attachment issues)
  } 

  // -------------------------------------------------------
  void Control::handle_button_data(const robobrawl::ButtonData& data){
    // do a check for it being at this system name... 
  }
  
  //--------------------------------------------------------
  void Control::handle_new_device(const robobrawl::DeviceInfo& dev_info)
  {
    // update things.. or not... 
    // I may try to virtual this up to the specific control mpl. 
    printf("new device callback in control template, override in app pls \n ");
  }



  // -------------------------------------------------------
  void Control::do_continuous()
  {
    Robobrawl_Control::do_continuous();
    
    if ( transition_timer_.is_running() ) {
      if ( transition_timer_.elapsed_msec() > 10.0f * 1000 ) {
        // yeah like I could use this but ehhhh
      }
    }
  }
  
  // -------------------------------------------------------
  void Control::do_medium()
  {
    Robobrawl_Control::do_medium();
    
    
  }

};
