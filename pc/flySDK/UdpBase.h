#pragma once

class UdpBase
{
public:
	UdpBase();
	virtual ~UdpBase();

public:
	// 加载
	void Init();
	// 释放
	void Free();

	// 绑定本地端口
	bool Bind(int port);
	// 获取本地端口
	bool Host(char *server, int &port);

	// 发送数据
	bool Send(char *server, int port, char *data, int nSize);
	// 接收数据(非阻塞)
	bool Recv(char *server, int &port, char *data, int &nSize, int sec, int usec);
	// 接收数据(阻塞方式)
	bool Receive(char *server, int &port, char *data, int &nSize);

public:
	// 连接标记
	bool m_bInit;
	// 连接句柄
	SOCKET m_socket;
	// 操作锁
	CRITICAL_SECTION m_csLock;
};

