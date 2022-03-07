// Copyright 2021 Your Name <your_email>

#include <stdexcept>

#include <server.hpp>

boost::asio::io_service service;
//std::vector<std::string> client_list;
std::vector<client_ptr> clients;
boost::recursive_mutex cli_mutex;

void accept_thread() {
    boost::asio::ip::tcp::acceptor acceptor
        {service, boost::asio::ip::tcp::endpoint
        {boost::asio::ip::tcp::v4(), 8001}};

    while (true) {
        client_ptr new_client(new talk_to_client); // talk_to_client - user defined class
        acceptor.accept(new_client->_sock);
        boost::recursive_mutex::scoped_lock lock{cli_mutex};
        clients.push_back(new_client);
    }
}

void handle_clients_thread() {
    while (true) {
        boost::this_thread::sleep(boost::posix_time::millisec(1));
        boost::recursive_mutex::scoped_lock lock{cli_mutex};
        for (auto& client : clients) {
          client->answer_to_client();
        }

        clients.erase(std::remove_if(clients.begin(), clients.end(),
                    boost::bind(&talk_to_client::timed_out,_1)), clients.end());
        // and then erase clients that timed out
      }
}
