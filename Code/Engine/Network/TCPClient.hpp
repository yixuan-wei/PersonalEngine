#pragma once

#include "Engine/Network/TCPSocket.hpp"

class Network;

class TCPClient
{
    friend class Network;

public:
    ~TCPClient() = default;

    void BeginFrame();

    void Connect(std::string const& host, uint16_t port);
    void Disconnect();
    void Shutdown();
    void SendTCPMessage(std::string const& str);

    bool IsValid() const;

protected:
    TCPClient(eNetworkMode mode = eNetworkMode::NET_BLOCKING);

private:
    eNetworkMode m_mode;
    TCPSocket m_socket;
};