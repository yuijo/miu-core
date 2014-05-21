#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <zmq.hpp>
#include <msgpack.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <miu/config.hpp>
#include <miu/utility.hpp>

namespace po = boost::program_options;
typedef std::vector<std::string> strings_t;

void parse_options(int argc, char** argv, po::variables_map& vm)
{
  po::options_description visible("Allowed options");
  visible.add_options()
    ("bind,A", po::value<strings_t>()->default_value(strings_t(), ""), "bind socket to the address")
    ("connect,a", po::value<strings_t>()->default_value(strings_t(), ""), "connect socket to the address")
    ("interval,i", po::value<int>(), "send message every N seconds")
    ("topic,t", po::value<std::string>()->default_value("", "\"\""), "topic for publish")
    ("io-threads,T", po::value<int>()->default_value(1), "zmq thread pool size")
    ("version,v", "print version and exit")
    ("help,h", "show this message")
    ;

  po::options_description hidden;
  hidden.add_options()
    ("input-file", po::value<strings_t>(), "input file")
    ;

  po::positional_options_description pos;
  pos.add("input-file", -1);

  po::options_description desc = visible;
  desc.add(hidden);

  po::store(po::command_line_parser(argc, argv).options(desc).positional(pos).run(), vm);
  po::notify(vm);

  if (vm.count("version"))
  {
    std::cout << "miu-cat " << MIU_CORE_VERSION << std::endl;
    std::exit(0);
  }

  if (vm.count("help"))
  {
    std::cout << "Usage: miu-cat [option] [<file>...]" << std::endl;
    std::cout << visible << std::endl;
    std::exit(0);
  }
}

void run(zmq::socket_t& socket, const po::variables_map& vm)
{
  const std::string& topic = vm["topic"].as<std::string>();

  int interval = 0;
  if (vm.count("interval"))
    interval = vm["interval"].as<int>();

  strings_t values;
  if (vm.count("input-file"))
  {
    const strings_t& files = vm["input-file"].as<strings_t>();
    for (strings_t::const_iterator it = files.begin(); it != files.end(); ++it)
    {
      std::ifstream ifs(it->c_str());
      std::istreambuf_iterator<char> first(ifs), last;
      values.push_back(std::string(first, last));
    }
  }
  else
  {
    std::istreambuf_iterator<char> first(std::cin), last;
    values.push_back(std::string(first, last));
  }

  while (true)
  {
    for (strings_t::const_iterator it = values.begin(); it != values.end(); ++it)
    {
      msgpack::sbuffer sbuf;
      msgpack::pack(&sbuf, *it);

      socket.send(topic.c_str(), topic.size(), ZMQ_SNDMORE);
      socket.send(sbuf.data(), sbuf.size());

      if (interval > 0)
        boost::this_thread::sleep(boost::posix_time::seconds(interval));
    }

    if (interval <= 0)
      break;
  }
}

int main(int argc, char** argv)
{
  try
  {
    po::variables_map vm;
    parse_options(argc, argv, vm);

    zmq::context_t context(vm["io-threads"].as<int>());
    zmq::socket_t socket(context, ZMQ_PUB);

    miu::zmq::bind(socket, vm["bind"].as<strings_t>());
    miu::zmq::connect(socket, vm["connect"].as<strings_t>());

    run(socket, vm);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

