

#include "app.hpp"


// ---------------------------------------------------------------
// send messages that need to be published at startup:
// ---------------------------------------------------------------

void
robobrawl::startup()
{
  for (auto device: robobrawl_devices)
    device->do_startup();
}
