#ifndef SESSION_H
#define SESSION_H
#include "hdrs.h"

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, std::unordered_map<std::string, std::vector<Record>>& records);
    void start();

private:
    void do_read();
    void do_write(const std::string& data);

    void handle_request();
    void handle_insert(const std::string& table, const std::string& id_str, const std::string& name);
    void handle_truncate(const std::string& table);

    void handle_intersection();
    void handle_symmetric_difference();

    std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

private:
    tcp::socket socket_;
    std::unordered_map<std::string, std::vector<Record>>& records_;
    std::string buffer_;
};

#endif // SESSION_H
