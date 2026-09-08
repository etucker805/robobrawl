
#include "control.hpp"

namespace robobrawl {

  void Control::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {

    robobrawl::Identity active_dev_id = dev_id;
    
    
    Robobrawl_Control::init(context, dev_id);
    known_devices.add_client(this);
    cd_listener_.add_client(this);
    bd_listener_.add_client(this);
    
    



    // initialize some messages things 
    
    update_timer_.start();
  }

  // -------------------------------------------------------
  void Control::init_device_info()
  {

    // -----------------------------------------------------------
    // Initialize the DeviceInfo with appropriate info

    device_info_.deviceId(device_id_);
    device_info_.system("");
    device_info_.role(robobrawl::DeviceRole::ROLE_CONTROL);


  
  }







  // -------------------------------------------------------
  // invoked ~100Hz
  void
  Control::update_state()
  {


    // update all of out timers, will call handler if needed 
    timer_manager_.update();
    

  } 

  // -------------------------------------------------------
  void Control::handle_button_data(const robobrawl::ButtonData& data){
    // do a check for it being at this system name... 
    if(std::find(claimed_systems_.begin(), claimed_systems_.end(), data.sysName()) != claimed_systems_.end()){
      //out button
      switch(states_[data.sysName()]){
        case robobrawl::States::STATE_READY_WAIT:
          if(data.color() == robobrawl::ButtonColor::BUTTON_BLUE && data.mainPressed()){
            states_[data.sysName()] = robobrawl::States::STATE_BLUE_READY;
            sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]);
          } else if (data.color() == robobrawl::ButtonColor::BUTTON_ORANGE && data.mainPressed()){
            states_[data.sysName()] = robobrawl::States::STATE_ORANGE_READY;
            sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]);
          }
          // otehr things that might happen I guesss.... 
          break;

        case robobrawl::States::STATE_BLUE_READY:
          // check orange
          if (data.color() == robobrawl::ButtonColor::BUTTON_ORANGE && data.mainPressed()){
            states_[data.sysName()] = robobrawl::States::STATE_BOTH_READY;
            sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]);
          }
          break;
        case robobrawl::States::STATE_ORANGE_READY:
          // check bllue, Im not gonna allow tapouts at this states.
          if( data.color() == robobrawl::ButtonColor::BUTTON_ORANGE && data.mainPressed()){
            states_[data.sysName()] = robobrawl::States::STATE_BOTH_READY;
            sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]);
          }  
          break;

        case robobrawl::States::STATE_RUNNING_MATCH:
          // tapouts and unsticks ... I may need a return from unstick.. or we send time with pause state, save that then continue.
            // id have to save it here to, migth as well just make it less on teh pi side and just send whatever resume time on the run match state change, or I could do it from 3-2-1 too...
            // I guess the clock would have to store max time to get the green - yellow - red times to work correctly... or I coukld have a clcok command msg that will update that...
          if(data.tapoutPressed()) {
            if(data.color() == robobrawl::ButtonColor::BUTTON_BLUE){
              states_[data.sysName()] = robobrawl::States::STATE_TAPOUT_BLUE;
              sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()] /* TODO: add teh tie state cvalues here, ill need the timer form before.... :(*/);
            } else if( data.color() == robobrawl::ButtonColor::BUTTON_ORANGE){
              states_[data.sysName()] = robobrawl::States::STATE_TAPOUT_ORANGE;
              sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]);
            }
          }

          if(data.unstickPressed()){
            if(data.color() == robobrawl::ButtonColor::BUTTON_BLUE){
              states_[data.sysName()] = robobrawl::States::STATE_UNSTICK_BLUE;
              sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]);
            } else if( data.color() == robobrawl::ButtonColor::BUTTON_ORANGE){
              states_[data.sysName()] = robobrawl::States::STATE_UNSTICK_ORANGE;
              sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]);
            }
          }

          break;
          
        case robobrawl::States::STATE_MATCH_TIME_EXPIRE:
          // if I want a tapout to work after this... I guess... 

          break;

        default: // nothing
        break;
      }



    }

  }
  


  // -------------------------------------------------------
  void Control::handle_clock_data(const robobrawl::ClockData& data){
    // should just be the done flag...
    // i dont... knwo if I care... 
    do{  
    // if(std::find(claimed_systems_.begin(), claimed_systems_.end(), data.sysName()) != claimed_systems_.end()){
    //   if(data.done() != 1) { break; } // nothing happended?
    //   switch (states_[data.sysName()])
    //   {
    //   case States::STATE_RUNNING_MATCH:
    //     states_[data.sysName()] = States::STATE_MATCH_TIME_EXPIRE;
    //     sc_writer_.send(device_id_, data.sysName(), states_[data.sysName()]); // time is 0 be def... but
    //     break;
      
    //   default:
    //     break;
    //   }

    // }


      std::cout << "CLOCK" << std::endl;
      if(data.time().has_value()){
        std::cout << "Time from clock: " << data.time().value().seconds() << std::endl;
      }




    } while(0);
  }


  //--------------------------------------------------------
  void Control::handle_new_device(const robobrawl::DeviceInfo& dev_info)
  {
    // update things.. or not... 
    //bump state message.. 
    //if I have the system of this device
    publish_device_info();


    if(std::find(claimed_systems_.begin(), claimed_systems_.end(), dev_info.system()) != claimed_systems_.end()){
      send_state_change(dev_info.system(), states_[dev_info.system()], system_match_times_[dev_info.system()] );
    }

    
  }

  void Control::handle_timer_expired(const CountdownTimer::Id& id){
    // check system stuff, transition state, update system.   
    std::cout << "TIMER EXPIRED! on system: " << id <<  std::endl;

    //id is system name.... btw
    robobrawl::SystemId sys = id;
    switch (states_[sys])
    {
    case robobrawl::States::STATE_RUNNING_MATCH:
      // match over
      states_[sys] = robobrawl::States::STATE_MATCH_TIME_EXPIRE;
      send_state_change(sys, states_[sys], 0);
      break;
    case robobrawl::States::STATE_COUNTDOWN_3_2_1:
      std::cout << "Set state to Run Match, from 3-2-1." << std::endl;
      states_[sys] = robobrawl::States::STATE_RUNNING_MATCH;
      timer_manager_.set_starting_time(id, std::chrono::seconds(system_match_times_[sys]));
      timer_manager_.start(id);
      send_state_change(sys, states_[sys], system_match_times_[sys]);

      break;
    default:
      break;
    }





  }

  // ret 0 = no, ret 1 = yes, ret 2 = uh system not a part of me... so idk
  int Control::need_override_authority(SystemId target_system){
    if(std::find(claimed_systems_.begin(), claimed_systems_.end(), target_system) == claimed_systems_.end()){
      return 2;
    }
    States curr_state = states_[target_system];
    if( curr_state == States::STATE_IDLE_OFF || 
        curr_state == States::STATE_IDLE_ON || 
        curr_state == States::STATE_INDIVIDUAL_OVERRIDE)
        return 0;

      return 1;

  }


  void Control::send_override_button(Identity target, ButtonColor new_color, SystemId new_system){
    // yeah do this.
  }

  void Control::send_override_clock(Identity target, Colors display_color, TimeData display_time, SystemId new_system){
    // this too... 
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
