#pragma once
#include <optional>
#include <dots/asio.h>
#include <dots/io/Listener.h>
#include <dots/io/channels/TcpChannel.h>

namespace dots::io::details
{
    template <typename TChannel>
    struct GenericTcpListener : Listener
    {
        GenericTcpListener(asio::io_context& ioContext, const Endpoint& endpoint, std::optional<int> backlog = std::nullopt);
        GenericTcpListener(asio::io_context& ioContext, std::string address, std::string port, std::optional<int> backlog = std::nullopt);
        GenericTcpListener(const GenericTcpListener& other) = delete;
        GenericTcpListener(GenericTcpListener&& other) = delete;
        ~GenericTcpListener() override = default;

        GenericTcpListener& operator = (const GenericTcpListener& rhs) = delete;
        GenericTcpListener& operator = (GenericTcpListener&& rhs) = delete;

    protected:

        void asyncAcceptImpl() override;

    private:

        using buffer_t = typename TChannel::buffer_t;
        using payload_cache_t = typename TChannel::payload_cache_t;

        std::string m_address;
        std::string m_port;
        asio::ip::tcp::acceptor m_acceptor;
        asio::ip::tcp::socket m_socket;
        payload_cache_t m_payloadCache;
    };

    extern template struct GenericTcpListener<LegacyTcpChannel>;
    extern template struct GenericTcpListener<TcpChannel>;
}

namespace dots::io
{
    using LegacyTcpListener = details::GenericTcpListener<LegacyTcpChannel>;
    using TcpListener = details::GenericTcpListener<TcpChannel>;
}
