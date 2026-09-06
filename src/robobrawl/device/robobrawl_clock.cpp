#include "robobrawl_clock.hpp"

namespace robobrawl {
  void Robobrawl_Clock::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    Robobrawl_Device::init(context, dev_id);
    //client and message inits 

    cd_writer_.init(context, dev_id);

    cc_listener_.init(context, dev_id);
    sc_listener_.init(context, dev_id);

  }

  
  void Robobrawl_Clock::clear() 
  {
    // client and message clears
    Robobrawl_Device::clear();

    cd_writer_.clear();
    cc_listener_.clear();
    sc_listener_.clear();
  }

  // // -------------------------------------------------------
  // void Robobrawl_Control::handle_ate_reply(const robobrawl::AuthorizationToEnergizeReply& reply)
  // {


  // }

  void Robobrawl_Clock::do_startup()
  {

    publish_device_info();
    heartbeat_.publish();            // write every time
    // startup messages

    
    publish_controller_state();
  }

  void Robobrawl_Clock::do_continuous()
  {
    
    // continuous messages (100hz?)

  }

  void Robobrawl_Clock::do_medium()
  {
    heartbeat_.publish();            // write every time
    // medium messages (1 hz?)
  }

  void Robobrawl_Clock::do_slow()
  {
    //slow messages (0.1hz?)
    publish_device_info();

  }

};
