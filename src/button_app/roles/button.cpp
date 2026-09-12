
#include "button.hpp"

namespace robobrawl {

  void Button::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {

    robobrawl::Identity active_dev_id = dev_id;
    
    
    Robobrawl_Button::init(context, dev_id);


    // initialize some messages things 

    cs_reader_.add_client(this);
    sc_listener_.add_client(this);
    bc_listener_.add_client(this);
    
  }

  // -------------------------------------------------------
  void Button::init_device_info()
  {

    // -----------------------------------------------------------
    // Initialize the DeviceInfo with appropriate info

    device_info_.deviceId(this->device_id_);
    device_info_.role(robobrawl::DeviceRole::ROLE_BUTTON);
    device_info_.system("Test System");
    // I think thats all I got... 

  
  }

  // -------------------------------------------------------
  // invoked ~100Hz
  void
  Button::update_state()
  {
    // as in like for the port sttaus, might just not be used, cause listeners.... or this is gui in a wierd way
    // shou]ld I have timers? for the gui probbably.. Il do that now.
    timer_.update();

  } 

  void Button::handle_claim_system(const robobrawl::ClaimSystem& claim){
    std::cout << "you cant buy me! Im not property! (claim system request)" << std::endl;
  }

  void Button::handle_state_change(const robobrawl::StateChange& req){
      std::cout << "state chnge.... I do something? I think" << std::endl;

      if(req.target() != device_info_.system()) return; // not for us

      switch (req.newState())
      {
      case States::STATE_COUNTDOWN_3_2_1:
        timer_.set_starting_time(std::chrono::seconds(3));
        current_state = States::STATE_COUNTDOWN_3_2_1;
        timer_.start();
        // drawing handled in the interface loop 
        break;

      case States::STATE_RUNNING_MATCH:
        current_state = States::STATE_RUNNING_MATCH;
        if(timer_.is_paused()){
          timer_.resume();
        }
        else if(!req.timerTime().has_value()){
          // problems
          std::wcout << "Tried To Start Match Without StateChange.TImerTime, panic" << std::endl;
          return;
        } else {
          timer_.set_expiration_time_ns(req.timerTime().value().deadline_ns_since_epoc());
          timer_.start();
        }

        break;


        // ADD: pause, halt, expire, tapouts/wins
      case States::STATE_PAUSED_MATCH:
        timer_.pause();
        current_state = States::STATE_PAUSED_MATCH;
        break;
      
      case States::STATE_MATCH_EMERGENCY_STOP:
        timer_.pause();
        current_state = States::STATE_MATCH_EMERGENCY_STOP;
        break;
      
      case States::STATE_TAPOUT_BLUE:
      case States::STATE_TAPOUT_ORANGE:
      case States::STATE_WIN_BLUE:
      case States::STATE_WIN_ORANGE:
        timer_.pause();
        current_state = req.newState();
        
        break;

      default:
        current_state = req.newState();
        break;
      }


  }

  void Button::handle_button_command(const robobrawl::ButtonCommand& comm){
    std::cout << "someone tried to tell me wha to do but im not fond of that" << std::endl;
    std::cout << "for: " << comm.target() << " new color: " << (int)comm.newColor().value() << " new sys: " << comm.newSystem().value() << std::endl;

  }

  void Button::handle_timer_expired(const CountdownTimer::Id& id){
    // yay
    std::cout << "Time expored on a timer! ayyya" << std::endl;
  }
 
  // -------------------------------------------------------
  void Button::do_continuous()
  {
    Robobrawl_Button::do_continuous();
    
  
  }
  
};
