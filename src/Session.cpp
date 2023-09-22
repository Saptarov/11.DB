#include "Session.h"

Session::Session(tcp::socket socket, std::unordered_map<std::string, std::vector<Record>>& records)
    : socket_(std::move(socket))
    , records_(records)
{}

void Session::start() {
    do_read();
}

void Session::do_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(
                socket_,
                boost::asio::dynamic_buffer(buffer_),
                '\n',
                [this, self](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            handle_request();
        }
    });
}

void Session::do_write(const std::string& data) {
    auto self(shared_from_this());
    boost::asio::async_write(
                socket_,
                boost::asio::buffer(data),
                [this, self](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            do_read();
        }
    });
}

void Session::handle_request() {
    std::string command, table, id_str, name;
    std::istringstream iss(buffer_);
    iss >> command >> table >> id_str >> name;

    buffer_ = "";

    if (command == "INSERT") {
        handle_insert(table, id_str, name);
    } else if (command == "TRUNCATE") {
        handle_truncate(table);
    } else if (command == "INTERSECTION") {
        handle_intersection();
    } else if (command == "SYMMETRIC_DIFFERENCE") {
        handle_symmetric_difference();
    } else {
        do_write("ERR Invalid command\n");
    }
}

void Session::handle_insert(const std::string& table, const std::string& id_str, const std::string& name) {
    try {
        int id = std::stoi(id_str);
        Record record{id, name};

        auto& table_records = records_[table];
        if (std::find_if(table_records.begin(), table_records.end(), [id](const auto& r) { return r.id == id; }) == table_records.end()) {
            table_records.push_back(record);
            do_write("OK\n");
        } else {
            do_write("ERR duplicate " + std::to_string(id) + "\n");
        }
    } catch (const std::exception&) {
        do_write("ERR Invalid entry format\n");
    }
}

void Session::handle_truncate(const std::string& table) {
    records_.erase(table);
    do_write("OK\n");
}

void Session::handle_intersection() {
    if (records_.count("A") > 0 && records_.count("B") > 0) {
        auto& table_a = records_["A"];
        auto& table_b = records_["B"];

        std::vector<std::string> intersection_result;
        for (const auto& record_a : table_a) {
            for (const auto& record_b : table_b) {
                if (record_a.id == record_b.id) {
                    intersection_result.push_back(std::to_string(record_a.id) + "," + record_a.name + "," + record_b.name);
                    break;
                }
            }
        }

        std::string data = join(intersection_result, "\n") + "\n";
        do_write(data);
    } else {
        do_write("ERR Tables A or B do not exist\n");
    }
}

void Session::handle_symmetric_difference() {
    if (records_.count("A") > 0 && records_.count("B") > 0) {
        auto& table_a = records_["A"];
        auto& table_b = records_["B"];

        std::unordered_map<int, std::pair<std::string, std::string>> difference_result;
        for (const auto& record_a : table_a) {
            difference_result[record_a.id].first = record_a.name;
        }

        for (const auto& record_b : table_b) {
            difference_result[record_b.id].second = record_b.name;
        }

        std::vector<std::string> difference_data;
        for (const auto& [id, names] : difference_result) {
            const auto& [name_a, name_b] = names;
                    if (name_a.empty()) {
                difference_data.push_back(std::to_string(id) + "," + name_b);
            } else if (name_b.empty()) {
                difference_data.push_back(std::to_string(id) + "," + name_a);
            }
        }

        std::string data = join(difference_data, "\n") + "\n";
        do_write(data);
    } else {
        do_write("ERR Tables A or B do not exist\n");
    }
}

std::string Session::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::string result;
    if (!strings.empty()) {
        result = strings[0];
        for (std::size_t i = 1; i < strings.size(); ++i) {
            result += delimiter + strings[i];
        }
    }
    return result;
}
