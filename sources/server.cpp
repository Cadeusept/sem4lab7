// Copyright 2021 Your Name <your_email>

#include <stdexcept>

#include <server.hpp>

boost::asio::io_service service;
std::vector<client_ptr> clients;
boost::recursive_mutex cli_mutex;

boost::asio::ip::tcp::socket &client_data::get_socket() {
    return _user_socket;
}

bool client_data::timed_out() const {
    boost::posix_time::ptime now_time =
        boost::posix_time::microsec_clock::local_time();
    long long ms = (now_time - _last_ping).total_milliseconds();
    return ms > 5000;
}

void client_data::answer_to_client() {
    try {
        read_request();
        process_request();
    }
    catch (boost::system::system_error&) {
        stop();
    }
    if (timed_out())
        stop();
}

void client_data::stop() {
    boost::system::error_code err;
    _user_socket.close(err);
}

void accept_thread() {
    boost::asio::ip::tcp::acceptor acceptor
        {service, boost::asio::ip::tcp::endpoint
        {boost::asio::ip::tcp::v4(), 8001}};

    while (true) {
        client_ptr new_client(new client_data); // talk_to_client - user defined class
        acceptor.accept(new_client->get_socket());
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
                    boost::bind(&client_data::timed_out,_1)), clients.end());
      }
}
