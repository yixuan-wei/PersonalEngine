#include "Engine/Network/NetworkCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/NamedProperties.hpp"

static uint16_t sSequenceNo = 0;

//////////////////////////////////////////////////////////////////////////
std::string MakeClientQuitPackage()
{
    char buffer[NET_HEADER_LEN];
    NetworkPackageHeader* headerPtr = reinterpret_cast<NetworkPackageHeader*>(&buffer[0]);
    headerPtr->m_type = eNetworkPackageHeaderType::HEAD_CLIENT_CLOSE;
    headerPtr->m_size = 0;
    headerPtr->m_seqNo = sSequenceNo++;
    return std::string(buffer, NET_HEADER_LEN);
}

//////////////////////////////////////////////////////////////////////////
std::string MakeClientStartPackageHeader()
{
    char buffer[NET_HEADER_LEN];
    NetworkPackageHeader* headerPtr = reinterpret_cast<NetworkPackageHeader*>(&buffer[0]);
    headerPtr->m_type = eNetworkPackageHeaderType::HEAD_CLIENT_START;
    headerPtr->m_size = 0;
    headerPtr->m_seqNo = sSequenceNo++;
    return std::string(buffer, NET_HEADER_LEN);
}

//////////////////////////////////////////////////////////////////////////
std::string MakeClientConnectPackageHeader()
{
    char buffer[NET_HEADER_LEN];
    NetworkPackageHeader* headerPtr = reinterpret_cast<NetworkPackageHeader*>(&buffer[0]);
    headerPtr->m_type = eNetworkPackageHeaderType::HEAD_SERVER_LISTEN;
    headerPtr->m_size = 0;
    headerPtr->m_seqNo = sSequenceNo++;
    headerPtr->m_reliable = true;
    return std::string(buffer, NET_HEADER_LEN);
}

//////////////////////////////////////////////////////////////////////////
std::string MakeClientConnectPackage(std::string const& clientIP)
{
    UNUSED(clientIP);
    char buffer[NET_HEADER_LEN];
    NetworkPackageHeader* headerPtr = reinterpret_cast<NetworkPackageHeader*>(&buffer[0]);
    headerPtr->m_type = eNetworkPackageHeaderType::HEAD_SERVER_LISTEN;
    headerPtr->m_size = 0;
    headerPtr->m_seqNo = sSequenceNo++;
    return std::string(buffer, NET_HEADER_LEN);
}

//////////////////////////////////////////////////////////////////////////
std::vector<std::string> MakeTextPackages(std::string const& srcMsg, bool reliable)
{
    size_t remainLen = srcMsg.size();
    size_t startPos = 0;
    std::string msg;
    std::vector<std::string> result;
    char buffer[NET_HEADER_LEN];
    NetworkPackageHeader* headerPtr = reinterpret_cast<NetworkPackageHeader*>(&buffer[0]);
    headerPtr->m_reliable = reliable;
    headerPtr->m_type = eNetworkPackageHeaderType::HEAD_TEXT;

    do {
        headerPtr->m_size = (uint16_t)remainLen;
        headerPtr->m_seqNo = sSequenceNo++;
        msg = std::string(buffer, NET_HEADER_LEN);

        if (remainLen > (size_t)NET_MAX_DATA_LEN) {
            msg += srcMsg.substr(startPos, NET_MAX_DATA_LEN);
            startPos += NET_MAX_DATA_LEN;
            remainLen -= NET_MAX_DATA_LEN;
        }
        else {
            msg += srcMsg.substr(startPos);
            startPos += remainLen;
            remainLen = 0;
        }
        msg += '\0';

        result.push_back(msg);
    } while (remainLen > 0);

    return result;
}

//////////////////////////////////////////////////////////////////////////
std::string MakeTextPackage(std::string const& srcMsg, bool reliable)
{
    if (srcMsg.size() > NET_MAX_DATA_LEN) {
        g_theConsole->PrintString(Rgba8::YELLOW, Stringf("MakeTextPackage exceeds max length %i", NET_MAX_DATA_LEN));
    }

    std::string msgToSend = srcMsg.substr(0,NET_MAX_DATA_LEN);

    char buffer[NET_HEADER_LEN];
    NetworkPackageHeader* headerPtr = reinterpret_cast<NetworkPackageHeader*>(&buffer[0]);
    headerPtr->m_type = eNetworkPackageHeaderType::HEAD_TEXT;
    headerPtr->m_size = (uint16_t)msgToSend.size();
    headerPtr->m_seqNo = sSequenceNo++;
    headerPtr->m_reliable = reliable;

    return std::string(buffer,NET_HEADER_LEN)+msgToSend;
}

//////////////////////////////////////////////////////////////////////////
std::string GetPackageHeader(std::string const& msg, uint16_t& size)
{
    std::string content = msg.substr(NET_HEADER_LEN);
    NetworkPackageHeader const* headerPtr = reinterpret_cast<NetworkPackageHeader const*>(&msg[0]);
    size = headerPtr->m_size;
    return content;
}

//////////////////////////////////////////////////////////////////////////
void SetupNetworkEventParameter(std::string const& data, void* ptr, NamedProperties& parameters)
{
    parameters.SetValue("data",data);
    parameters.SetValue("ptr",ptr);
}
