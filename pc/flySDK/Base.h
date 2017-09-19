#pragma once

// SManage服务器地址
const char gSManageIP[32] = "60.205.137.243";
const int gSManagePort = 7886;

// 获取本地IP的url
const wchar_t gGetIpUrl[128] = L"http://59.110.10.28:8097/v3/getnatip?";

// SDK和SManage之间的消息类型
#define MSG_REGISTER_REQ			1		// SDK注册请求
#define MSG_REGISTER_RSP			2		// SDK注册响应
#define MSG_HB_REQ					3		// SDK心跳请求
#define MSG_HB_RSP					4		// SDK心跳响应
#define MSG_BIND_SESSION_REQ		5		// 绑定会话请求
#define MSG_BIND_SESSION_RSP		6		// 绑定会话响应
#define MSG_CONNECT_SESSION			7		// 创建会话请求消息
#define MSG_ACCEPT_SESSION			8		// 接收会话请求消息
#define MSG_REJECT_SESSION			9		// 拒接会话请求消息
#define MSG_DISCONN_SESSION_REQ		10		// 释放会话请求消息
#define MSG_DISCONN_SESSION_RSP		11		// 释放会话响应消息
#define MSG_LISTEN_SESSION_REQ		12		// 会话监听请求消息
#define MSG_LISTEN_SESSION_RSP		13		// 会话监听响应消息
#define MSG_UNREGISTER_REQ			14		// SDK注销请求
#define MSG_UNREGISTER_RSP			15		// SDK注销响应

// SDK和SManage之间的消息参数定义
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

// 消息头
typedef struct
{
	unsigned char msg_type;
	unsigned char msg_sn;
	unsigned int msg_time;
	unsigned short msg_len;
} MSG_HEAD;

// TLV格式
typedef struct
{
	unsigned char ulTag;		// Tag
	unsigned char usLen;		// Len
	unsigned char pValue[128];	// Value
} TLV;

// IP,端口对
typedef struct
{
	char szIp[32];			// IP字符串带.表示
	unsigned char nIP[4];	// IP数组表示
	unsigned short nPort;	// 端口
	unsigned short nDelay;	// 延时
	unsigned char nLost;	// 丢包
} IPORT;

// 客户端的信息
typedef struct
{
	unsigned int m_nPeerSn;
	unsigned int m_dwPeerTime;
	char m_strPeerSdkid[32];
	char m_strPeerSdkPeerId[32];
} CLIENT;

#pragma pack(pop)
