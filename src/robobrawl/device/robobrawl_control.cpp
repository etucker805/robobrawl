#include "robobrawl_control.hpp"
#include "robobrawl.hh"
#include <cstdint>

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
    publish_device_info();
    heartbeat_.publish();            // write every time
    // medium messages (1 hz?)
  }

  void Robobrawl_Control::do_slow()
  {
    //slow messages (0.1hz?)
    publish_device_info();

  }


  void Robobrawl_Control::send_state_change(SystemId target, States state, TimeData time){

    robobrawl::StateChange sc;
    sc.source(device_id_);
    sc.target(target);
    sc.newState(state);

    if(!time.deadline_ns_since_epoc().has_value() && !time.duration_msec().has_value()){
      // have nothing, dont put itme in 
    } else {
      //have somthing
      sc.timerTime(time); 
    }


    sc_writer_.send(sc);
  }

  // shoudl Add auth capability to this... 
  void Robobrawl_Control::send_claim_system(SystemId target){
    robobrawl::ClaimSystem cs;
    cs.target(target);
    cs.source(device_id_);
    cs_writer_.send(cs);
  }

  void Robobrawl_Control::send_lights_config(Identity target, SystemId new_system){
    robobrawl::LightsConfig lc;
    lc.source(device_id_);
    lc.target(target);
    if(!new_system.empty())
      lc.newSystem(new_system);


    lc_writer_.send(lc);
  }

  void Robobrawl_Control::send_button_config(Identity target, SystemId new_system, ButtonColor new_color){
    robobrawl::ButtonConfig bc;
    bc.source(device_id_);
    bc.target(target);
    if(!new_system.empty())
      bc.newSystem(new_system);

    if(new_color != robobrawl::ButtonColor::BUTTON_UNKNOWN)
      bc.newButtonColor(new_color);

    bc_writer_.send(bc);
  }

  void Robobrawl_Control::send_clock_config(Identity target, SystemId new_system, uint16_t new_ledSegPerDigitSeg, uint16_t new_hasArenaLights, const std::optional<LedConfig>& led_conf){
    robobrawl::ClockConfig cc;
    cc.source(device_id_);
    cc.target(target);
    if(!new_system.empty())
      cc.newSystem(new_system);
  
    if(led_conf.has_value())
      cc.ledConfig(led_conf.value());
  
    if(new_ledSegPerDigitSeg > 0)
      cc.ledSegPerDigitSeg(new_ledSegPerDigitSeg);
  
    if(new_hasArenaLights >= 0)
      cc.hasArenaLights(new_hasArenaLights);

    cc_writer_.send(cc);
  }

};
