#pragma once

#include "Engine/Network/NetworkCommon.hpp"
#include "Engine/Network/BlockingQueue.hpp"
#include "Engine/Network/ParallelQueue.hpp"
#include <string>
#include <array>

class UDPSocket
{
public:
    using Buffer = std::array<char, NET_DEFAULT_BUFLEN+1>;

    UDPSocket(std::string const& host, int port);
    ~UDPSocket();

    void BeginFrame();
    void Bind(int port);
    void SendUDPMessage(std::string const& msg);
    int Send(int length);   //::sendto
    int Receive();          //::recvfrom
    void Close();

    void UDPWriterMain();
    void UDPReaderMain();

    Buffer& GetSendBuffer() {return m_sendBuffer;}
    Buffer& GetReceiveBuffer() {return m_receiveBuffer;}
    bool IsValid() const {return m_socket!=INVALID_SOCKET;}
    int GetBindPort() const {return m_bindPort;}

private:
    Buffer m_sendBuffer;
    Buffer m_receiveBuffer;
    sockaddr_in m_toAddr;
    sockaddr_in m_bindAddr;
    SOCKET m_socket;
    int m_bindPort = -1;
    int m_toPort = -1;

    BlockingQueue<Buffer> m_writeQueue;
    ParallelQueue<Buffer> m_readQueue;

    std::thread m_writeThread;
    std::thread m_readThread;
};