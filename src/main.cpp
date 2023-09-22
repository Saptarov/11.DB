#include "Server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: join_server <port>\n";
        return 1;
    }

    try {
        boost::asio::io_context io_context;
        Server server(io_context, std::atoi(argv[1]));
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
