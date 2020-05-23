#pragma once
#include <string_view>
#include <optional>
#include <dots/io/Medium.h>
#include <dots/io/auth/Digest.h>

namespace dots::io
{
    struct HostTransceiver;
}

namespace dots::io
{
    struct AuthManager
    {
        AuthManager(HostTransceiver& transceiver);
        AuthManager(const AuthManager& other) = default;
        AuthManager(AuthManager&& other) = default;
        virtual ~AuthManager() = default;

        AuthManager& operator = (const AuthManager& rhs) = default;
        AuthManager& operator = (AuthManager&& rhs) = default;

        const HostTransceiver& transceiver() const;
        HostTransceiver& transceiver();

        virtual std::optional<Nonce> requiresAuthentication(const Medium& medium, std::string_view guest) = 0;
        virtual bool verifyAuthentication(const Medium& medium, std::string_view guest, Nonce nonce, Nonce cnonce, const Digest& response) = 0;

        [[deprecated("only available for backwards compatibility")]]
        virtual bool verifyAuthentication(const Medium& medium, std::string_view guest, Nonce nonce, std::string_view cnonce, const Digest& response) = 0;

    private:

        HostTransceiver* m_transceiver;
    };
}
