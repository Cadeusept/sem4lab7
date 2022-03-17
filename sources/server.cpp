// Copyright 2021 Your Name <your_email>

#include <stdexcept>

#include <server.hpp>

boost::asio::io_service service;
typedef std::vector<client_ptr> cli_array;
cli_array clients;
boost::recursive_mutex cli_mutex;

boost::asio::ip::tcp::socket &talk_to_client::get_socket() {
    return _client_socket;
}

std::string talk_to_client::username() {
    return _username;
}

bool talk_to_client::timed_out() const {
    boost::posix_time::ptime now_time =
        boost::posix_time::microsec_clock::local_time();
    long long ms = (now_time - _last_ping).total_milliseconds();
    return ms > 5000;
}

void talk_to_client::answer_to_client() {
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

void talk_to_client::read_request() {
    if (_client_socket.available()) {
        _already_read += _client_socket.read_some(boost::asio::buffer(
          _already_read + _buffer, _max_msg - _already_read));
    }
}

void talk_to_client::process_request() {
    bool found_endl =
      std::find(_buffer, _already_read + _buffer, '\n')
                      < _already_read + _buffer;
    if (!found_endl)
        return;

    _last_ping = boost::posix_time::microsec_clock::local_time();
    size_t pos =
        std::find(_buffer, _already_read + _buffer, '\n')
        - _buffer;
    std::string msg(_buffer, pos);
    std::copy(_already_read + _buffer, _buffer + _max_msg,
              _buffer);
    _already_read -= pos + 1;
    if (msg.find("login ")) on_login(msg);
    else if (msg.find("ping")) on_ping();
    else if (msg.find("clients")) on_clients();
    else {
        std::cerr << "invalid message \"" << msg << "\"" << std::endl;
        write("invalid message \"" + msg + "\"\n");
    }
}

void talk_to_client::on_login(std::string msg) {
    std::string instruction;
    std::istringstream input(msg);
    input >> instruction >> _username;
    write("login ok\n");
    clients_changed();
}

void talk_to_client::clients_changed() {
    _clients_changed_flag = true;
}

void talk_to_client::on_ping() {
    write(_clients_changed_flag ? "clients list changed\n" : "ping ok\n" );
    _clients_changed_flag = false;
}

void talk_to_client::on_clients() {
    std::string msg;
    {
        boost::recursive_mutex::scoped_lock lock(cli_mutex);
        for (cli_array::const_iterator b = clients.begin(), e = clients.end(); b!=e; ++b) {
            msg += (*b)->username() + " ";
        }
    }
    write("client list: " + msg + "\n");
}

void talk_to_client::write(std::string msg) {
    _client_socket.write_some(boost::asio::buffer(msg));
}


void talk_to_client::stop() {
    boost::system::error_code err;
    _client_socket.close(err);
}

void accept_thread() {
    boost::asio::ip::tcp::acceptor acceptor
        {service, boost::asio::ip::tcp::endpoint
        {boost::asio::ip::tcp::v4(), 8001}};

    while (true) {
        client_ptr new_client(new talk_to_client); // talk_to_client - user defined class
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
                    boost::bind(&talk_to_client::timed_out,_1)), clients.end());
    }
}
