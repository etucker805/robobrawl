
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
  
  //--------------------------------------------------------
  void Control::handle_new_device(const robobrawl::DeviceInfo& dev_info)
  {
    // update things.. or not... 
    //bump state message.. 
    //if I have the system of this device
    if(std::find(claimed_systems_.begin(), claimed_systems_.end(), dev_info.system()) != claimed_systems_.end()){
      sc_writer_.send(device_id_, dev_info.system(), states_[dev_info.system()], state_times_[dev_info.system()] );
    }

    
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
