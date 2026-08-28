

#include "app.hpp"

// ---------------------------------------
int
main(int argc, char* argv[])
{
  std::clog << "APP::init()..." << std::endl;
  robobrawl::init(argc, argv);

  std::clog << "APP::startup()..." << std::endl;
  robobrawl::startup();

  std::clog << "APP::main_loop()..." << std::endl;
  robobrawl::main_loop();

  std::clog << "APP::cleanup()..." << std::endl;
  robobrawl::cleanup();

  return 0;
}
