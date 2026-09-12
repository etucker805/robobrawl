
#include "app.hpp"

#include <chrono>
#include <thread>

// ---------------------------------------------------------------
bool                        robobrawl::done = false;
robobrawl::RobobrawlContext robobrawl::robobrawl_context;
robobrawl::KnownDevices     robobrawl::known_devices;
robobrawl::ActiveMC         robobrawl::active_mc(robobrawl::known_devices);

std::vector<robobrawl::Robobrawl_BaseDevice*>   robobrawl::robobrawl_devices;

static Timer timer_01hz; //  0.1 Hz
static Timer timer_1hz;  //  1   Hz
static Timer timer_10hz; // 10   Hz

// ---------------------------------------------------------------
void
robobrawl::main_loop()
{
  timer_01hz.start();
  timer_1hz.start();
  timer_10hz.start();

  while (!done) {
    // read status from hardware, and update apropriate values:
    for (auto device: robobrawl_devices)
      device->update_state();

    // ------------------------------------------
    // publish to TMS infrastructure:
    
 
    // every 0.1 sec:
    if (timer_01hz.elapsed().count() >= 100) {
      timer_01hz.start();
      for (auto device: robobrawl_devices)
        device->do_continuous();
    }

    // every 1 sec:
    if (timer_1hz.elapsed().count() >= 1000) {
      timer_1hz.start();

      for (auto device: robobrawl_devices)
        device->do_medium();

      // check if the 'active controller' should change
      // if (robobrawl::active_mc.check()) {
      //   // active controller changed
      //   // -- publish a Active Controller State (for each local device we have)
      //   for (auto device: robobrawl_devices) {
      //     device->set_active_controller(robobrawl::active_mc.get_active_controller());
      //     device->publish_controller_state();
      //   }
      // }

      std::clog << "Tick..." << std::endl;
    }

    // every 10 sec:
    if (timer_10hz.elapsed().count() >= 10000) {
      timer_10hz.start();
      
      for (auto device: robobrawl_devices)
        device->do_slow();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
