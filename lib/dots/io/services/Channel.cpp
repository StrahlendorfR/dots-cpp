#include "Channel.h"
#include <DotsClient.dots.h>
#include <DotsDescriptorRequest.dots.h>
#include <DotsMsgError.dots.h>

namespace dots
{
    void Channel::asyncReceive(io::Registry& registry, receive_handler_t&& receiveHandler, error_handler_t&& errorHandler)
    {
        if (m_asyncReceiveActive)
        {
            throw std::logic_error{ "only one async receive can be active at the same time" };
        }

        m_asyncReceiveActive = true;
    	m_registry = &registry;
        m_receiveHandler = std::move(receiveHandler);
        m_errorHandler = std::move(errorHandler);
        asyncReceiveImpl();
    }

    void Channel::transmit(const type::Struct& instance, types::property_set_t includedProperties/* = types::property_set_t::All*/, bool remove/* = false*/)
    {
	    const type::StructDescriptor<>& descriptor = instance._descriptor();

        DotsTransportHeader header{
            DotsTransportHeader::destinationGroup_i{ descriptor.name() },
            DotsTransportHeader::dotsHeader_i{
                DotsHeader::typeName_i{ descriptor.name() },
                DotsHeader::sentTime_i{ pnxs::SystemNow() },
                DotsHeader::attributes_i{ includedProperties ==  types::property_set_t::All ? instance._validProperties() : includedProperties },
                DotsHeader::removeObj_i{ remove }
            }
        };

        if (descriptor.internal() && !instance._is<DotsClient>() && !instance._is<DotsDescriptorRequest>())
        {
            header.nameSpace("SYS");
        }

    	transmit(header, instance);
    }

    void Channel::transmit(const DotsTransportHeader& header, const type::Struct& instance)
    {
        transmitImpl(header, instance);
    }

    void Channel::transmit(const DotsTransportHeader& header, const Transmission& transmission)
    {
        transmitImpl(header, transmission);
    }

    const io::Registry& Channel::registry() const
    {
	    return *m_registry;
    }
	
    io::Registry& Channel::registry()
    {
	    return *m_registry;
    }

    void Channel::transmitImpl(const DotsTransportHeader& header, const Transmission& transmission)
    {
        transmitImpl(header, transmission.instance());
    }

    void Channel::processReceive(const DotsTransportHeader& header, Transmission&& transmission)
    {
        if (m_receiveHandler(header, std::move(transmission)))
        {
            asyncReceiveImpl();
        }
        else
        {
            m_asyncReceiveActive = false;
            m_receiveHandler = nullptr;
            m_errorHandler = nullptr;
        }
    }
	
    void Channel::processError(const std::exception& e)
    {
        if (m_errorHandler != nullptr)
        {
            m_errorHandler(e);
        }        
    }

    void Channel::processError(const std::string& what)
    {
        processError(std::runtime_error{ what });
    }

    void Channel::verifyErrorCode(const std::error_code& errorCode)
    {
        if (errorCode)
        {
            throw std::system_error{ errorCode };
        }
    }
}