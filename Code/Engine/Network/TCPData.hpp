#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum eNetworkPackageHeaderType : uint16_t;

class TCPData
{
public:
    TCPData()=default;
    TCPData(size_t length, char* dataPtr);
    ~TCPData();

    size_t GetLength() const {return m_length;}
    std::string GetData() const;
    std::string GetContent() const;
    eNetworkPackageHeaderType GetHeader() const;

private:
    size_t m_length = 0;
    std::string m_dataStr;
};