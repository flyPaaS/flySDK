#include "stdafx.h"
#include "UdpBase.h"

UdpBase::UdpBase()
{
	m_bInit = false;
	m_socket = INVALID_SOCKET;
	InitializeCriticalSection(&m_csLock);
}

UdpBase::~UdpBase()
{
	DeleteCriticalSection(&m_csLock);
}

void UdpBase::Init()
{
	// 先释放
	Free();
	// 再加载
	EnterCriticalSection(&m_csLock);
	WSADATA wsaData = { 0 };
	WORD wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		LeaveCriticalSection(&m_csLock);
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 2 && HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		LeaveCriticalSection(&m_csLock);
		return;
	}

	m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		WSACleanup();
		LeaveCriticalSection(&m_csLock);
		return;
	}
	m_bInit = true;
	LeaveCriticalSection(&m_csLock);
}

void UdpBase::Free()
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		WSACleanup();
		m_bInit = false;
	}
	LeaveCriticalSection(&m_csLock);
}

bool UdpBase::Bind(int port)
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(port);
		servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		if (bind(m_socket, (sockaddr*)&servAddr, sizeof(sockaddr)) == 0)
		{
			LeaveCriticalSection(&m_csLock);
			return true;
		}
	}
	LeaveCriticalSection(&m_csLock);
	return false;
}

bool UdpBase::Host(char *server, int &port)
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		int nLen = sizeof(sockaddr);
		if (getsockname(m_socket, (sockaddr *)&servAddr, &nLen) == 0)
		{
			// 转换
			port = ntohs(servAddr.sin_port);
			strcpy(server, inet_ntoa(servAddr.sin_addr));
			LeaveCriticalSection(&m_csLock);
			return true;
		}
	}
	LeaveCriticalSection(&m_csLock);
	return false;
}

bool UdpBase::Send(char *server, int port, char *data, int nSize)
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(port);
		servAddr.sin_addr.S_un.S_addr = inet_addr(server);
		int nSend = ::sendto(m_socket, data, nSize, 0, (sockaddr *)&servAddr, sizeof(sockaddr));
		if (nSend == nSize)
		{
			LeaveCriticalSection(&m_csLock);
			return true;
		}
	}
	LeaveCriticalSection(&m_csLock);
	return false;
}

bool UdpBase::Recv(char *server, int &port, char *data, int &nSize, int sec, int usec)
{
	if (m_bInit)
	{
		fd_set readfds;
		struct timeval tv;
		FD_ZERO(&readfds);
		FD_SET(m_socket, &readfds);
		tv.tv_sec = sec;
		tv.tv_usec = usec;
		int nRes = select(m_socket + 1, &readfds, NULL, NULL, &tv);
		if (nRes <= 0)
		{
			// 失败或者超时
			return false;
		}

		if (FD_ISSET(m_socket, &readfds))
		{
			sockaddr_in servAddr;
			servAddr.sin_family = AF_INET;
			int nLen = sizeof(sockaddr);
			int nRecv = ::recvfrom(m_socket, data, nSize, 0, (sockaddr *)&servAddr, &nLen);
			if (nRecv > 0)
			{
				nSize = nRecv;
				data[nRecv] = '\0';
				port = ntohs(servAddr.sin_port);
				strcpy(server, inet_ntoa(servAddr.sin_addr));
				return true;
			}
		}
	}
	return false;
}

bool UdpBase::Receive(char *server, int &port, char *data, int &nSize)
{
	if (m_bInit)
	{
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		int nLen = sizeof(sockaddr);
		int nRecv = ::recvfrom(m_socket, data, nSize, 0, (sockaddr *)&servAddr, &nLen);
		if (nRecv > 0)
		{
			nSize = nRecv;
			data[nRecv] = '\0';
			port = ntohs(servAddr.sin_port);
			strcpy(server, inet_ntoa(servAddr.sin_addr));
			return true;
		}
	}
	return false;
}