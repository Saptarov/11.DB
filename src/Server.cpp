#include "Server.h"

Server::Server(boost::asio::io_context& io_context, std::size_t port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket), records_)->start();
        }

        do_accept();
    });
}
