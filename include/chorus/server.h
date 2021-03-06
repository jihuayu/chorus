#ifndef CHORUS_SERVER_H_
#define CHORUS_SERVER_H_

#include <string>
#include <cstdint>
#include <type_traits>
#include <memory>
#include <asio.hpp>

namespace chorus
{
    class Session
        : public std::enable_shared_from_this<Session>
    {
    public:
        Session(asio::ip::tcp::socket socket)
            : socket_(std::move(socket))
        {
        }

        void start()
        {
            do_read();
        }

    private:
        void do_read()
        {
            auto self(shared_from_this());
            socket_.async_read_some(asio::buffer(data_, max_length),
                [this, self](std::error_code ec, std::size_t length)
                {
                    if (!ec)
                    {
                        do_write(length);
                    }
                });
        }

        void do_write(std::size_t length)
        {
            auto self(shared_from_this());
            asio::async_write(socket_, asio::buffer(data_, length),
                [this, self](std::error_code ec, std::size_t /*length*/)
                {
                    if (!ec)
                    {
                        do_read();
                    }
                });
        }

        asio::ip::tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
    };

    class Server
    {
    public:
        Server(std::uint16_t port);
        ~Server();

        template<typename F>
        void bind(const std::string& name, F func)
        {
            // static_assert(std::is_function<F>::value, "Binded object is not a function.");
        }

        void run();

        void accept();
    private:
        int port;
        std::unique_ptr<asio::ip::tcp::acceptor> connection_acceptor;
    };
}


#endif // CHORUS_SERVER_H_