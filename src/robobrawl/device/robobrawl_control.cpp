#include "robobrawl_control.hpp"

namespace robobrawl {
  void Robobrawl_Control::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {
    Robobrawl_BaseDevice::init(context, dev_id);
    //client and message inits 

    

    sc_writer_.init(context, dev_id);
    cs_writer_.init(context, dev_id);

    bc_writer_.init(context, dev_id);
    cc_writer_.init(context, dev_id);
    lc_writer_.init(context, dev_id);

    bd_listener_.init(context, dev_id);
    cd_listener_.init(context, dev_id);

    timer_manager_.set_handler([this](const CountdownTimer::Id &id) {
      this->handle_timer_expired(id);
        }
     );
     // I think im a wizard
  
  }

  
  void Robobrawl_Control::clear() 
  {
    // client and message clears
    Robobrawl_BaseDevice::clear();

    sc_writer_.clear();
    cs_writer_.clear();

    bc_writer_.clear();
    cc_writer_.clear();
    lc_writer_.clear();

    bd_listener_.clear();
    cd_listener_.clear();
  }




  void Robobrawl_Control::do_startup()
  {
    publish_device_info();
    // device_.publish(device_info_);
    heartbeat_.publish();            // write every time
    // startup messages

    
    

    
    publish_controller_state();
  }

  void Robobrawl_Control::do_continuous()
  {
    
    // continuous messages (100hz?)

  }

  void Robobrawl_Control::do_medium()
  {
    heartbeat_.publish();            // write every time
    // medium messages (1 hz?)
  }

  void Robobrawl_Control::do_slow()
  {
    //slow messages (0.1hz?)
    publish_device_info();

  }


  void Robobrawl_Control::send_state_change(SystemId target, States state, TimeData time){
    if(time.deadline_ns_since_epoc()!=0){
      
      sc_writer_.send(device_id_, target, state, time);
      return;
    }
    sc_writer_.send(device_id_, target, state);
  }

  // shoudl Add auth capability to this... 
  void Robobrawl_Control::send_claim_system(SystemId target){
    cs_writer_.send(device_id_, target);
  }

  void Robobrawl_Control::send_lights_command(Identity target, SystemId new_system){
    lc_writer_.send(device_id_, target, new_system );
  }

  void Robobrawl_Control::send_button_command(Identity target, SystemId new_system, ButtonColor new_color){
    bc_writer_.send(device_id_, target, new_color, new_system);
  }

  void Robobrawl_Control::send_clock_command(Identity target, SystemId new_system, Colors display_color, TimeData display_time){
    cc_writer_.send(device_id_, target, display_color, display_time, new_system);
  }

};
