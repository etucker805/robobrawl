
#include "lights.hpp"

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
  
  // -------------------------------------------------------
  void Lights::handle_clock_command(const robobrawl::ClockCommand& comm){
    std::cout << "Clock Command Recieved, Not implemeted lol" << std::endl;
  }
  
  // -------------------------------------------------------
  void Lights::handle_claim_system(const robobrawl::ClaimSystem& claim){
    std::cout << "Claim State Recieved, didnt do anything cause im lazy. sowwy" << std::endl;
  }
  
  // -------------------------------------------------------
  void Lights::handle_state_change(const robobrawl::StateChange& state){
    // so a valid concern here is that I may want to do transition anims? if so ill have to call led driver functs from here. so uh function passing! yay!
    // or I could just do a is state_curr = state_prev in the tree bit.
    std::cout << "A state has changed, mabey its Wyoming now? who cares" << std::endl;
  }

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
