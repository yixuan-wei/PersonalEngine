#include "Engine/Network/TCPServer.hpp"
#include "Engine/Network/TCPData.hpp"
#include "Engine/Network/Network.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedProperties.hpp"

//////////////////////////////////////////////////////////////////////////
void TCPServer::BeginFrame()
{    
    Accept();       
    UpdateForReceive();
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::Bind(uint16_t port)
{
    struct addrinfo addrHintsIn;
    struct addrinfo* pAddrOut = nullptr;

    ZeroMemory(&addrHintsIn, sizeof(addrHintsIn));
    addrHintsIn.ai_family=AF_INET;//TODO: why ipv4 address
    addrHintsIn.ai_socktype=SOCK_STREAM;
    addrHintsIn.ai_protocol = IPPROTO_TCP;
    addrHintsIn.ai_flags = AI_PASSIVE;

    std::string serverPort=Stringf("%hu",port);
    int iResult = ::getaddrinfo(NULL, serverPort.c_str(), &addrHintsIn, &pAddrOut);
    if (iResult != 0) {
        g_theConsole->PrintError("Fail to call getaddrinfo in TCPServer Bind");
        return;
    }

    SOCKET& soc = m_listenSocket.m_socket;
    soc = ::socket(pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol);
    if (soc == INVALID_SOCKET) {
        g_theConsole->PrintError("Fail to create listen socket in TCPServer Bind");
        freeaddrinfo(pAddrOut);
        return;
    }

    if (m_mode == eNetworkMode::NET_NON_BLOCKING) {
        u_long winSockMode=1;
        iResult = ioctlsocket(soc, FIONBIO, &winSockMode);
        if (iResult == SOCKET_ERROR) {
            g_theConsole->PrintError("Fail to set listen socket to non blocking in TCPServer Bind");
            return;
        }
    }
    else {
        g_theConsole->PrintError("Blocking mode could be buggy");
    }

    iResult = ::bind(soc, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError("TCPServer socket bind failed");
        closesocket(soc);
    }
    freeaddrinfo(pAddrOut);

    m_listenSocket.m_mode = m_mode;
}

//////////////////////////////////////////////////////////////////////////
bool TCPServer::Listen()
{
    int iResult = ::listen(m_listenSocket.m_socket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError(Stringf("Fail to listen on TCPServer, error=%i", WSAGetLastError()));
        m_listenSocket.Close();
        return false;
    }
    m_listenSocket.m_isQuiting = false;
    g_theConsole->PrintString(Rgba8::WHITE,"Started server listening");
    return true;
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::StopListen()
{
    if (m_listenSocket.IsValid()) {
        m_listenSocket.Close();
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::StopServer()
{
    StopListen();

    std::string quitMsg = MakeClientQuitPackage();
    SendMessageToAllClients(quitMsg);

    for (TCPSocket* clientSoc : m_clientSockets) {
        StopClient(clientSoc);
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::Accept()
{
    if (!m_listenSocket.IsValid()) {
        return;
    }

    TCPSocket* newClient = new TCPSocket();

    SOCKET& soc = newClient->m_socket;
    SOCKET& listenSoc = m_listenSocket.m_socket;
    if (m_mode == eNetworkMode::NET_NON_BLOCKING) {
        FD_ZERO(&m_listenSet);
        FD_SET(listenSoc, &m_listenSet);
        int iResult = select(0, &m_listenSet, NULL, NULL, &m_timeVal);
        if (iResult == SOCKET_ERROR) {
            g_theConsole->PrintError("select on server in non-blocking mode failed");
            m_listenSocket.Shutdown();
        }
    }
    if(m_mode==eNetworkMode::NET_BLOCKING
        || (m_mode == eNetworkMode::NET_NON_BLOCKING && FD_ISSET(listenSoc, &m_listenSet))) {
        soc = ::accept(listenSoc, NULL, NULL);
        if (soc == INVALID_SOCKET) {
            g_theConsole->PrintError("Fail to accept on TCPServer");
            m_listenSocket.Shutdown();
        }
    }
    
    if(newClient->IsValid()){
        newClient->m_mode = m_mode;
        newClient->m_isQuiting = false;

        std::string clientIP = newClient->GetRemoteFullAddress();
        g_theConsole->PrintString(Rgba8::GREEN, Stringf("TCPClient connected from %s", clientIP.c_str()));
        std::string msg = m_onNewConnectAccept(TCPSocket::GetIPAddress(clientIP));
        newClient->Send(msg.data(), msg.size());

        m_clientSockets.push_back(newClient);
    }
    else {
        delete newClient;
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::SendMessageToAllClients(std::string const& msg)
{
    for (TCPSocket* clientSoc : m_clientSockets) {
        SendMessageToClient(clientSoc, msg);
    }
}

//////////////////////////////////////////////////////////////////////////
TCPServer::TCPServer(eNetworkMode mode/*=eNetworkMode::NET_NON_BLOCKING*/)
    : m_mode(mode)
    , m_listenSocket(INVALID_SOCKET)
    , m_timeVal{0l, 0l}
{
    FD_ZERO(&m_listenSet);
    m_onNewConnectAccept = MakeClientConnectPackage;
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::UpdateForReceive()
{
    //for receive from new client
    bool couldServerQuit = true;

    for(size_t i=0;i<m_clientSockets.size();){
        bool couldQuit = true;
        TCPSocket* clientSoc = m_clientSockets[i];
        if (!clientSoc->IsValid()) {
            NamedProperties parameters;
            SetupNetworkEventParameter("", (void*)clientSoc, parameters);
            g_theEvents->FireEvent("TCPServerFailOneClient", parameters, EVENT_NETWORK);

            delete clientSoc;
            m_clientSockets.erase(m_clientSockets.begin()+i);
            continue;
        }

        if (clientSoc->IsDataAvailable()) {
            TCPData buf;
            do {
                buf = clientSoc->Receive();
                if (buf.GetLength() > 0) {
                    if (buf.GetHeader() == eNetworkPackageHeaderType::HEAD_CLIENT_CLOSE) {
                        g_theConsole->PrintString(Rgba8::WHITE, Stringf("Client %s disconnected", clientSoc->GetRemoteFullAddress()));
                        clientSoc->Shutdown();
                    }
                    else {
                        g_theConsole->PrintString(Rgba8::WHITE, Stringf("Received: %s", buf.GetContent().c_str()));
                        couldQuit = false;
                        couldServerQuit = false;
                    }
                    NamedProperties parameters;
                    SetupNetworkEventParameter(buf.GetData(), (void*)this, parameters);
                    g_theEvents->FireEvent("TCPServerReceive", parameters, EVENT_NETWORK);
                }
            } while (buf.GetLength() > 0);
        }

        if (couldQuit && clientSoc->IsQuiting()) {
            clientSoc->Close();
            delete clientSoc;
            m_clientSockets.erase(m_clientSockets.begin()+i);
        }

        i++;
    }

    if (couldServerQuit && m_listenSocket.IsQuiting()) {
        m_listenSocket.Close();
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::SendMessageToClient(TCPSocket* clientSoc, std::string const& msg)
{
    if (clientSoc->IsValid()) {
        clientSoc->Send(msg.data(), msg.size());
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPServer::StopClient(TCPSocket* clientSoc)
{
    if (clientSoc->IsValid()) {
        clientSoc->Shutdown();
    }
    if (!clientSoc->IsClosed()) {
        clientSoc->Close();
    }
}
