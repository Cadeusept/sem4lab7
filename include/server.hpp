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

        void answer_to_client();

        void read_request();

        void process_request();

        void on_login(std::string msg);

        void clients_changed();

        void on_ping();

        void write(std::string msg);

        bool timed_out() const;

        void stop();

};

typedef boost::shared_ptr<talk_to_client> client_ptr;

void accept_thread();

void handle_clients_thread();

#endif // INCLUDE_EXAMPLE_HPP_
