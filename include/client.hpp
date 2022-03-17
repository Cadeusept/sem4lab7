// Created by cadeusept on 07.03.2022.

#ifndef LAB_07_SERVER_CLIENT_H
#define LAB_07_SERVER_CLIENT_H

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
#include <boost/bind.hpp>

extern boost::asio::io_service service;

void run_client();


class talk_to_server {
  private:
      boost::asio::ip::tcp::socket _server_socket;
      enum {_max_msg = 1024};
      int _already_read;
      char _buffer[_max_msg];
      bool _started;
      std::string _username;

  public:
      talk_to_server(const std::string &username) :
                   _server_socket(service), _started(true), _username(username) {}

      void loop();

      std::string username() const;

      void ping_server();

      void read_answer();

      void process_answer();

      void on_login();

      void on_ping(const std::string &msg);

      void on_clients();

      void outpt_answer(const std::string &msg);

      void write(const std::string &msg);

      void wait_for_input();

      size_t read_complete(char * buf,
                         const boost::system::error_code &err, size_t bytes) {
          if (err) return 0;
          bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
          return found ? 0 : 1;
      }

      void connect(const boost::asio::ip::tcp::endpoint endpoint);
};

#endif  // LAB_07_SERVER_CLIENT_H
