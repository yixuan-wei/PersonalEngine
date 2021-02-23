#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPData.hpp"
#include "Engine/Network/Network.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedProperties.hpp"

//////////////////////////////////////////////////////////////////////////
void TCPClient::BeginFrame()
{
    //update from socket
    if (m_socket.IsValid()) {
        bool couldClose = true;
        if (m_socket.IsDataAvailable()) {
            TCPData buf;
            buf = m_socket.Receive();
            while(buf.GetLength()>0){    
                eNetworkPackageHeaderType header = buf.GetHeader();
                if (header == eNetworkPackageHeaderType::HEAD_SERVER_LISTEN) {
                    g_theConsole->PrintString(Rgba8::WHITE, "Connected to server");
                }
                else if (header == eNetworkPackageHeaderType::HEAD_CLIENT_CLOSE) {
                    m_socket.Shutdown();
                    g_theConsole->PrintString(Rgba8::WHITE, "Client going to close");
                }
                else {
                    couldClose = false;
                    g_theConsole->PrintString(Rgba8::GREEN, Stringf("Receive: %s", buf.GetContent().c_str()));
                }
                NamedProperties parameters;
                SetupNetworkEventParameter(buf.GetData(), (void*)this, parameters);
                g_theEvents->FireEvent("TCPClientReceive",parameters, EVENT_NETWORK);
                if(m_socket.IsValid()){
                    buf = m_socket.Receive();
                }
                else {
                    buf=TCPData();
                }
            }
        }

        if (couldClose && m_socket.IsQuiting()) {
            m_socket.Close();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPClient::Connect(std::string const& host, uint16_t port)
{
    struct addrinfo addrHintsIn;
    struct addrinfo* pAddrOut = nullptr;

    ZeroMemory(&addrHintsIn, sizeof(addrHintsIn));
    addrHintsIn.ai_family = AF_INET; //TODO: why only ipv4 address?
    addrHintsIn.ai_socktype = SOCK_STREAM;
    addrHintsIn.ai_protocol = IPPROTO_TCP;

    std::string serverPort = Stringf("%hu", port);
    int iResult = ::getaddrinfo(host.c_str(), serverPort.c_str(), &addrHintsIn, &pAddrOut);
    if (iResult != 0) {
        g_theConsole->PrintError("failed to call getaddrinfo in TCPClient Connect");
        return;
    }

    SOCKET& soc = m_socket.m_socket;
    soc = ::socket(pAddrOut->ai_family, pAddrOut->ai_socktype, pAddrOut->ai_protocol);
    if (soc == INVALID_SOCKET) {
        g_theConsole->PrintError("TCPClient Connect creat socket failed");
        freeaddrinfo(pAddrOut);
        return;
    }

    iResult = ::connect(soc, pAddrOut->ai_addr, (int)pAddrOut->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(soc);
        soc=INVALID_SOCKET;
        return;
    }
    freeaddrinfo(pAddrOut);

    if (soc == INVALID_SOCKET) {
        g_theConsole->PrintError("TCPClient Connect failed");
        return;
    }

    if (m_mode == eNetworkMode::NET_NON_BLOCKING) {
        u_long winSockMode=1;
        iResult = ioctlsocket(soc, FIONBIO, &winSockMode);
        if (iResult == SOCKET_ERROR) {
            g_theConsole->PrintError("ioctlsocket failed on client socket connect");
            closesocket(soc);
            return;
        }
    }
    else {
        g_theConsole->PrintError("Blocking mode could be buggy");
    }

    m_socket.m_mode = m_mode;
}

//////////////////////////////////////////////////////////////////////////
void TCPClient::Disconnect()
{
    std::string msg = MakeClientQuitPackage();
    m_socket.Send(msg.data(), msg.size());
    m_socket.Shutdown();
    m_socket.Close();
}

//////////////////////////////////////////////////////////////////////////
void TCPClient::Shutdown()
{
    std::string msg = MakeClientQuitPackage();
    m_socket.Send(msg.data(), msg.size());
    m_socket.Shutdown();
}

//////////////////////////////////////////////////////////////////////////
void TCPClient::SendTCPMessage(std::string const& str)
{
    if (m_socket.IsValid()) {
        m_socket.Send(str.data(), str.size());
    }
}

//////////////////////////////////////////////////////////////////////////
bool TCPClient::IsValid() const
{
    return !m_socket.IsClosed();
}

//////////////////////////////////////////////////////////////////////////
TCPClient::TCPClient(eNetworkMode mode)
    : m_mode(mode)
    , m_socket(INVALID_SOCKET)
{

}
