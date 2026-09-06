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

    bd_listener_.init(context, dev_id);
    cd_listener_.init(context, dev_id);
  }

  
  void Robobrawl_Control::clear() 
  {
    // client and message clears
    Robobrawl_BaseDevice::clear();

    sc_writer_.clear();
    cs_writer_.clear();

    bc_writer_.clear();
    cc_writer_.clear();

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

};
