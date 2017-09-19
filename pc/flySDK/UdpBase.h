#pragma once

class UdpBase
{
public:
	UdpBase();
	virtual ~UdpBase();

public:
	// ����
	void Init();
	// �ͷ�
	void Free();

	// �󶨱��ض˿�
	bool Bind(int port);
	// ��ȡ���ض˿�
	bool Host(char *server, int &port);

	// ��������
	bool Send(char *server, int port, char *data, int nSize);
	// ��������(������)
	bool Recv(char *server, int &port, char *data, int &nSize, int sec, int usec);
	// ��������(������ʽ)
	bool Receive(char *server, int &port, char *data, int &nSize);

public:
	// ���ӱ��
	bool m_bInit;
	// ���Ӿ��
	SOCKET m_socket;
	// ������
	CRITICAL_SECTION m_csLock;
};

