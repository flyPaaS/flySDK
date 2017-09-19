#pragma once

// SManage��������ַ
const char gSManageIP[32] = "60.205.137.243";
const int gSManagePort = 7886;

// ��ȡ����IP��url
const wchar_t gGetIpUrl[128] = L"http://59.110.10.28:8097/v3/getnatip?";

// SDK��SManage֮�����Ϣ����
#define MSG_REGISTER_REQ			1		// SDKע������
#define MSG_REGISTER_RSP			2		// SDKע����Ӧ
#define MSG_HB_REQ					3		// SDK��������
#define MSG_HB_RSP					4		// SDK������Ӧ
#define MSG_BIND_SESSION_REQ		5		// �󶨻Ự����
#define MSG_BIND_SESSION_RSP		6		// �󶨻Ự��Ӧ
#define MSG_CONNECT_SESSION			7		// �����Ự������Ϣ
#define MSG_ACCEPT_SESSION			8		// ���ջỰ������Ϣ
#define MSG_REJECT_SESSION			9		// �ܽӻỰ������Ϣ
#define MSG_DISCONN_SESSION_REQ		10		// �ͷŻỰ������Ϣ
#define MSG_DISCONN_SESSION_RSP		11		// �ͷŻỰ��Ӧ��Ϣ
#define MSG_LISTEN_SESSION_REQ		12		// �Ự����������Ϣ
#define MSG_LISTEN_SESSION_RSP		13		// �Ự������Ӧ��Ϣ
#define MSG_UNREGISTER_REQ			14		// SDKע������
#define MSG_UNREGISTER_RSP			15		// SDKע����Ӧ

// SDK��SManage֮�����Ϣ��������
#define ATR_AUTHEN			0
#define ATR_SID				1
#define ATR_APPID			2
#define ATR_POLICY			3
#define ATR_ROUTEHOP		4
#define ATR_ROUTENUM		5
#define ATR_PROTOCOL		6
#define ATR_UDPPORTNUM		7
#define ATR_VFLAG			8
#define ATR_IP				9
#define ATR_PORT           10
#define ATR_APLISTSTATUS   11
#define ATR_RETCODE        12
#define ATR_SESSIONID      13
#define ATR_APINFO         14
#define ATR_SDKID          15
#define ATR_APLIST         16
#define ATR_SIG            17
#define ATR_PEERSDKID      18

#pragma pack(push)
#pragma pack(1)

// ��Ϣͷ
typedef struct
{
	unsigned char msg_type;
	unsigned char msg_sn;
	unsigned int msg_time;
	unsigned short msg_len;
} MSG_HEAD;

// TLV��ʽ
typedef struct
{
	unsigned char ulTag;		// Tag
	unsigned char usLen;		// Len
	unsigned char pValue[128];	// Value
} TLV;

// IP,�˿ڶ�
typedef struct
{
	char szIp[32];			// IP�ַ�����.��ʾ
	unsigned char nIP[4];	// IP�����ʾ
	unsigned short nPort;	// �˿�
	unsigned short nDelay;	// ��ʱ
	unsigned char nLost;	// ����
} IPORT;

// �ͻ��˵���Ϣ
typedef struct
{
	unsigned int m_nPeerSn;
	unsigned int m_dwPeerTime;
	char m_strPeerSdkid[32];
	char m_strPeerSdkPeerId[32];
} CLIENT;

#pragma pack(pop)
