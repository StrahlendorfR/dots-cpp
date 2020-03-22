#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <dots/io/Connection.h>
#include <dots/io/Transceiver.h>
#include <dots/io/services/Listener.h>
#include <DotsClearCache.dots.h>
#include <DotsDescriptorRequest.dots.h>
#include <DotsMember.dots.h>

namespace dots
{
    struct HostTransceiver : Transceiver
    {
        using transition_handler_t = std::function<void(const io::Connection&)>;

        HostTransceiver(std::string selfName, transition_handler_t transitionHandler);
		HostTransceiver(const HostTransceiver& other) = delete;
		HostTransceiver(HostTransceiver&& other) = default;
		virtual ~HostTransceiver() = default;

		HostTransceiver& operator = (const HostTransceiver& rhs) = delete;
		HostTransceiver& operator = (HostTransceiver&& rhs) = default;

        void listen(listener_ptr_t&& listener);
        void publish(const type::Struct& instance, types::property_set_t includedProperties = types::property_set_t::All, bool remove = false) override;

    private:

        using listener_map_t = std::unordered_map<Listener*, listener_ptr_t>;
        using connection_map_t = std::unordered_map<io::Connection*, io::connection_ptr_t>;
        using group_t = std::unordered_set<io::Connection*>;
        using group_map_t = std::unordered_map<std::string, group_t>;

        void joinGroup(const std::string_view& name) override;
		void leaveGroup(const std::string_view& name) override;

        void transmit(io::Connection* origin, const std::string& group, const DotsTransportHeader& header, Transmission&& transmission);

        bool handleListenAccept(Listener& listener, channel_ptr_t channel);
        void handleListenError(Listener& listener, const std::exception_ptr& e);

        bool handleReceive(io::Connection& connection, const DotsTransportHeader& header, Transmission&& transmission, bool isFromMyself);
        void handleTransition(io::Connection& connection, const std::exception_ptr& e) noexcept;

        void handleMemberMessage(io::Connection& connection, const DotsMember& member);
        void handleDescriptorRequest(io::Connection& connection, const DotsDescriptorRequest& descriptorRequest);
        void handleClearCache(io::Connection& connection, const DotsClearCache& clearCache);

        void transmitContainer(io::Connection& connection, const Container<>& container);

        transition_handler_t m_transitionHandler;
        listener_map_t m_listeners;
        connection_map_t m_guestConnections;
        group_map_t m_groups;
    };
}