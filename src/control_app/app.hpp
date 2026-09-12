

#pragma once



#include "robobrawl.hh"
#include "components/timer.hpp"
#include "components/robobrawl_context.hpp"
#include "components/active_mc.hpp"
#include "device/known_devices.hpp"

#include "roles/control.hpp"

#include "gui/gui.hpp"

namespace robobrawl {

  void init(int argc, char* argv[]);

  void startup();

  void main_loop();

  void cleanup();

  extern bool done;

  extern RobobrawlContext   robobrawl_context;
  extern ActiveMC           active_mc;
  extern KnownDevices       known_devices;



  // components:
  extern std::vector<Robobrawl_Control*> robobrawl_devices; // loss fo gernerality but idc 

};
