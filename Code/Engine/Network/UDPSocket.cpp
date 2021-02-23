#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Game/EngineBuildPreferences.hpp"

#include <iostream>
#include <thread>

#ifdef TEST_MODE
    #define LOG_ERROR(...) printf((Stringf(__VA_ARGS__)+std::string("\n")).c_str())
#else
    #define LOG_ERROR(...) g_theConsole->PrintError(Stringf(__VA_ARGS__))
#endif

//////////////////////////////////////////////////////////////////////////
UDPSocket::UDPSocket(std::string const& host, int port)
    : m_socket(INVALID_SOCKET)
{
    m_toPort = port;
    m_toAddr.sin_family = AF_INET;
    m_toAddr.sin_port = htons((u_short)port);
    inet_pton(AF_INET,host.c_str(), &m_toAddr.sin_addr.s_addr);

    m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) {
        LOG_ERROR("Socket instantiation of UDPSocket failed, error=%i",WSAGetLastError());
    }
}

//////////////////////////////////////////////////////////////////////////
UDPSocket::~UDPSocket()
{
    try {
        m_readThread.join();
        m_writeQueue.Push(Buffer());
        m_writeThread.join();
    }
    catch (std::system_error& e) {
        g_theConsole->PrintError(Stringf("join udp thread failed %i: %s", e.code().value(), e.what()));
    }
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::BeginFrame()
{
    Buffer buf;
    uint16_t size = 0;
    while (true) {
        buf = m_readQueue.Pop();
        if (buf.empty()) {
            break;
        }

        NetworkPackageHeader const* pMsg = reinterpret_cast<NetworkPackageHeader const*>(&buf[0]);
        size = pMsg->m_size;
        if (size == 0) {
            break;
        }

        buf[NET_DEFAULT_BUFLEN]='\0';
        std::string fullText(&buf[0], NET_DEFAULT_BUFLEN);
        NamedProperties parameters;
        SetupNetworkEventParameter(fullText, (void*)this, parameters);
        g_theEvents->FireEvent("UDPSocketReceive", parameters, EVENT_NETWORK);
    }
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::Bind(int port)
{
    m_bindPort = port;

    m_bindAddr.sin_family = AF_INET;
    m_bindAddr.sin_port = htons((u_short)port);
    m_bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int result = ::bind(m_socket, (SOCKADDR*)&m_bindAddr, sizeof(m_bindAddr));
    if (result != 0) {
        LOG_ERROR("UDPSocket bind failed, error=%i", WSAGetLastError());
    }

    m_writeThread = std::thread(&UDPSocket::UDPWriterMain, this);
    m_readThread = std::thread(&UDPSocket::UDPReaderMain, this);
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::SendUDPMessage(std::string const& msg)
{
    Buffer buf;
    std::memcpy(&buf[0], &msg[0], msg.size());
    m_writeQueue.Push(buf);
}

//////////////////////////////////////////////////////////////////////////
int UDPSocket::Send(int length)
{
    if (!IsValid()) {
        return -1;
    }

    int result = ::sendto(m_socket, &m_sendBuffer[0], length, 0,
        reinterpret_cast<SOCKADDR*>(&m_toAddr), sizeof(m_toAddr));
    if (result == SOCKET_ERROR) {
        LOG_ERROR("UDPSocket sent failed, error=%i", WSAGetLastError());
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
int UDPSocket::Receive()
{
    if (!IsValid()) {
        return -1;
    }

    sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);
    int result = ::recvfrom(m_socket, &m_receiveBuffer[0], (int)m_receiveBuffer.size(), 0,
        reinterpret_cast<SOCKADDR*>(&fromAddr), &fromLen);
    if (result == SOCKET_ERROR || result < 0) {
        int error = WSAGetLastError();
        if (error == 10004) {   //interrupted call
            g_theConsole->PrintString(Rgba8::YELLOW, "UDP receive interrupted, quitting...");
        }
        else{
            g_theConsole->PrintString(Rgba8::RED,Stringf("%i",WSAGetLastError()));
        }
        NamedProperties parameters;
        SetupNetworkEventParameter("", (void*)this, parameters);
        g_theEvents->FireEvent("UDPReceiveFail", parameters, EVENT_NETWORK);
        return -1;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::Close()
{
    if (m_socket != INVALID_SOCKET) {
        int result = closesocket(m_socket);
        if (result == SOCKET_ERROR) {
            LOG_ERROR("UDPSocket close failed, error=%i", WSAGetLastError());
        }
        m_socket = INVALID_SOCKET;        
    } 
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::UDPWriterMain()
{
    while (IsValid()) {
        Buffer buf = m_writeQueue.Pop();
        if (!IsValid()) {
            return;
        }

        NetworkPackageHeader* header = reinterpret_cast<NetworkPackageHeader*>(&buf[0]);
        size_t totalLen = header->m_size + NET_HEADER_LEN;
        std::memcpy(&m_sendBuffer, &buf[0], totalLen);
        m_sendBuffer[totalLen] = NULL;
        Send((int)totalLen + 1);
    }
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::UDPReaderMain()
{
    std::string dataStr;
    size_t length = 0;
    while (IsValid()) {
        if (!IsValid()) {
            return;
        }
        length = Receive();
        dataStr.clear();
        if (length > 0 && IsValid() && length != INVALID_SOCKET) {
            auto& buf = GetReceiveBuffer();
            m_readQueue.Push(buf);
        }
        else {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }
}
