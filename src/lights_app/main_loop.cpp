
#include "app.hpp"

#include <chrono>
#include <thread>

// ---------------------------------------------------------------
bool                        robobrawl::done = false;
robobrawl::RobobrawlContext robobrawl::robobrawl_context;
robobrawl::KnownDevices     robobrawl::known_devices;
robobrawl::ActiveMC         robobrawl::active_mc(robobrawl::known_devices);

std::vector<robobrawl::Lights*>   robobrawl::robobrawl_devices;

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
    // stips are now doning a thing 
    
    std::thread strip_one  (render_strip, 0);
    std::thread strip_three(render_strip, 2);
    std::thread strip_two  (render_strip, 1);

    // this is chill for now 
    // noo its not cause I have to wait for the thing to join before I delay....
    // but honestly for the max of 50 whatever length It should be fineeeeee cause thats like 2 ms 
    // if I tried to do this later I would have to mux up my light lists, witch might by problematic, so we just launch the writes im paralell
    strip_one.join();
    strip_three.join();
    strip_two.join();


    // ------------------------------------------
    // publish to TMS infrastructure:

    // every 0.1 sec:
    if (timer_01hz.elapsed().count() >= 100) {
      refresh_lights(robobrawl_devices[0]);
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
