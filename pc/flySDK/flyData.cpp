#include "stdafx.h"
#include "flyData.h"
#include "flySession.h"

flyData::flyData()
{
	dwSessionId = 0;
	pSession = NULL;
	m_hRecvEvent = NULL;
	m_hRecvThread = NULL;

	nUse = 0;
	nProtocol = 0;
	memset(szIp1, 0, sizeof(char) * 32);
	memset(szIp2, 0, sizeof(char) * 32);
	for (int i = 0; i < 8; i++)
	{
		nPort[i] = 0;
	}
}

flyData::~flyData()
{

}

void flyData::Init()
{
	if (nProtocol == PROTOCOL_UDP)
	{
		// 启动UDP对象
		for (int i = 0; i < 8; i++)
		{
			udp[i].Init();
		}
		// 启动接收线程
		m_hRecvEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hRecvThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, this, 0, NULL);
	}
}

void flyData::Free()
{
	if (nProtocol == PROTOCOL_UDP)
	{
		// 关闭接收线程
		SetEvent(m_hRecvEvent);
		// 等待接收线程退出
		if (m_hRecvThread != NULL)
		{
			DWORD dwStart = GetTickCount();
			while (1)
			{
				int nRet = ::WaitForSingleObject(m_hRecvThread, 1000);
				if (nRet == WAIT_OBJECT_0)
				{
					break;
				}

				if (GetTickCount() - dwStart > 3000)
				{
					::TerminateThread(m_hRecvThread, 0);
					break;
				}
			}
			::CloseHandle(m_hRecvThread);
			m_hRecvThread = NULL;
		}
		if (m_hRecvEvent != NULL)
		{
			::CloseHandle(m_hRecvEvent);
			m_hRecvEvent = NULL;
		}
		// 释放UDP对象
		for (int i = 0; i < 8; i++)
		{
			udp[i].Free();
		}
	}
}

DWORD WINAPI flyData::RecvThread(LPVOID lpParam)
{
	flyData *pData = (flyData *)lpParam;
	while (1)
	{
		char szIP[256] = { 0 };
		int nPort = 0;
		char szRead[2048] = { 0 };
		int nSize = 2048;

		fd_set readfds;
		struct timeval tv;
		FD_ZERO(&readfds);
		for (int i = 0; i < 8; i++)
		{
			FD_SET(pData->udp[i].m_socket, &readfds);
		}
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		int nRes = select(0, &readfds, NULL, NULL, &tv);
		if (nRes > 0)
		{
			for (int i = 0; i < 8; i++)
			{
				if (FD_ISSET(pData->udp[i].m_socket, &readfds))
				{
					if (pData->udp[i].Receive(szIP, nPort, szRead, nSize))
					{
						if (nSize == 8)
						{
							//{ 0xFF, 0x55, 0xFF, 0x55, 0xFF, 0x77, 0xFF, 0x77 };
							if ((BYTE)szRead[0] == 0xFF && (BYTE)szRead[1] == 0x55 &&
								(BYTE)szRead[2] == 0xFF && (BYTE)szRead[3] == 0x55 &&
								(BYTE)szRead[4] == 0xFF && (BYTE)szRead[5] == 0x77 &&
								(BYTE)szRead[6] == 0xFF && (BYTE)szRead[7] == 0x77)
							{
								// 探测包,不需要回调
								OutputDebugStringA("Receive NatPack\r\n");
								continue;
							}
						}

						if (pData->pSession != NULL)
						{
							int nCount = pData->pSession->m_vtCallBack.size();
							for (int i = 0; i < nCount; i++)
							{
								pData->pSession->m_vtCallBack[i]->recv_cb((void *)(pData->dwSessionId), szRead, nSize, i);
							}
						}
					}
				}
			}
		}

		if (::WaitForSingleObject(pData->m_hRecvEvent, 100) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}