#include "robobrawl_button.hpp"

namespace robobrawl {
  void Robobrawl_Button::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    Robobrawl_Device::init(context, dev_id);
    //client and message inits 

    bd_writer_.init(context, dev_id);

    sc_listener_.init(context, dev_id);
    bc_listener_.init(context, dev_id);

    timer_.set_handler([this](const CountdownTimer::Id &id) { this->handle_timer_expired(id); } );

  }

  
  void Robobrawl_Button::clear() 
  {
    // client and message clears
    Robobrawl_Device::clear();

    bd_writer_.clear();

    sc_listener_.clear();
    bc_listener_.clear();
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
    
    // continuous messages (10hz)

  }

  void Robobrawl_Button::do_medium()
  {
    heartbeat_.publish();            // write every time
    // medium messages (1 hz?)
  }

  void Robobrawl_Button::do_slow()
  {
    //slow messages (0.1hz?)
    device_.publish(device_info_);
  }

};
