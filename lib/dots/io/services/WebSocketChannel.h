#pragma once
#include <optional>
#include <string_view>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <dots/io/services/Channel.h>

namespace dots
{
	struct WebSocketChannel : Channel
	{
		using ws_stream_t = boost::beast::websocket::stream<boost::beast::tcp_stream>;
		static constexpr char Subprotocol[] = "dots-json";

        WebSocketChannel(boost::asio::io_context& ioContext, const std::string_view& host, const std::string_view& port);
		WebSocketChannel(ws_stream_t&& stream);
		WebSocketChannel(const WebSocketChannel& other) = delete;
		WebSocketChannel(WebSocketChannel&& other) = delete;
		virtual ~WebSocketChannel() = default;

		WebSocketChannel& operator = (const WebSocketChannel& rhs) = delete;
		WebSocketChannel& operator = (WebSocketChannel&& rhs) = delete;

	protected:

		void asyncReceiveImpl() override;
		void transmitImpl(const DotsTransportHeader& header, const type::Struct& instance) override;

	private:

		ws_stream_t m_stream;
		boost::beast::flat_buffer m_buffer;
	};
}