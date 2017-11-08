#pragma once

#ifdef DLLProvider
#define DLL_EXPORT_IMPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT_IMPORT extern "C" __declspec(dllimport)
#endif

// Э������
#define PROTOCOL_UDP	0
#define PROTOCOL_TCP	1
//#define PROTOCOL_RTP	2
//#define PROTOCOL_QUIC	3

// �ص����¼�
#define EVT_REGISTER_SUCCUSS		1000		// SDKע��ɹ�
#define EVT_REGISTER_FAILURE		1001		// SDKע��ʧ��
#define EVT_LOGINOUT_SUCCUSS		1002		// SDKע���ɹ�
#define EVT_SESSION_BIND_SUCCUSS	1003		// Session�󶨳ɹ�
#define EVT_SESSION_BIND_FAILURE	1004		// Session��ʧ��
#define EVT_SESSION_LISTEN_SUCCUSS	1005		// Session�����ɹ�
#define EVT_SESSION_LISTEN_FAILURE	1006		// Session����ʧ��
#define EVT_SESSION_RELEASE_SUCCUSS	1007		// Session�ͷųɹ�
#define EVT_SESSION_RELEASE_FAILURE	1008		// Session�ͷ�ʧ��
#define EVT_SESSION_INCOMING		1009		// �Ự��������
#define EVT_SESSION_ACCEPT			1010		// �Ự����ȷ��
#define EVT_SESSION_REJECT			1011		// �Ự���Ӿܾ�

// �¼��ص�
typedef void(*CallBackEvent)(int nEvent, int nCode, void *pData);
// �������ݻص�
typedef void(*CallBackRecv)(void* pHandle, void *pData, int nLen, int nChannalId);

// �ص���������
typedef struct
{
	CallBackEvent event_cb;
	CallBackRecv recv_cb;
} CallBackCB,*CallBackFunc;


// ��ʼ��
DLL_EXPORT_IMPORT void FlyCanInit(char *sid, char *token, char *appid, char *sdkid);

// ����ʼ��
DLL_EXPORT_IMPORT void FlyCanUnInit();

// ���ûص�
DLL_EXPORT_IMPORT void FlyCanAddCallBack(CallBackFunc cb);

// ɾ���ص�
DLL_EXPORT_IMPORT void FlyCanRemoveCallBack(CallBackFunc cb);

// �˺ŵ�¼�����ͨ���ص��첽����
DLL_EXPORT_IMPORT void FlyCanRegister();

// �˺�ע��
DLL_EXPORT_IMPORT void FlyCanUnRegister();

// ����һ��ָ��Э��Ự,���ػỰ���
DLL_EXPORT_IMPORT void* FlyCanCreateSession(int protocol);

// �ͷ�ָ��Э��Ự
DLL_EXPORT_IMPORT void FlyCanReleaseSession(void *handle);

// ����һ���Ự
DLL_EXPORT_IMPORT void FlyCanListenSession(void *handle, int nNum);

// ����flyCan��Ҫ���ӵ�Ŀ�ĵ�id(���Ʋ���绰)
DLL_EXPORT_IMPORT bool FlyCanConnectSession(void *handle, char *peersdkid);

// Ŀ�ĵ��յ��������󣬽�������(�����������)
DLL_EXPORT_IMPORT bool FlyCanAcceptSession(void *handle);

// Ŀ�ĵ��յ��������󣬾ܾ�����(��������ܽ�)
DLL_EXPORT_IMPORT bool FlyCanRejectSession(void *handle);

// ��������(��������ͨ���ص�����)
DLL_EXPORT_IMPORT bool FlyCanSend(void *handle, void *pData, int nLen, int nChannalId);

// ֱ�����Ӷ�Ӧ�ĵ�ַ
DLL_EXPORT_IMPORT int FlyCanConnectServer(void *handle, char *ip, int port);




