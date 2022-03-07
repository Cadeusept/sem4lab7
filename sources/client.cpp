// Created by cadeusept on 07.03.2022.

#include "client.hpp"

boost::asio::io_service service;
boost::asio::ip::tcp::endpoint ep(
    boost::asio::ip::adress::from_string("127.0.0.1"), 8001);

void run_client(const std::string &client_name) {
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


