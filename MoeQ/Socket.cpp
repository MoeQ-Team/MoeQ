#include "pch.h"
#include "Socket.h"

Socket::Socket()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) throw "Init error";
	Client = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (Client == INVALID_SOCKET)
	{
		WSACleanup();
		throw "create socket error";
	}
}

bool Socket::Connect(const char* IP, const unsigned short Port) {
	if (!IsConnected())
	{
		closesocket(Client);
		Client = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (Client == INVALID_SOCKET)
		{
			WSACleanup();
			throw "create socket error";
		}
	}
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(Port);
	InetPtonA(AF_INET, IP, &addrServer.sin_addr.s_addr);

	if (connect(Client, (const sockaddr*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR)
	{
		closesocket(Client);
		WSACleanup();
		return false;
	}
	return true;
}

bool Socket::IsConnected()
{
	return true;
}

void Socket::Close()
{
	closesocket(Client);
	WSACleanup();
}

void Socket::Send(const LPBYTE data)
{
	send(Client, (const char*)data, XBin::Bin2Int(data), 0);
}

LPBYTE Socket::Receive()
{
	byte len[4];
	LPBYTE buffer;
	if (recv(Client, (char*)len, 4, 0) != 4)
	{
		throw "read len failure";
		return nullptr;
	}
	int length = XBin::Bin2Int(len);
	if (length <= 4)
	{
		throw "len error";
		return nullptr;
	}
	buffer = new byte[length];
	memcpy(buffer, len, 4);
	uint reclen = 0;
	while (reclen < length - 4)
	{
		reclen += recv(Client, (char*)buffer + reclen + 4, length - reclen - 4, 0);
	}
	return buffer;
}

//域名取IP(域名, IP 指针回传)
void Socket::DomainGetIP(const wchar_t* Domain, wchar_t*& szHostaddress) {
	ADDRINFOW hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	DWORD dwRetval;
	ADDRINFOW* result = NULL;
	ADDRINFOW* ptr = NULL;
	dwRetval = GetAddrInfoW(Domain, L"", &hints, &result);
	if (dwRetval != 0) throw "Get ERROR";
	szHostaddress = new wchar_t[46];
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		switch (ptr->ai_family) {
		case AF_INET:
			DWORD ipbufferlength = 46;
			INT iRetval = WSAAddressToString((LPSOCKADDR)ptr->ai_addr, (DWORD)ptr->ai_addrlen, NULL, szHostaddress, &ipbufferlength);
			if (iRetval)
				throw "WSAAddressToString failed";
			else
				return;
			break;
		}
	}
}