#include <dots/io/Io.h>
#include <dots/io/services/TimerService.h>
#include <dots/io/services/FdHandlerService.h>

namespace dots
{
	asio::io_context& global_io_context()
	{
		static asio::io_context ioContext;
		return ioContext;
	}

	asio::execution_context& global_execution_context()
	{
		return static_cast<asio::execution_context&>(global_io_context());
	}

	asio::executor global_executor()
	{
		return global_io_context().get_executor();
	}
}