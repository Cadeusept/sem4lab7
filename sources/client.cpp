// Created by cadeusept on 07.03.2022.

#include "client.hpp"

boost::asio::io_service service;
boost::asio::ip::tcp::endpoint ep(
    boost::asio::ip::address::from_string("127.0.0.1"), 8001);

void talk_to_server::loop() {
    write("login " + _username + "\n");
    read_answer();
    while (_started) {
        ping_server();
        read_answer();
        boost::this_thread::sleep(
            boost::posix_time::millisec(rand() % 7000));
    }
}

std::string talk_to_server::username() const {
    return _username;
}

void talk_to_server::ping_server() {
    write("ping\n");
}

void talk_to_server::read_answer() {
    if (_server_socket.available()) {
        _already_read += _server_socket.read_some(boost::asio::buffer(
          _already_read + _buffer, _max_msg - _already_read));
    }
    process_answer();
}

void talk_to_server::process_answer() {
    std::string msg(_buffer, _already_read);
    if (msg.find("login ok") == 0) on_login();
    else if (msg.find("ping ok") == 0) outpt_answer(msg);//wait_for_input();
    else if (msg.find("clients list changed") == 0) on_clients();
    else if (msg.find("client list:") == 0) outpt_answer(msg);//wait_for_input();
    else {
        std::cerr << "invalid message from server \"" << msg << "\"" << std::endl;
        write("invalid message from server \"" + msg + "\"\n");
    }
}

void talk_to_server::on_clients() {
    write("clients");
}

void talk_to_server::outpt_answer(const std::string &msg) {
    std::cout << msg;
}

void talk_to_server::write(const std::string &msg) {
    _server_socket.write_some(boost::asio::buffer(msg));
}

void talk_to_server::wait_for_input() {
    std::string query;
    std::cin >> query;
    write(query);
}

void talk_to_server::connect(const boost::asio::ip::tcp::endpoint endpoint) {
    _server_socket.connect(endpoint);
}

void run_client() {
    std::string client_name;
    std::cout << "Write your username: " << std::endl;
    std::cin >> client_name;
    talk_to_server client(client_name);
    try
    {
        client.connect(ep);
        client.loop();
    }
    catch(boost::system::system_error &err)
    {
        std::cout << "client terminated" << std::endl;
    }
}


