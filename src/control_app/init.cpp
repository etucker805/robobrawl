

#include "app.hpp"

#include <cstdio>
#include <csignal>
#include <cstdlib>


// ---------------------------------------------------------------

static std::string qos_file = "file:///mil-std-3071_qos_library.xml";
static std::string baseDeviceId;
// static std::string serialNum;
static uint32_t    domain_id = 0;

static robobrawl::Control control;

// ---------------------------------------------------------------
void handle_sig(int sig)
{
  if (sig == SIGINT)
    robobrawl::done = true;
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
      
          
    case 'd':
      domain_id = static_cast<uint32_t>(atoi(toc_optarg));
      break;

    case 'q':
      qos_file = toc_optarg;
      break;

    case 's':
      // serialNum = toc_optarg;
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
void robobrawl::init(int argc, char* argv[])
{

  try {

    //setup  state to string map
    state_to_string_map[States::STATE_IDLE_ON] ="STATE_IDLE_ON";
    state_to_string_map[States::STATE_IDLE_OFF] ="STATE_IDLE_OFF"; 
    state_to_string_map[States::STATE_BLUE_READY] ="STATE_BLUE_READY"; 
    state_to_string_map[States::STATE_BLUE_VICTOR] ="STATE_BLUE_VICTOR"; 
    state_to_string_map[States::STATE_BOTH_READY] ="STATE_BOTH_READY"; 
    state_to_string_map[States::STATE_COUNTDOWN_3_2_1] ="STATE_COUNTDOWN_3_2_1";
    state_to_string_map[States::STATE_INDIVIDUAL_OVERRIDE] ="STATE_INDIVIDUAL_OVERRIDE"; 
    state_to_string_map[States::STATE_MATCH_EMERGENCY_STOP] ="STATE_MATCH_EMERGENCY_STOP"; 
    state_to_string_map[States::STATE_MATCH_TIME_EXPIRE] ="STATE_MATCH_TIME_EXPIRE"; 
    state_to_string_map[States::STATE_ORANGE_READY] ="STATE_ORANGE_READY";
    state_to_string_map[States::STATE_ORANGE_VICTOR] ="STATE_ORANGE_VICTOR";
    state_to_string_map[States::STATE_PAUSED_MATCH] ="STATE_PAUSED_MATCH"; 
    state_to_string_map[States::STATE_RUNNING_MATCH] ="STATE_RUNNING_MATCH"; 
    state_to_string_map[States::STATE_TAPOUT_BLUE] ="STATE_TAPOUT_BLUE"; 
    state_to_string_map[States::STATE_TAPOUT_ORANGE] ="STATE_TAPOUT_ORANGE"; 
    state_to_string_map[States::STATE_UNSTICK_BLUE] ="STATE_UNSTICK_BLUE"; 
    state_to_string_map[States::STATE_UNSTICK_ORANGE] ="STATE_UNSTICK_ORANGE"; 
    state_to_string_map[States::STATE_WIN_BLUE] ="STATE_WIN_BLUE"; 
    state_to_string_map[States::STATE_WIN_ORANGE] ="STATE_WIN_ORANGE"; 
    // end that thing... hahaaaaaaaaa



    install_sig_handlers();

    if (parse_args(argc, argv) < 0) {
      exit(-1);
    }



    /* check that license is available */
    if (DDS_DomainParticipantFactory_get_instance() == nullptr) {
      fprintf(stderr, "Failed to initialize CoreDX DDS: missing license?\n");
      exit(1);
    }

    baseDeviceId = "bobobrawl";     // todo: select a good value here
    // if (serialNum.empty())
    std::string serialNum = "SN-0001";       // todo: provide a good fallback, if not provided on command-line

    robobrawl_context.init(domain_id, qos_file, baseDeviceId, serialNum);



    active_mc.init(robobrawl_context);
    known_devices.init(robobrawl_context);

    control.init(robobrawl_context, baseDeviceId + "-CONT-1");



    robobrawl_devices.push_back(&control);
  
    // init all DDS entities:
    robobrawl_context.subscriber().enable();
    robobrawl_context.publisher().enable();



    if (gui_setup() != 0) {
      std::cerr << "GUI initialization failed; stopping the application.\n";
      done = true;
    }

  }
  catch (const std::exception& e) {
    std::cerr << "err: " << e.what() << std::endl;
  }
}

