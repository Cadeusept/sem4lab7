// Copyright 2021 Your Name <your_email>

#include <stdexcept>

#include <server.hpp>

boost::asio::io_service service;
static boost::asio::ip::tcp::acceptor acceptor
        {service, boost::asio::ip::tcp::endpoint
        {boost::asio::ip::tcp::v4(), 8001}};
std::vector<std::string> client_list;
typedef boost::shared_ptr<talk_to_client> client_ptr;
std::vector<client_ptr> clients;
boost::recursive_mutex cli_mutex;

void accept_thread() {
    fflush(stdout);
    while (true) {
        client_ptr one(new talk_to_client); // talk_to_client - user defined class
        acceptor.accept(one->_sock);
        boost::recursive_mutex::scoped_lock lock{cli_mutex};
        clients.push_back(one);
    }
}

void handle_clients_thread() {
    while (true) {
        boost::this_thread::sleep(boost::posix_time::millisec(1));
        boost::recursive_mutex::scoped_lock lock{cli_mutex};
        for (auto& client : clients) {
          client.communicate();
          // for each client calling answer_to_client();
        }
        // and then erase clients that timed out
      }
}

