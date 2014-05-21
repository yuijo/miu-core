#ifndef MIU_UTILITY_INCLUDED
#define MIU_UTILITY_INCLUDED

#include <iostream>
#include <zmq.hpp>

namespace miu {

namespace zmq {
  template <typename AddressList>
  void bind(::zmq::socket_t& socket, const AddressList& address_list)
  {
    typedef typename AddressList::const_iterator iter_t;
    for (iter_t it = address_list.begin(); it != address_list.end(); ++it)
      socket.bind(it->c_str());
  }

  template <typename AddressList>
  void connect(::zmq::socket_t& socket, const AddressList& address_list)
  {
    typedef typename AddressList::const_iterator iter_t;
    for (iter_t it = address_list.begin(); it != address_list.end(); ++it)
      socket.connect(it->c_str());
  }
} // zmq

}

#endif
