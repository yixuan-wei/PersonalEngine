#pragma once

#include "Engine/Network/NetworkCommon.hpp"
#include "Engine/Network/TCPSocket.hpp"
#include <vector>
#include <functional>

class TCPServer
{
    friend class Network;

public:
    ~TCPServer()=default;

    void BeginFrame();

    eNetworkMode GetMode() const {return m_mode;}

    void Bind(uint16_t port);
    bool Listen();
    void StopListen();
    void StopServer();
    void Accept();
    void SendMessageToAllClients(std::string const& msg);

    std::function<std::string(std::string const&)> m_onNewConnectAccept;

protected:
    TCPServer(eNetworkMode mode=eNetworkMode::NET_NON_BLOCKING);

    void UpdateForReceive();
    void SendMessageToClient(TCPSocket* clientSoc, std::string const& msg);
    void StopClient(TCPSocket* clientSoc);

private:
    eNetworkMode m_mode;
    FD_SET m_listenSet;
    TCPSocket m_listenSocket;
    std::vector<TCPSocket*> m_clientSockets;
    timeval m_timeVal;
};