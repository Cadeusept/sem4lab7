// Copyright 2021 Your Name <your_email>

#include <stdexcept>

#include <server.hpp>

boost::recursive_mutex mutex;
std::vector<std::shared_ptr<talk_to_client>> clients;

void accept_thread() {
    boost::asio::ip::tcp::acceptor acceptor{service,
              boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4{}, 8001}};
    while (true) {
        auto client = std::make_shared<talk_to_client>(); // talk_to_client - user defined class
        acceptor.accept(client->sock());
        boost::recursive_mutex::scoped_lock lock{mutex};
        clients.push_back(client);
    }
}

void handle_clients_thread() {
    while (true) {
        std::this_thread::sleep(std::chrono::milliseconds{1});
        boost::recursive_mutex::scoped_lock lock{mutex};
        for (auto& client : clients) {
            // for each client calling answer_to_client();
        }
        // and then erase clients that timed out
      }
}

