/*****************************************************************
 *   file:   main_loop.cpp
 *   desc:   storage application main loop
 *
 *****************************************************************
 *
 *   Copyright(C) 2022-2026 Twin Oaks Computing, Inc
 *   All rights reserved.   Castle Rock, CO 80108
 *
 *****************************************************************
 *  This software has been provided pursuant to a License Agreement
 *  containing restrictions on its use.  This software contains
 *  valuable trade secrets and proprietary information of
 *  Twin Oaks Computing, Inc and is protected by law.  It may not be
 *  copied or distributed in any form or medium, disclosed to third
 *  parties, reverse engineered or used in any manner not provided
 *  for in said License Agreement except with the prior written
 *  authorization from Twin Oaks Computing, Inc.
 *****************************************************************/

#include "app.hpp"

#include <chrono>
#include <thread>

// ---------------------------------------------------------------
bool                  tms::done = false;
tms::TmsContext       tms::tms_context;
tms::KnownDevices     tms::known_devices;
tms::ActiveMC         tms::active_mc(tms::known_devices);

std::vector<tms::TMS_BaseDevice*>   tms::tms_devices;

static Timer timer_01hz; //  0.1 Hz
static Timer timer_1hz;  //  1   Hz
static Timer timer_10hz; // 10   Hz

// ---------------------------------------------------------------
void
tms::main_loop()
{
  timer_01hz.start();
  timer_1hz.start();
  timer_10hz.start();

  while (!done) {
    // read status from hardware, and update apropriate values:
    for (auto device: tms_devices)
      device->update_state();

    // ------------------------------------------
    // publish to TMS infrastructure:

    // every 0.1 sec:
    if (timer_01hz.elapsed().count() >= 100) {
      timer_01hz.start();
      for (auto device: tms_devices)
        device->do_continuous();
    }

    // every 1 sec:
    if (timer_1hz.elapsed().count() >= 1000) {
      timer_1hz.start();

      for (auto device: tms_devices)
        device->do_medium();

      // check if the 'active controller' should change
      if (tms::active_mc.check()) {
        // active controller changed
        // -- publish a Active Controller State (for each local device we have)
        for (auto device: tms_devices) {
          device->set_active_controller(tms::active_mc.get_active_controller());
          device->publish_controller_state();
        }
      }

      std::clog << "Tick..." << std::endl;
    }

    // every 10 sec:
    if (timer_10hz.elapsed().count() >= 10000) {
      timer_10hz.start();
      
      for (auto device: tms_devices)
        device->do_slow();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
