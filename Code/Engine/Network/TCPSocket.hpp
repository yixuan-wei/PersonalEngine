#pragma once

#include "Engine/Network/NetworkCommon.hpp"
#include <string>

class TCPData;

class TCPSocket
{
    friend class TCPServer;
    friend class TCPClient;

public:
    static std::string GetIPAddress(std::string const& fullAddress);

    TCPSocket();
    TCPSocket( SOCKET socket, eNetworkMode mode=eNetworkMode::NET_BLOCKING, size_t bufSize=NET_DEFAULT_BUFLEN );
    TCPSocket(TCPSocket const& src);
    ~TCPSocket();

    void operator=(TCPSocket const& src);

    void SetMode(eNetworkMode newMode);

    size_t GetBufferSize() const {return m_bufferSize;}
    eNetworkMode GetMode() const {return m_mode;}
    bool IsClosed() const {return m_socket==INVALID_SOCKET;}
    bool IsQuiting() const {return m_isQuiting;}
    bool IsValid() const; 
    char* GetRemoteFullAddress() const;
    char* GetLocalFullAddress() const;

    bool IsDataAvailable();

    void Send(char const* dataPtr, size_t length);
    TCPData Receive();
    void Shutdown();
    void Close();

private:
    bool m_isQuiting = false;
    eNetworkMode m_mode;
    size_t m_bufferSize = 0;
    char* m_bufferPtr = nullptr;
    SOCKET m_socket;
    FD_SET m_fdSet;
    timeval m_timeVal;
};