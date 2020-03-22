#pragma once
#include <functional>
#include <optional>
#include <boost/asio.hpp>
#include <dots/io/services/Listener.h>
#include <dots/io/services/TcpChannel.h>

namespace dots
{
	struct TcpListener : Listener
	{
		TcpListener(boost::asio::io_context& ioContext, std::string address, std::string port, std::optional<int> backlog = std::nullopt);
		TcpListener(const TcpListener& other) = delete;
		TcpListener(TcpListener&& other) = delete;
		~TcpListener() = default;

		TcpListener& operator = (const TcpListener& rhs) = delete;
		TcpListener& operator = (TcpListener&& rhs) = delete;

	protected:

		void asyncAcceptImpl() override;

	private:

		std::string m_address;
		std::string m_port;
		boost::asio::ip::tcp::acceptor m_acceptor;
		boost::asio::ip::tcp::socket m_socket;
	};
}