

#include "app.hpp"

#include <cstdio>
#include <csignal>
#include <cstdlib>
#include <algorithm>


// ---------------------------------------------------------------

static std::string qos_file = "file:///mil-std-3071_qos_library.xml";
static std::string baseDeviceId;
static std::string system_name_base;
std::string robobrawl::host_corner_color;
static uint32_t    domain_id = 0;

static robobrawl::Lights lights;

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
  fprintf(stderr, "   -s <system>          : specify the system to default to\n");
  fprintf(stderr, "   -c <host_color>      : specify the color of the correner the compute is on (all CAPS pls)\n");

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
      system_name_base = toc_optarg;
      break;

    case 'c':
      robobrawl::host_corner_color = toc_optarg;
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

    install_sig_handlers();

    if (parse_args(argc, argv) < 0) {
      exit(-1);
    }

    /* check that license is available */
    if (DDS_DomainParticipantFactory_get_instance() == nullptr) {
      fprintf(stderr, "Failed to initialize CoreDX DDS: missing license?\n");
      exit(1);
    }

    baseDeviceId = "ROBOBRAWL";     // todo: select a good value here
    if (system_name_base.empty())
      system_name_base = "Test System";
    
    if(robobrawl::host_corner_color.empty())
      robobrawl::host_corner_color = "BLUE";

      // to a to upper case 
    std::transform(robobrawl::host_corner_color.begin(), robobrawl::host_corner_color.end(), robobrawl::host_corner_color.begin(), [](unsigned char c){
      return std::toupper(c);
    });

    robobrawl_context.init(domain_id, qos_file, baseDeviceId);

    active_mc.init(robobrawl_context);
    known_devices.init(robobrawl_context);

    lights.init(robobrawl_context, baseDeviceId + "-LIGHT-1");

    robobrawl_devices.push_back(&lights);
  
    // init all DDS entities:
    robobrawl_context.subscriber().enable();
    robobrawl_context.publisher().enable();

    

  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

