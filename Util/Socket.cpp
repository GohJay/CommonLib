#include <ws2tcpip.h>
#include "Socket.h"
#pragma comment(lib, "Ws2_32.lib")

bool Jay::Startup()
{
    WSADATA ws;
    return WSAStartup(MAKEWORD(2, 2), &ws) == 0;
}
bool Jay::Cleanup()
{
    return WSACleanup() == 0;
}
int Jay::DomainToIP(const wchar_t* domain, IN_ADDR* address)
{
    ADDRINFOW* addrInfo;
    if (GetAddrInfo(domain, L"0", NULL, &addrInfo) != 0)
        return 0;

    ADDRINFOW* cur = addrInfo;
    SOCKADDR_IN* sockAddr;
    int count;
    for (count = 0; count < MAX_ADDR && cur; count++)
    {
        sockAddr = (SOCKADDR_IN*)cur->ai_addr;
        address[count] = sockAddr->sin_addr;
        cur = addrInfo->ai_next;
    }

    FreeAddrInfo(addrInfo);
    return count;
}
bool Jay::IPToString(IN_ADDR* address, wchar_t* stringBuf, size_t stringBufSize)
{
    return InetNtop(AF_INET, address, stringBuf, stringBufSize) != NULL;
}
