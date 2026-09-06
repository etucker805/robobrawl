/*****************************************************************
 *   file:   app.hpp
 *   desc:   storage application declarations
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

#pragma once

#include "robobrawl.hh"
#include "components/timer.hpp"
#include "components/robobrawl_context.hpp"
#include "components/active_mc.hpp"
#include "device/known_devices.hpp"

#include "roles/clock.hpp"

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
  extern std::vector<Robobrawl_BaseDevice*> robobrawl_devices;

};
