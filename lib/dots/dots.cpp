#include <dots/dots.h>
#include <dots/io/Io.h>
#include <dots/io/services/TimerService.h>
#include <dots/io/services/FdHandlerService.h>

namespace dots
{
	Timer::id_t add_timer(const type::Duration& timeout, const std::function<void()>& handler, bool periodic/* = false*/)
	{
		return global_service<TimerService>().addTimer(timeout, handler, periodic);
	}

	void remove_timer(Timer::id_t id)
	{
		global_service<TimerService>().removeTimer(id);
	}

	void add_fd_handler(int fileDescriptor, const std::function<void()>& handler)
	{
		global_service<FdHandlerService>().addInEventHandler(fileDescriptor, handler);
	}

	void remove_fd_handler(int fileDescriptor)
	{
		global_service<FdHandlerService>().removeInEventHandler(fileDescriptor);
	}

	Publisher*& publisher()
	{
		static Publisher* publisher = nullptr;
		return publisher;
	}

	GuestTransceiver& transceiver(const std::string_view& name/* = "dots-transceiver"*/)
	{
		static GuestTransceiver transceiver{ name.data() };

		if (Publisher*& p = publisher(); p == nullptr)
		{
			p = &transceiver;
		}
		
		return transceiver;
	}

	void publish(const type::Struct& instance, types::property_set_t includedProperties, bool remove)
	{
		publisher()->publish(&instance._descriptor(), instance, includedProperties, remove);
	}

	void remove(const type::Struct& instance)
	{
		publish(instance, instance._keyProperties(), true);
	}

	Subscription subscribe(const type::StructDescriptor<>& descriptor, GuestTransceiver::receive_handler_t<>&& handler)
	{
		return transceiver().subscribe(descriptor, std::move(handler));
	}

	Subscription subscribe(const type::StructDescriptor<>& descriptor, GuestTransceiver::event_handler_t<>&& handler)
	{
		return transceiver().subscribe(descriptor, std::move(handler));
	}

	const ContainerPool& pool()
	{
		return transceiver().pool();
	}

	const Container<>& container(const type::StructDescriptor<>& descriptor)
	{
		return transceiver().container(descriptor);
	}

	void publish(const type::StructDescriptor<>*/* td*/, const type::Struct& instance, types::property_set_t what, bool remove)
	{
	    publish(instance, what, remove);
	}
}