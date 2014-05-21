#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <zmq.hpp>
#include <boost/program_options.hpp>
#include <miu/config.hpp>
#include <miu/utility.hpp>

namespace po = boost::program_options;
typedef std::vector<std::string> strings_t;

void parse_options(int argc, char** argv, po::variables_map& vm)
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("bind-frontend,F",    po::value<strings_t>()->default_value(strings_t(), ""), "bind frontend socket to the address")
    ("connect-frontend,f", po::value<strings_t>()->default_value(strings_t(), ""), "connect frontend socket to the address")
    ("bind-backend,B",     po::value<strings_t>()->default_value(strings_t(), ""), "bind backend socket to the address")
    ("connect-backend,b",  po::value<strings_t>()->default_value(strings_t(), ""), "connect backend socket to the address")
    ("io-threads,t", po::value<int>()->default_value(1), "zmq thread pool size")
    ("version,v", "print version and exit")
    ("help,h", "show this message")
    ;

  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);

  if (vm.count("version"))
  {
    std::cout << "miu-hub " << MIU_CORE_VERSION << std::endl;
    std::exit(0);
  }

  if (vm.count("help"))
  {
    std::cout << "Usage: miu-hub [option]" << std::endl;
    std::cout << desc << std::endl;
    std::exit(0);
  }
}

int main(int argc, char** argv)
{
  try
  {
    po::variables_map vm;
    parse_options(argc, argv, vm);

    zmq::context_t context(vm["io-threads"].as<int>());
    zmq::socket_t frontend(context, ZMQ_XSUB);
    zmq::socket_t backend(context, ZMQ_XPUB);

    miu::zmq::bind(frontend, vm["bind-frontend"].as<strings_t>());
    miu::zmq::connect(frontend, vm["connect-frontend"].as<strings_t>());
    miu::zmq::bind(backend, vm["bind-backend"].as<strings_t>());
    miu::zmq::connect(backend, vm["connect-backend"].as<strings_t>());

    zmq::proxy(frontend, backend, 0);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

