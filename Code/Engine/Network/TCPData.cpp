#include "Engine/Network/TCPData.hpp"
#include "Engine/Network/NetworkCommon.hpp"


//////////////////////////////////////////////////////////////////////////
TCPData::TCPData(size_t length, char* dataPtr)
    : m_length(length)
    , m_dataStr(dataPtr, length)
{
}

//////////////////////////////////////////////////////////////////////////
TCPData::~TCPData()
{
    m_length = 0;
}

//////////////////////////////////////////////////////////////////////////
std::string TCPData::GetData() const
{
    NetworkPackageHeader const* header = reinterpret_cast<NetworkPackageHeader const*>(&m_dataStr[0]);
    return std::string(&m_dataStr[0], NET_HEADER_LEN+header->m_size);
}

//////////////////////////////////////////////////////////////////////////
std::string TCPData::GetContent() const
{
    if (m_dataStr.empty()) {
        return m_dataStr;
    }

    return m_dataStr.substr(NET_HEADER_LEN);
}

//////////////////////////////////////////////////////////////////////////
eNetworkPackageHeaderType TCPData::GetHeader() const
{
    if (m_length < sizeof(NetworkPackageHeader)) {
        return eNetworkPackageHeaderType::HEAD_INVALID;
    }

    NetworkPackageHeader const* headerPtr = reinterpret_cast<NetworkPackageHeader const*>(m_dataStr.data());
    return static_cast<eNetworkPackageHeaderType>(headerPtr->m_type);
}
