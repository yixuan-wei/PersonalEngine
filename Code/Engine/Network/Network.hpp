#pragma once

#include "Engine/Network/NetworkCommon.hpp"
#include <vector>
#include <string>
#include <array>

class TCPServer;
class TCPClient;
class UDPSocket;

std::string TranslateIPStringToHost(std::string const& ipStr, uint16_t& outPort);

class Network
{
public:
    using Buffer = std::array<char, NET_DEFAULT_BUFLEN+1>;

    Network()=default;
    ~Network()=default;

    void Startup();
    void BeginFrame();
    void Shutdown();

    void DisconnectClients();
    void StopServers();
    void SendTCPMessage(std::string const& msg) const;
    TCPServer* CreateTCPServer(unsigned short port, eNetworkMode mode =NET_BLOCKING);
    TCPServer* CreateTCPServer(eNetworkMode mode = eNetworkMode::NET_BLOCKING);
    TCPClient* CreateTCPClient(eNetworkMode mode = eNetworkMode::NET_BLOCKING);
    TCPClient* CreateTCPClient(std::string const& ip, unsigned short port, eNetworkMode mode = NET_BLOCKING);

    UDPSocket* CreateUDPSocket(std::string const& host, int toPort, int bindPort);
    void SendUDPMessage(std::string const& msg);
    void StopUDPSocket(int bindPort);
    void StopUDPSockets();

private:
    std::vector<TCPServer*> m_tcpServers;
    std::vector<TCPClient*> m_tcpClients;

    std::vector<UDPSocket*> m_udpSockets;
};