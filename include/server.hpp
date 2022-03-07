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

class talk_to_client;

typedef boost::shared_ptr<talk_to_client> client_ptr;
extern boost::asio::io_service service;
extern boost::recursive_mutex cli_mutex;
extern std::vector<client_ptr> clients;

class talk_to_client {
 private:
  boost::asio::ip::tcp::socket _sock{service};
  //boost::posix_time::ptime _now;
  enum {max_msg = 1024};
  int _already_read;
  char _buff[max_msg];
  bool _started;
  bool _status = true;
  bool _clients_changed;
  bool _initial = true;
  uint64_t _clients_num = 0;
  std::string _username, _request;
  boost::posix_time::ptime _last_ping;

 public:
  bool set_clients_changed() {_clients_changed = true;}

  void update_clients_changed() {_clients_changed = !_clients_changed;}

  boost::asio::ip::tcp::socket &sock() {return _sock;}

  std::string username() {return _username;}

  bool timed_out() const {
    boost::posix_time::ptime _now =
        boost::posix_time::microsec_clock::local_time();
    long long ms = (_now - _last_ping).total_milliseconds();
    return ms > 5000 ;
  }

  void stop() {
    boost::system::error_code err; _sock.close(err);
  }

  /*void read_request() {
    boost::asio::streambuf buffer{};
    read_until(_sock, buffer, "\n");
    std::string request((std::istreambuf_iterator<char>(&buffer)),
                        std::istreambuf_iterator<char>());
    _request = request;
  }*/

  void write_reply(std::string reply) {
    reply += "\r\n";
    _sock.write_some(boost::asio::buffer(reply));
  }

  void answer_to_client() {
    try {
      read_request();
      process_request();
    }
    catch(boost::system::system_error&) {
      stop();
    }
  }

  void read_request() {
    if (_sock.available())
      _already_read += _sock.read_some(
          boost::asio::buffer(_buff + _already_read,
                              max_msg - _already_read));
  }

  void process_request() {
    bool found_enter = std::find(_buff, _buff + _already_read, '\n');
    if (!found_enter)
      return; //сообщение не полное

    _last_ping = boost::posix_time::microsec_clock::local_time();
    size_t pos = std::find(_buff, _buff + _already_read, '\n') - _buff;
    std::string msg(_buff, pos);
    std::copy(_buff + _already_read, _buff + max_msg, _buff);
    _already_read -= pos + 1;
    if (msg.find("login ") == 0) on_login(msg);
    else if (msg.find("ping") == 0) on_ping();
    else if (msg.find("ask_clients") == 0) on_clients();
    else std::cerr << "invalid message \"" << msg << "\"" << std::endl;
  }

  void on_login(const std::string &msg) {
    std::istringstream in(msg);
    in >> _username >> _username;
    write("login ok\n");
    update_clients_changed();
  }

  void on_ping() {
    write(_clients_changed ? "ping client_list_changed\n" : "ping ok\n");
    _clients_changed = false;
  }

  void on_clients() {
    std::string msg;
    {
      boost::recursive_mutex::scoped_lock lock(cli_mutex);
      for (auto& client : clients) {
          msg += client->username() +" ";
      }
    }
    write("clients " + msg + "\n");
  }

  void write (const std::string &msg) {
    _sock.write_some(boost::asio::buffer(msg));
  }
};

//extern boost::recursive_mutex mutex;

void accept_thread();

void handle_clients_thread();

#endif // INCLUDE_EXAMPLE_HPP_
