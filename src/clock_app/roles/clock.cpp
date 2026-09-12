
#include "clock.hpp"

namespace robobrawl {

  void Clock::init(RobobrawlContext& context, const robobrawl::Identity& dev_id)
  {

    robobrawl::Identity active_dev_id = dev_id;


    
    Robobrawl_Clock::init(context, dev_id);
    sc_listener_.add_client(this);
    cs_reader_.add_client(this);
    cc_listener_.add_client(this);
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
    timer_.update();
  } 

  void Clock::handle_new_device(const robobrawl::DeviceInfo& info){
    publish_device_info();
  }



  void Clock::handle_clock_command(const robobrawl::ClockCommand& comm){
    std::cout << "Recieved clock command! not gonna do anything abut it tho " << std::endl;
  }


  void Clock::handle_claim_system(const robobrawl::ClaimSystem& claim){
    std::cout << "system Claimed by " << claim.source() << ", not actualy doing anythig tho" << std::endl;

  }


  void Clock::handle_state_change(const robobrawl::StateChange& state){

    if(state.target() != device_info_.system())
      return;

    //@TODO not yet checking if this is our claimed one, or for active mc cause thats a thing too IG.

    std::cout << "New State: " << get_state_string(state.newState()) << std::endl; 
    switch (state.newState())
    {
      // these are only the things that have to happen on transition, play apuse resume, that kind of thing, the others will be in interface loop, as statics. 
    case robobrawl::States::STATE_RUNNING_MATCH:
    
      if(!state.timerTime().has_value() || state.timerTime().value().deadline_ns_since_epoc() <= std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() ){
        std::cout << "Attempt to start match with no time or 0 time!" << std::endl;
      } else if(current_state == robobrawl::States::STATE_PAUSED_MATCH){
        // should match times, but Ill check just to be sure.
        if( timer_.is_paused() ){
          timer_.resume(); // not nessisary at all but whatever,.
        } else {
          timer_.set_expiration_time_ns(std::chrono::nanoseconds(state.timerTime().value().deadline_ns_since_epoc()).count());
          timer_.start();
        }
      } else{
        timer_.set_expiration_time_ns(std::chrono::nanoseconds(state.timerTime().value().deadline_ns_since_epoc()).count());
        timer_.start();
        // then we chould be good. I assume
      }
      break;
    
    case robobrawl::States::STATE_COUNTDOWN_3_2_1:
      timer_.set_starting_time(std::chrono::seconds(3)); // yeah idc I think.... 
      timer_.start();
      break;
      
    case robobrawl::States::STATE_PAUSED_MATCH:
      timer_.pause();
      break;

    case robobrawl::States::STATE_MATCH_EMERGENCY_STOP:
      timer_.pause();
      break;

    case robobrawl::States::STATE_MATCH_TIME_EXPIRE: // Im gonna rely on this one for the most part. 
      // nothing to do thogh, ill let the timer run if it is, if not we just go to the expired state. 
      break;

    default:
      break;
    }

    current_state = state.newState();
  }


  void Clock::handle_timer_expired(const CountdownTimer::Id& id){
    // I dont think I will actualy care about this, as I 
    std::cout << " Clock Timer Expored! oh boy" << std::endl;
  }

  // -------------------------------------------------------
  void Clock::do_continuous()
  {
    Robobrawl_Clock::do_continuous();
    
  }
  

  void Clock::do_medium()
  {
    Robobrawl_Clock::do_medium();   
    // std::cout << "Timer: " << std::chrono::duration_cast<std::chrono::seconds>(timer_.remaining_time()).count() << std::endl;
  }
};


