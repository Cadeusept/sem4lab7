// Copyright 2021 Your Name <your_email>

#ifndef INCLUDE_EXAMPLE_HPP_
#define INCLUDE_EXAMPLE_HPP_

#include <boost/thread/thread.hpp>
#include <boost/thread/detail/thread.hpp>
#include <boost/thread.hpp>
#include <thread>
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
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <pthread.h>
#include <condition_variable>

extern boost::asio::io_service service;

class talk_to_client {
    private:
        boost::asio::ip::tcp::socket _client_socket{service};
        std::string _username;
        boost::posix_time::ptime _last_ping;
        int _already_read = 0;
        enum {_max_msg = 1024};
        char _buffer[_max_msg];
        bool _clients_changed_flag = false;
    public:

        talk_to_client() {};

        boost::asio::ip::tcp::socket &get_socket();

        std::string username();

        void answer_to_client();

        void read_request();

        void process_request();

        void on_login(std::string msg);

        void clients_changed();

        void on_ping();

        void on_clients();

        void write(std::string msg);

        bool timed_out() const;

        void stop();

};

typedef boost::shared_ptr<talk_to_client> client_ptr;

void accept_thread();

void handle_clients_thread();

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
                   _server_socket(service),
                   _started(true),
                   _username(username) {}

      void loop();

      std::string username() const;

      void ping_server();

      void read_answer();

      void process_answer();

      //void on_login();

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

void run_client();

#endif // INCLUDE_EXAMPLE_HPP_
