#pragma once

class flySession;
class flyData
{
public:
	flyData();
	virtual ~flyData();

public:
	// 加载
	void Init();
	// 释放
	void Free();

	// 接收线程
	DWORD static WINAPI RecvThread(LPVOID lpParam);

public:
	// 对应的SessionId;
	DWORD dwSessionId;
	// 上层对象
	flySession *pSession;
	// 线程标记
	HANDLE m_hRecvEvent;
	HANDLE m_hRecvThread;
	
	// 可以使用的端口数
	int nUse;
	// 传输协议
	int nProtocol;
	// udp对象
	UdpBase udp[8];
	// 2个接入点
	char szIp1[32];
	char szIp2[32];
	// 8个端口
	int nPort[8];
};

