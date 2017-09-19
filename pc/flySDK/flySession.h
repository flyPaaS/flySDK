#pragma once
#include "flySDK.h"
#include "flyData.h"

class flySession
{
public:
	flySession();
	virtual ~flySession();

public:
	// �����˺�
	void SetParam(char *sid, char *token, char *appid, char *sdkid);
	// ���ûص�
	void AddCallBack(CallBackFunc cb);
	// �Ƴ��ص�
	void RemoveCallBack(CallBackFunc cb);

	// ����SDK��SManageͨ��
	void Register();
	// ֹͣSDK��SManageͨ��
	void UnRegister();

	// �����Ự
	void* CreateSession(int protocol);
	// �ͷŻỰ
	void ReleaseSession(void *handle);
	// �����Ự
	void ListenSession(void *handle, int num);

	// �ͷŻỰ��Ӧ
	void ReleaseSessionRsp(void *handle, BYTE sn, DWORD time);

	// ����flyCan��Ҫ���ӵ�Ŀ�ĵص�ַ(���Ʋ���绰)
	bool ConnectSession(void *handle, char *peersdkid);
	// Ŀ�ĵ��յ��������󣬽�������(�����������)
	bool AcceptSession(void *handle);
	// Ŀ�ĵ��յ��������󣬾ܾ�����(��������ܽ�)
	bool RejectSession(void *handle);

	// ��������(��������ͨ���ص�����)
	bool Send(void *handle, void *data, int len, int nChannalId);

private:
	// ��¼SManage
	bool SendLogin();
	// ����SManage
	bool SendHeat();
	// ��¼SManage
	bool SendLoginOut();

	// ���Ͱ�ָ��
	bool SendBind(DWORD sessionid);
	// �����˳�ָ��
	bool SendRelease(DWORD sessionid);
	// ���ͼ���ָ��
	bool SendListen(DWORD sessionid);

	// ���ͻỰ�ͷ���Ӧ
	bool SendReleaseRsp(DWORD sessionid, BYTE sn, DWORD time);

	// ��������ָ��
	bool SendConnect(DWORD sessionid, char *peersdkid);
	// ���ͽ���ָ��
	bool SendAccept(DWORD sessionid);
	// ���;ܽ�ָ��
	bool SendReject(DWORD sessionid);
	
	// ������յ����ݰ�
	void OnRecvData(unsigned char *pData, int nSize);
	
	// SDK��SManageͨѶ �����߳�
	DWORD static WINAPI WorkThread(LPVOID lpParam);
	// SDK��SManageͨѶ �����߳�
	DWORD static WINAPI RecvThread(LPVOID lpParam);

	// ��ȡʱ���
	CStringA GetTimeStr();

	// ��ȡ���ع���IP
	void GetLocalIp();
	// ��ȡAP�б��RTT
	void GetAPDelay();
	// ���ʹ�͸��
	void SendNatPack(flyData *pData);

public:
	// �ص�����
	vector<CallBackFunc> m_vtCallBack;
	// ���ݶ����ϣ
	map<DWORD, flyData*> m_mpSession;
	// �ͻ��˶����ϣ
	map<DWORD, CLIENT*> m_mpClient;

private:
	// SessionId(����)
	DWORD m_dwSessionId;

	// У��������ر���
	CStringA m_strSid;
	CStringA m_strToken;
	CStringA m_strAppId;
	CStringA m_strSdkId;

	// SN
	BYTE m_nSn;
	// ����״̬
	int m_nStatus;
	// ���ջ�Ӧ
	HANDLE m_hEvent[2];
	// �̱߳��
	HANDLE m_hWorkEvent;
	HANDLE m_hRecvEvent;
	HANDLE m_hWorkThread;
	HANDLE m_hRecvThread;
	// UDP���Ӷ���
	UdpBase m_cClient;

	// ��������
	bool m_bPing;
	// ���ص�AP�б�
	vector<IPORT*> m_vtAPList;

	// ��������IP����
	bool m_bLocalIp;
	// ���汾��IP
	CStringA m_strLocalIP;
};

