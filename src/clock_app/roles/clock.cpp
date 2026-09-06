
#include "clock.hpp"

namespace robobrawl {

  void Clock::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {

    robobrawl::Identity active_dev_id = dev_id;
    
    


    
    Robobrawl_Clock::init(context, dev_id);


    // initialize some messages things 
    
    
    update_timer_.start();
  }

  // -------------------------------------------------------
  void Clock::init_device_info()
  {

    // -----------------------------------------------------------
    // Initialize the DeviceInfo with appropriate info

    device_info_.deviceId(device_id_);
    device_info_.system("Test System");
    device_info_.role(robobrawl::DeviceRole::ROLE_CLOCK);

  
  }

  // -------------------------------------------------------
  // invoked ~100Hz
  void
  Clock::update_state()
  {
    // as in like for the port sttaus, might just not be used, cause listeners.... or this is gui in a wierd way

  } 

  void Clock::handle_new_device(const robobrawl::DeviceInfo& info){
    publish_device_info();
  }



  void Clock::handle_clock_command(const robobrawl::ClockCommand& comm){

  }


  void Clock::handle_claim_system(const robobrawl::ClaimSystem& claim){

  }


  void Clock::handle_state_change(const robobrawl::StateChange& state){

  }



  // ------------------------------------------------------- ( not a thing for control prob )
  // bool
  // Control::authorization_required( const tms::EnergyStartStopRequest& ess_request )
  // {
  // }


  // -------------------------------------------------------
  void Clock::do_continuous()
  {
    Robobrawl_Clock::do_continuous();
    
    if ( transition_timer_.is_running() ) {
      if ( transition_timer_.elapsed_msec() > 10.0f * 1000 ) {
        // yeah like I could use this but ehhhh
      }
    }
  }
  

  void Clock::do_medium()
  {
    Robobrawl_Clock::do_medium();   
  }
};


