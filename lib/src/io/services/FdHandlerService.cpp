#include <boost/asio.hpp>
#if defined(BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR)
#include <dots/io/services/FdHandlerService.h>

namespace dots::io::posix
{
    FdHandlerService::FdHandlerService(boost::asio::execution_context& executionContext) :
        boost::asio::execution_context::service(executionContext)
    {
        /* do nothing */
    }

    void FdHandlerService::addInEventHandler(int fileDescriptor, callback_t callback)
    {
        bool emplaced = m_inEventHandlers.try_emplace(fileDescriptor, static_cast<boost::asio::io_context&>(context()), fileDescriptor, std::move(callback)).second;

        if (!emplaced)
        {
            throw std::logic_error{ "there already is a handler registered for the given file descriptor '" + std::to_string(fileDescriptor) + "'"};
        }
    }

    void FdHandlerService::removeInEventHandler(int fileDescriptor)
    {
        if (auto it = m_inEventHandlers.find(fileDescriptor);  it != m_inEventHandlers.end())
        {
            m_inEventHandlers.erase(it);
        }
    }

    void FdHandlerService::shutdown() noexcept
    {
        m_inEventHandlers.clear();
    }
}
#endif