#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <zmq.hpp>
#include <msgpack.hpp>
#include <boost/program_options.hpp>
#include <miu/config.hpp>
#include <miu/utility.hpp>

namespace po = boost::program_options;
typedef std::vector<std::string> strings_t;

void parse_options(int argc, char** argv, po::variables_map& vm)
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("bind,A", po::value<strings_t>()->default_value(strings_t(), ""), "bind socket to the address")
    ("connect,a", po::value<strings_t>()->default_value(strings_t(), ""), "connect socket to the address")
    ("topic,t", po::value<std::string>()->default_value("", "\"\""), "topic for subscribes")
    ("io-threads,T", po::value<int>()->default_value(1), "zmq thread pool size")
    ("version,v", "print version and exit")
    ("help,h", "show this message")
    ;

  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  po::notify(vm);

  if (vm.count("version"))
  {
    std::cout << "miu-tail " << MIU_CORE_VERSION << std::endl;
    std::exit(0);
  }

  if (vm.count("help"))
  {
    std::cout << "Usage: miu-tail [option]" << std::endl;
    std::cout << desc << std::endl;
    std::exit(0);
  }
}

void run(zmq::socket_t& socket)
{
  while (true)
  {
    zmq::message_t msg_topic, msg_body;
    socket.recv(&msg_topic);
    socket.recv(&msg_body);

    msgpack::unpacked msg;
    msgpack::unpack(&msg, static_cast<const char*>(msg_body.data()), msg_body.size());

    std::string topic(static_cast<const char*>(msg_topic.data()), msg_topic.size());
    msgpack::object obj = msg.get();

    std::cout << "<" << topic << "> " << obj << std::endl;
  }
}

int main(int argc, char** argv)
{
  try
  {
    po::variables_map vm;
    parse_options(argc, argv, vm);

    zmq::context_t context(vm["io-threads"].as<int>());
    zmq::socket_t socket(context, ZMQ_SUB);

    miu::zmq::bind(socket, vm["bind"].as<strings_t>());
    miu::zmq::connect(socket, vm["connect"].as<strings_t>());

    const std::string& topic = vm["topic"].as<std::string>();
    socket.setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());

    run(socket);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

