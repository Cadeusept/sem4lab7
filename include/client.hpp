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

void run_client(const std::string &client_name);


class talk_to_server {
 public:
    talk_to_server(const std::string &username) :
                   _sock(service), _started(true), _username(username) {}

    void loop() {
      write("login " + _username + "\n");
      read_answer();
      while(_started) {
        write_request();
        read_answer();
        boost::this_thread::sleep(boost::posix_time::millisec(rand() % 7000));
      }
    }

    std::string username() const {return _username;}

    void write_request() {
      write("ping\n");
    }

    void read_answer() {
      _already_read = 0;
      read(_sock, boost::asio::buffer(_buff),
           boost::bind(&talk_to_server::read_complete, this, _1, _2));
      process_msg();
    }

    void process_msg() {
      std::string msg(_buff, _already_read);
      if (msg.find("login ") == 0) on_login();
      else if (msg.find("ping") == 0) on_ping(msg);
      else if (msg.find("clients ") == 0) on_clients(msg);
      else std::cerr << "invalid message \"" << msg << "\"" << std::endl;
    }

    void on_login() {
      do_ask_clients();
    }

    void on_ping(const std::string &msg) {
      std::istringstream in(msg);
      std::string answer;
      in >> answer >> answer;
      if (answer == "clients_list_changed")
          do_ask_clients();
    }

    void on_clients(const std::string &msg) {
      std::string clients_list = msg.substr(8);
      std::cout << _username << ", new clients list:" << clients_list;
    }

    void do_ask_clients() {
      write("ask_clients\n");
      read_answer();
    }

    void write(const std::string &msg) {
      _sock.write_some(boost::asio::buffer(msg));
    }

    size_t read_complete(char * buf,
                         const boost::system::error_code &err, size_t bytes) {
      if (err) return 0;
      bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
      return found ? 0 : 1;
    }

    void connect(boost::asio::ip::tcp::endpoint endpoint) {
      _sock.connect(endpoint);
    }

   private:
    boost::asio::ip::tcp::socket _sock;
    enum {max_msg = 1024};
    int _already_read;
    char _buff[max_msg];
    bool _started;
    std::string _username;
};

#endif  // LAB_07_SERVER_CLIENT_H
