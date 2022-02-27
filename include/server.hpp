// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_EXAMPLE_HPP_
#define INCLUDE_EXAMPLE_HPP_

#include <boost/log/utility/setup.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

extern boost::asio::io_service service;

class talk_to_client {
 public:
  boost::asio::ip::tcp::socket _sock{service};
  boost::posix_time::ptime now;
  bool _status = true;
  bool _initial = true;
  uint64_t _clients_num = 0;
  std::string _name, _request;

  void read_request() {
    boost::asio::streambuf buffer{};
    read_until(_sock, buffer, "\n");
    std::string request((std::istreambuf_iterator<char>(&buffer)),
                        std::istreambuf_iterator<char>());
    _request = request;
  }

  void write_reply(std::string reply) {
    reply += "\r\n";
    _sock.write_some(boost::asio::buffer(reply));
  }

  void ping() {
    write_reply("ping_ok\n");
  }

  void communicate() {
    try {

    }
    catch(...) {

    }
  }
};

//extern boost::recursive_mutex mutex;

void accept_thread();

void handle_clients_thread();

#endif // INCLUDE_EXAMPLE_HPP_
