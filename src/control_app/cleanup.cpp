

#include "app.hpp"

void
robobrawl::cleanup()
{
  gui_cleanup();

  
  active_mc.clear();
  known_devices.clear();

  for (auto device: robobrawl_devices)
    device->clear();

  robobrawl_context.clear();


}
