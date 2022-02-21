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

class talk_to_client {
 private:
  int socket;
 public:
  int sock() {
    return this->socket;
  }
};

static volatile boost::recursive_mutex mutex;

void accept_thread();

void handle_clients_thread();

#endif // INCLUDE_EXAMPLE_HPP_
