#include "Engine/Network/Network.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPData.hpp"
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Game/EngineBuildPreferences.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "Ws2_32.lib")

#ifdef ENGINE_ENABLE_NETWORK

//////////////////////////////////////////////////////////////////////////
COMMAND(StartServer, "start tcp server, port=48000", eEventFlag::EVENT_CONSOLE) 
{
    int port = args.GetValue("0", 48000);
    if (port < 0 || port>65535) {
        g_theConsole->PrintError(Stringf("port %i invalid, using 48000", port));
    }

    g_theNetwork->CreateTCPServer((uint16_t)port, NET_NON_BLOCKING);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(StopServer, "stop tcp server", eEventFlag::EVENT_CONSOLE)
{
    UNUSED(args);
    g_theNetwork->StopServers();
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(SendTCPMessage, "send text message, msg=hello", eEventFlag::EVENT_CONSOLE)
{
    std::string msg = args.GetValue("0","");
    if (msg.empty()) {
        g_theConsole->PrintError("Message to be sent is null, ignore");
        return false;
    }

    std::vector<std::string> messages = MakeTextPackages(msg);
    for(std::string sendMsg : messages) {
        g_theNetwork->SendTCPMessage(sendMsg);
        g_theConsole->PrintString(Rgba8::WHITE, sendMsg.substr(NET_HEADER_LEN));
    }
    
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(Disconnect, "disconnect from server", eEventFlag::EVENT_CONSOLE)
{
    UNUSED(args);
    g_theNetwork->DisconnectClients();
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(Connect, "connect to server, ip=127.0.0.1:48000", eEventFlag::EVENT_CONSOLE)
{
    std::string ip = args.GetValue("0", "127.0.0.1:48000");
    uint16_t port =48000;
    std::string host = TranslateIPStringToHost(ip,port);
    g_theNetwork->CreateTCPClient(ip,port,NET_NON_BLOCKING);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(OpenUDPPort, "open new udp port, bindPort=48000, sendToPort=48001, ip=127.0.0.1", eEventFlag::EVENT_CONSOLE)
{
    std::string ip = args.GetValue("ip","127.0.0.1");
    int bindPort = args.GetValue("bindPort", 48000);
    int sendPort = args.GetValue("sendToPort", 48001);
    if (ip == "127.0.0.1" && bindPort == 48000 && sendPort == 48001) {
        ip = args.GetValue("2", "127.0.0.1");
        bindPort = args.GetValue("0", 48000);
        sendPort = args.GetValue("1", 48001);
    }

    g_theNetwork->CreateUDPSocket(ip, sendPort, bindPort);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(CloseUDPPort, "close udp port, bindPort=48000", eEventFlag::EVENT_CONSOLE)
{
    int bindPort = args.GetValue("0", 48000);
    g_theNetwork->StopUDPSocket(bindPort);
    return true;
}

//////////////////////////////////////////////////////////////////////////
COMMAND(SendUDPMessage, "send udp message, message=test", eEventFlag::EVENT_CONSOLE)
{
    std::string msg = args.GetValue("0", "test");
    std::vector<std::string> messages = MakeTextPackages(msg);
    for (std::string sendMsg : messages) {
        g_theNetwork->SendUDPMessage(sendMsg);
        g_theConsole->PrintString(Rgba8::WHITE, sendMsg.substr(NET_HEADER_LEN));
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
std::string TranslateIPStringToHost(std::string const& ipStr, uint16_t& outPort)
{
    Strings results = SplitStringOnDelimiter(ipStr, ':');
    if (results.size() == 2) {
        unsigned int x=StringConvert(results[1].c_str(),0);
        outPort = (uint16_t)x;        
    }
    return results[0];
}

//////////////////////////////////////////////////////////////////////////
void Network::Startup()
{
    WSADATA wsaData;
    WORD wVersion = MAKEWORD(2,2);
    int iResult = WSAStartup(wVersion, &wsaData);
    if (iResult != 0) {
        g_theConsole->PrintError(Stringf("Network Startup WSAStartup failed %i", WSAGetLastError()));
    }
}

//////////////////////////////////////////////////////////////////////////
void Network::BeginFrame()
{
    for (TCPServer* server : m_tcpServers) {
        if (server != nullptr) {
            server->BeginFrame();
        }
    }

    for ( size_t i=0;i<m_tcpClients.size();) {
        TCPClient* client = m_tcpClients[i];
        if (client != nullptr) {
            client->BeginFrame();
            if (!client->IsValid()) {
                delete client;
                m_tcpClients.erase(m_tcpClients.begin()+i);
            }
            else {
                i++;
            }
        }
    }

    for (size_t i=0;i<m_udpSockets.size();) {
        UDPSocket* soc = m_udpSockets[i];
        if (!soc->IsValid()) {
            delete soc;
            m_udpSockets.erase(m_udpSockets.begin()+i);
        }
        else{
            soc->BeginFrame();
            i++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Network::Shutdown()
{
    DisconnectClients();
    StopServers();
    StopUDPSockets();

    int iResult = WSACleanup();
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError("Network Shutdown WSACleanup failed");
    }
}

//////////////////////////////////////////////////////////////////////////
void Network::DisconnectClients()
{
    for (std::vector<TCPClient*>::iterator it=m_tcpClients.begin();it!=m_tcpClients.end();it++) {
        if (*it != nullptr) {
            (*it)->Disconnect();
            delete *it;
            *it = nullptr;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Network::StopServers()
{
    for (std::vector<TCPServer*>::iterator it=m_tcpServers.begin();it!=m_tcpServers.end();it++) {
        if (*it != nullptr) {
            (*it)->StopServer();
            delete *it;
            *it = nullptr;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Network::SendTCPMessage(std::string const& msg) const
{
    for (auto server : m_tcpServers) {
        if (server != nullptr) {
            server->SendMessageToAllClients(msg);
        }
    }

    for (auto client : m_tcpClients) {
        if (client != nullptr) {
            client->SendTCPMessage(msg);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
TCPServer* Network::CreateTCPServer(eNetworkMode mode /*= eNetworkMode::NET_BLOCKING*/)
{
    TCPServer* newServer = new TCPServer(mode);
    for (auto it=m_tcpServers.begin();it!=m_tcpServers.end();it++) {
        if (*it == nullptr) {
            *it = newServer;
            return newServer;
        }
    }

    m_tcpServers.push_back(newServer);
    return newServer;
}

//////////////////////////////////////////////////////////////////////////
TCPServer* Network::CreateTCPServer(unsigned short port, eNetworkMode mode /*=NET_BLOCKING*/)
{
    TCPServer* server = CreateTCPServer(mode);
    server->Bind(port);
    server->Listen();
    return server;
}

//////////////////////////////////////////////////////////////////////////
TCPClient* Network::CreateTCPClient(eNetworkMode mode)
{
    TCPClient* newClient = new TCPClient(mode);
    for (auto it = m_tcpClients.begin(); it != m_tcpClients.end(); it++) {
        if (*it == nullptr) {
            *it = newClient;
            return newClient;
        }
    }

    m_tcpClients.push_back(newClient);
    return newClient;
}

//////////////////////////////////////////////////////////////////////////
TCPClient* Network::CreateTCPClient(std::string const& ip, unsigned short port, eNetworkMode mode/*=NET_BLOCKING*/)
{
    TCPClient* client = CreateTCPClient(mode);
    client->Connect(ip, port);
    return client;
}

//////////////////////////////////////////////////////////////////////////
UDPSocket* Network::CreateUDPSocket(std::string const& host, int toPort, int bindPort)
{
    StopUDPSocket(bindPort);
    
    UDPSocket* newSoc = new UDPSocket(host, toPort);
    newSoc->Bind(bindPort);
    m_udpSockets.push_back(newSoc);

    g_theConsole->PrintString(Rgba8::WHITE, Stringf("Start UDPSocket of %s on %i to %i", host.c_str(), bindPort, toPort));
    return newSoc;
}

//////////////////////////////////////////////////////////////////////////
void Network::SendUDPMessage(std::string const& msg)
{
    if (m_udpSockets.empty()) {
        g_theConsole->PrintError("No existing udp socket to send message");
        return;
    }

    for (UDPSocket* soc : m_udpSockets) {
        soc->SendUDPMessage(msg);
    }
}

//////////////////////////////////////////////////////////////////////////
void Network::StopUDPSocket(int bindPort)
{
    for (auto it=m_udpSockets.begin();it!=m_udpSockets.end();it++) {
        UDPSocket* udp = *it;
        if (udp && udp->IsValid() && udp->GetBindPort() == bindPort) {
            udp->Close();
            return;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Network::StopUDPSockets()
{
    for (UDPSocket* soc : m_udpSockets) {
        soc->Close();
        delete soc;
    }
    m_udpSockets.clear();
}

#endif // ENGINE_ENABLE_NETWORK
