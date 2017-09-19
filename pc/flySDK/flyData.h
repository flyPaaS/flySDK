#pragma once

class flySession;
class flyData
{
public:
	flyData();
	virtual ~flyData();

public:
	// ����
	void Init();
	// �ͷ�
	void Free();

	// �����߳�
	DWORD static WINAPI RecvThread(LPVOID lpParam);

public:
	// ��Ӧ��SessionId;
	DWORD dwSessionId;
	// �ϲ����
	flySession *pSession;
	// �̱߳��
	HANDLE m_hRecvEvent;
	HANDLE m_hRecvThread;
	
	// ����ʹ�õĶ˿���
	int nUse;
	// ����Э��
	int nProtocol;
	// udp����
	UdpBase udp[8];
	// 2�������
	char szIp1[32];
	char szIp2[32];
	// 8���˿�
	int nPort[8];
};

