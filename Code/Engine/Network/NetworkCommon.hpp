#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstdint>
#include <string>
#include <vector>

class NamedProperties;

enum eNetworkMode
{
    NET_INVALID = 0,
    NET_BLOCKING = 1,
    NET_NON_BLOCKING = 2
};

struct NetworkPackageHeader
{
    uint16_t m_type=0;
    uint16_t m_size=0;
    uint16_t m_seqNo=0;
    int m_key=0xffffffff;
    bool m_reliable = false;
};

enum eNetworkPackageHeaderType : uint16_t
{
    HEAD_INVALID = 0,
    HEAD_SERVER_LISTEN,
    HEAD_CLIENT_START,
    HEAD_TEXT,
    HEAD_CLIENT_CLOSE,
};

constexpr int NET_DEFAULT_BUFLEN = 512;
constexpr int NET_HEADER_LEN = (int)sizeof(NetworkPackageHeader);
constexpr int NET_MAX_DATA_LEN = NET_DEFAULT_BUFLEN-NET_HEADER_LEN;

std::string MakeClientQuitPackage();
std::string MakeClientStartPackageHeader();
std::string MakeClientConnectPackageHeader();
std::string MakeClientConnectPackage(std::string const& clientIP);
std::vector<std::string> MakeTextPackages(std::string const& srcMsg, bool reliable = false);
std::string MakeTextPackage(std::string const& srcMsg, bool reliable = false);

std::string GetPackageHeader(std::string const& msg, uint16_t& size);

void SetupNetworkEventParameter(std::string const& data, void* ptr, NamedProperties& parameters);