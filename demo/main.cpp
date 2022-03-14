#include "server.hpp"
#include "client.hpp"

int main(int argc, char* argv[]) {
  boost::thread_group threads;
  if (argc == 2) {
    if (atoi(argv[1]) == 1) { //server
      threads.create_thread(accept_thread);
      threads.create_thread(handle_clients_thread);
      threads.join_all();
    } else if (atoi(argv[1]) == 2) {  // client
      threads.create_thread(run_client);
    } else {
      return 1;
    }
  } else {
    return 2;
  }
}