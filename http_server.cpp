#include <iostream>
#include <unistd.h>
#include <boost/asio.hpp>
#include "utils.hpp"
#include "middleware.hpp"

using namespace std;
using boost::asio::ip::tcp;

class session : public enable_shared_from_this<session>{
public:
    session(tcp::socket socket) : socket_(move(socket)) {
        server_addr = socket_.local_endpoint().address().to_string();
        server_port = socket_.local_endpoint().port();
        remote_addr = socket_.remote_endpoint().address().to_string();
        remote_port = socket_.remote_endpoint().port();
    }
    void start() {
        do_read();
    }
private:
    void do_read() {
        auto self(shared_from_this()); // make `this` session outlives the asynchronous operation
        socket_.async_read_some(boost::asio::buffer(data_, max_length), [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                pid_t pid = fork();
                if (pid < 0) {
                    cerr << "fork error.\n";
                } else if (pid == 0) {
                    int sock = socket_.native_handle();

                    parse_http(data_);
                    set_env(server_addr, server_port, remote_addr, remote_port);
                    printcgienv();
                    dup2(sock, 0);
                    dup2(sock, 1);
                    socket_.close();
                    middleware();
                    exit(1);
                } else {
                    socket_.close();
                }
            }
        });
    }
    tcp::socket socket_;
    string server_addr, remote_addr;
    unsigned short server_port, remote_port;
    enum { max_length = 1024 };
    char data_[max_length];
};


class server {
public:
    server(boost::asio::io_context& io_context, short port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
private:
    void do_accept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                make_shared<session>(move(socket))->start();
            }
            do_accept();
        });
    }
    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            cerr << "Usage: http_server <port>\n";
            return 1;
        }
        boost::asio::io_context io_context;
        server s(io_context, atoi(argv[1]));
        io_context.run();
    } catch (exception& e) {
        cerr << "Exception: " << e.what() << '\n';
    }
}
