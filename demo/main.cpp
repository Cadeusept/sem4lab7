#include "server.hpp"

int main(int argc, char* argv[]) {
    //boost::thread_group threads;
    if (argc == 2) {
        if (atoi(argv[1]) == 1) { //server
            //threads.create_thread(accept_thread);
            std::thread server_accept(accept_thread);
            std::thread server_handle(handle_clients_thread);
            server_accept.join();
            server_handle.join();
        } else if (atoi(argv[1]) == 2) {  // client
            srand(time(NULL));
            std::thread client(run_client);
            client.join();
        } else {
            return 1;
        }
    } else {
        return 2;
    }
}