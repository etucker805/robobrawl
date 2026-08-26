/*****************************************************************
 *   file:   init.cpp
 *   desc:   storage application initialization routines
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

#include <cstdio>
#include <csignal>
#include <cstdlib>


// ---------------------------------------------------------------

static std::string qos_file = "file:///mil-std-3071_qos_library.xml";
static std::string baseDeviceId;
static std::string serialNum;
static bool        ac_active;
static uint32_t    domain_id = 0;

static tms::Storage storage;

// ---------------------------------------------------------------
void handle_sig(int sig)
{
  if (sig == SIGINT)
    tms::done = true;
}

// ---------------------------------------------------------------
static int install_sig_handlers()
{
  struct sigaction int_action{};
  int_action.sa_handler = handle_sig;
  sigemptyset(&int_action.sa_mask);
  sigaddset(&int_action.sa_mask, SIGINT);
  int_action.sa_flags = 0;

  sigaction(SIGINT, &int_action, nullptr);
  return 0;
}

// ---------------------------------------------------------------
static void print_usage(int argc, char* argv[])
{
  fprintf(stderr, "Usage: %s \n", argv[0]);
  fprintf(stderr, "   -h                   : this help messaage\n");
  fprintf(stderr, "   -p  <port>           : specify active load port ( ac / dc )\n");
  fprintf(stderr, "   -d <domain_id>       : specify the DDS DomainId to join [default: %d]\n", domain_id);
  fprintf(stderr, "   -q <url_to_qos_xml>  : specify the path to the QoS XML file [default: %s]\n", qos_file.c_str());
  fprintf(stderr, "   -s <serial number>   : specify the serial number to use throughout\n");
}

// ---------------------------------------------------------------
static int parse_args(int argc, char* argv[])
{
  int opt;
  while ((opt = toc_getopt(argc, argv, "q:hd:s:p:-")) != -1) {
    switch (opt) {
      
    default: /* '?' */
      print_usage(argc, argv);
      printf("Invalid option : 0x%02x\n", opt);
      return -1;
      break;
      
    case 'p':
      ac_active = strcasecmp(toc_optarg, "ac") == 0;  
      break;
          
    case 'd':
      domain_id = static_cast<uint32_t>(atoi(toc_optarg));
      break;

    case 'q':
      qos_file = toc_optarg;
      break;

    case 's':
      serialNum = toc_optarg;
      break;

    case 'h':
      print_usage(argc, argv);
      return -1;
      break;

    }
  }
  return 0;
}

// ---------------------------------------------------------------
void tms::init(int argc, char* argv[])
{

  try {

    install_sig_handlers();

    if (parse_args(argc, argv) < 0) {
      exit(-1);
    }

    /* check that license is available */
    if (DDS_DomainParticipantFactory_get_instance() == nullptr) {
      fprintf(stderr, "Failed to initialize CoreDX DDS: missing license?\n");
      exit(1);
    }

    baseDeviceId = "INTRACOM";     // todo: select a good value here
    if (serialNum.empty())
      serialNum = "SN-0001";       // todo: provide a good fallback, if not provided on command-line

    tms_context.init(domain_id, qos_file, baseDeviceId, serialNum);

    active_mc.init(tms_context);
    known_devices.init(tms_context);

    storage.init(tms_context, baseDeviceId + "-STOR-1" + "-" + serialNum, ac_active);

    tms_devices.push_back(&storage);
  
    // init all DDS entities:
    tms_context.subscriber().enable();
    tms_context.publisher().enable();

  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

