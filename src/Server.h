#ifndef SERVER_H
#define SERVER_H
#include "hdrs.h"
#include "Session.h"

class Server {
public:
    Server(boost::asio::io_context& io_context, std::size_t port);

private:
    void do_accept();

private:
    tcp::acceptor acceptor_;
    std::unordered_map<std::string, std::vector<Record>> records_;
};

#endif // SERVER_H
