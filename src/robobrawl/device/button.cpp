#include "button.hpp"

namespace robobrawl {
  void Robobrawl_Button::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    Robobrawl_Device::init(context, dev_id);
    //client and message inits 

  }

  
  void Robobrawl_Button::clear() 
  {
    // client and message clears
    Robobrawl_Device::clear();
  }

  // // -------------------------------------------------------
  // void Robobrawl_Control::handle_ate_reply(const robobrawl::AuthorizationToEnergizeReply& reply)
  // {


  // }

  void Robobrawl_Button::do_startup()
  {
    device_.publish(device_info_);
    heartbeat_.publish();            // write every time
    // startup messages

    
    publish_controller_state();
  }

  void Robobrawl_Button::do_continuous()
  {
    
    // continuous messages (100hz?)

  }

  void Robobrawl_Button::do_medium()
  {
    heartbeat_.publish();            // write every time
    // medium messages (1 hz?)
  }

  void Robobrawl_Button::do_slow()
  {
    //slow messages (0.1hz?)
  }

};
