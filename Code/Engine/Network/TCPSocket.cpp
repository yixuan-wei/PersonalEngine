#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Network/TCPData.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

//////////////////////////////////////////////////////////////////////////
std::string TCPSocket::GetIPAddress(std::string const& fullAddress)
{
    Strings trunks = SplitStringOnDelimiter(fullAddress,':');
    return trunks[0];
}

//////////////////////////////////////////////////////////////////////////
TCPSocket::TCPSocket()
    : m_mode(eNetworkMode::NET_BLOCKING)
    , m_bufferSize(NET_DEFAULT_BUFLEN)
    , m_socket(INVALID_SOCKET)
    , m_timeVal{0l, 0l}
{
    FD_ZERO(&m_fdSet);
    m_bufferPtr = new char[m_bufferSize+1];
}

//////////////////////////////////////////////////////////////////////////
TCPSocket::TCPSocket(SOCKET socket, eNetworkMode mode, size_t bufSize/*=256 */)
    : m_mode(mode)
    , m_bufferSize(bufSize)
    , m_socket(socket)
    , m_timeVal{0l, 0l}
{
    FD_ZERO(&m_fdSet);
    GUARANTEE_OR_DIE(bufSize>0, "TCPSocket inited with bufSize set to 0");
    m_bufferPtr = new char[m_bufferSize+1];
}

//////////////////////////////////////////////////////////////////////////
TCPSocket::TCPSocket(TCPSocket const& src)
    : m_mode(src.m_mode)
    , m_bufferSize(src.m_bufferSize)
    , m_socket(src.m_socket)
    , m_fdSet(src.m_fdSet)
    , m_timeVal(src.m_timeVal)
{
    GUARANTEE_OR_DIE(m_bufferSize > 0, "TCPSocket inited with bufSize set to 0");
    m_bufferPtr = new char[m_bufferSize+1];
}

//////////////////////////////////////////////////////////////////////////
TCPSocket::~TCPSocket()
{
    m_mode = eNetworkMode::NET_INVALID;
    m_bufferSize = 0;
    FD_ZERO(&m_fdSet);
    if (IsValid()) {
        Close();
    }
    delete[] m_bufferPtr;
}

//////////////////////////////////////////////////////////////////////////
void TCPSocket::SetMode(eNetworkMode newMode)
{
    m_mode = newMode;
    if (m_mode != eNetworkMode::NET_INVALID) {
        u_long winSockMode = 0;
        switch (m_mode)
        {
        case NET_BLOCKING:  winSockMode=0; break;
        case NET_NON_BLOCKING:  winSockMode=1; break;
        }
        int iResult = ioctlsocket(m_socket, FIONBIO, &winSockMode);
        if (iResult == SOCKET_ERROR) {
            g_theConsole->PrintError("TCPSocket SetMode ioctlsocket call failed");
        }
    }
}

//////////////////////////////////////////////////////////////////////////
bool TCPSocket::IsValid() const
{
    return m_socket != INVALID_SOCKET && !m_isQuiting;
}

//////////////////////////////////////////////////////////////////////////
char* TCPSocket::GetRemoteFullAddress() const
{
    char* result = new char[64];
    wchar_t* addressStr = new wchar_t[64];
    DWORD outLen = 64;

    sockaddr clientAddr;
    int addrSize = sizeof(clientAddr);
    int iResult = getpeername(m_socket, &clientAddr, &addrSize);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError("Getpeername on socket failed");
        return nullptr;
    }

    iResult = WSAAddressToStringW(&clientAddr, addrSize, NULL, &addressStr[0], &outLen);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError("WSAAddressToStringA on socket failed");
        return nullptr;
    }
    addressStr[outLen]=NULL;
    //TODO move wide c to mb to stringutils
    size_t length;
    wcstombs_s(&length, result, 64, addressStr, 64);
    delete[] addressStr;
    return result;
}

//////////////////////////////////////////////////////////////////////////
char* TCPSocket::GetLocalFullAddress() const
{
    char* result = new char[64];
    wchar_t* addressStr = new wchar_t[64];
    DWORD outLen = 64;

    sockaddr localAddr;
    int addrSize = sizeof(localAddr);
    int iResult = getsockname(m_socket, &localAddr, &addrSize);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError("Getpeername on socket failed");
        return nullptr;
    }

    iResult = WSAAddressToStringW(&localAddr, addrSize, NULL, &addressStr[0], &outLen);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError("WSAAddressToStringA on socket failed");
        return nullptr;
    }
    addressStr[outLen] = NULL;
    //TODO move wide c to mb to stringutils
    size_t length;
    wcstombs_s(&length, result, 64, addressStr, 64);
    delete[] addressStr;
    return result;
}

//////////////////////////////////////////////////////////////////////////
bool TCPSocket::IsDataAvailable()
{
    if (m_mode == eNetworkMode::NET_NON_BLOCKING) {
        FD_ZERO(&m_fdSet);
        FD_SET(m_socket, &m_fdSet);
        int iResult = select(0, &m_fdSet, NULL, NULL, &m_timeVal);
        if (iResult == SOCKET_ERROR) {
            g_theConsole->PrintError(Stringf("fail to select in IsDataAvailable in TCPSocket, error=%i", WSAGetLastError()));
            shutdown(m_socket, SD_BOTH);
            m_isQuiting = true;
        }
        return FD_ISSET(m_socket, &m_fdSet);
    }
    else {
        g_theConsole->PrintString(Rgba8::YELLOW, "IsDataAvailable in TCPSocket only available in non blocking mode");
        return true;
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPSocket::Send(char const* dataPtr, size_t length)
{
    int intLeng = static_cast<int>(length);
    int iResult = ::send(m_socket, dataPtr, intLeng, 0);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError("Send on socket failed");
        m_isQuiting = true;
        shutdown(m_socket, SD_BOTH);
    }
    else if (iResult < intLeng) {
        g_theConsole->PrintString(Rgba8::MAGENTA, Stringf("Requested %i bytes to be sent, only iResult bytes were sent", intLeng, iResult));
        shutdown(m_socket, SD_BOTH);
        m_isQuiting = true;
    }
}

//////////////////////////////////////////////////////////////////////////
TCPData TCPSocket::Receive()
{
    int iResult = ::recv(m_socket, m_bufferPtr, (static_cast<int>(m_bufferSize)), 0);
    if (iResult == SOCKET_ERROR) {
        if(!(WSAGetLastError()==WSAEWOULDBLOCK && m_mode==eNetworkMode::NET_NON_BLOCKING)){
            g_theConsole->PrintError(Stringf("Receive TCP failed, error=%i", WSAGetLastError()));
            m_isQuiting = true;
            shutdown(m_socket, SD_BOTH);
        }
        return TCPData();
    }
    m_bufferPtr[m_bufferSize]='\0';
    return TCPData(static_cast<size_t>(iResult), m_bufferPtr);
}

//////////////////////////////////////////////////////////////////////////
void TCPSocket::Shutdown()
{
    int iResult = ::shutdown(m_socket, SD_BOTH);
    m_isQuiting = true;
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError(Stringf("shutdown tcp socket failed, error=%i", WSAGetLastError()));
        Close();
    }
}

//////////////////////////////////////////////////////////////////////////
void TCPSocket::Close()
{
    m_isQuiting = true;
    int iResult = closesocket(m_socket);
    if (iResult == SOCKET_ERROR) {
        g_theConsole->PrintError(Stringf("Close tcp socket failed, error=%i",WSAGetLastError()));
    }
    m_socket = INVALID_SOCKET;
}

//////////////////////////////////////////////////////////////////////////
void TCPSocket::operator=(TCPSocket const& src)
{
    if (this != &src) {
        m_mode = src.m_mode;
        m_socket = src.m_socket;
        m_bufferSize = src.m_bufferSize;
        m_fdSet = src.m_fdSet;
        m_timeVal = src.m_timeVal;
        GUARANTEE_OR_DIE(m_bufferSize > 0, "TCPSocket inited with bufSize set to 0");
        m_bufferPtr = new char[m_bufferSize+1];
    }
}
