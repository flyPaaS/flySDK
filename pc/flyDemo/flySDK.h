#pragma once

#ifdef DLLProvider
#define DLL_EXPORT_IMPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT_IMPORT extern "C" __declspec(dllimport)
#endif

// 协议类型
#define PROTOCOL_UDP	0
#define PROTOCOL_TCP	1
//#define PROTOCOL_RTP	2
//#define PROTOCOL_QUIC	3

// 回调的事件
#define EVT_REGISTER_SUCCUSS		1000		// SDK注册成功
#define EVT_REGISTER_FAILURE		1001		// SDK注册失败
#define EVT_LOGINOUT_SUCCUSS		1002		// SDK注销成功
#define EVT_SESSION_BIND_SUCCUSS	1003		// Session绑定成功
#define EVT_SESSION_BIND_FAILURE	1004		// Session绑定失败
#define EVT_SESSION_LISTEN_SUCCUSS	1005		// Session监听成功
#define EVT_SESSION_LISTEN_FAILURE	1006		// Session监听失败
#define EVT_SESSION_RELEASE_SUCCUSS	1007		// Session释放成功
#define EVT_SESSION_RELEASE_FAILURE	1008		// Session释放失败
#define EVT_SESSION_INCOMING		1009		// 会话连接请求
#define EVT_SESSION_ACCEPT			1010		// 会话连接确认
#define EVT_SESSION_REJECT			1011		// 会话连接拒绝

// 事件回调
typedef void(*CallBackEvent)(int nEvent, int nCode, void *pData);
// 接收数据回调
typedef void(*CallBackRecv)(void* pHandle, void *pData, int nLen, int nChannalId);

// 回调函数集合
typedef struct
{
	CallBackEvent event_cb;
	CallBackRecv recv_cb;
} CallBackCB,*CallBackFunc;


// 初始化
DLL_EXPORT_IMPORT void FlyCanInit(char *sid, char *token, char *appid, char *sdkid);

// 反初始化
DLL_EXPORT_IMPORT void FlyCanUnInit();

// 设置回调
DLL_EXPORT_IMPORT void FlyCanAddCallBack(CallBackFunc cb);

// 删除回调
DLL_EXPORT_IMPORT void FlyCanRemoveCallBack(CallBackFunc cb);

// 账号登录，结果通过回调异步返回
DLL_EXPORT_IMPORT void FlyCanRegister();

// 账号注销
DLL_EXPORT_IMPORT void FlyCanUnRegister();

// 创建一个指定协议会话,返回会话句柄
DLL_EXPORT_IMPORT void* FlyCanCreateSession(int protocol);

// 释放指定协议会话
DLL_EXPORT_IMPORT void FlyCanReleaseSession(void *handle);

// 监听一个会话
DLL_EXPORT_IMPORT void FlyCanListenSession(void *handle, int nNum);

// 告诉flyCan需要连接的目的地id(类似拨打电话)
DLL_EXPORT_IMPORT bool FlyCanConnectSession(void *handle, char *peersdkid);

// 目的地收到连接请求，接收请求(类似来电接听)
DLL_EXPORT_IMPORT bool FlyCanAcceptSession(void *handle);

// 目的地收到连接请求，拒绝请求(类似来电拒接)
DLL_EXPORT_IMPORT bool FlyCanRejectSession(void *handle);

// 发送数据(接收数据通过回调返回)
DLL_EXPORT_IMPORT bool FlyCanSend(void *handle, void *pData, int nLen, int nChannalId);

// 直接连接对应的地址
DLL_EXPORT_IMPORT int FlyCanConnectServer(void *handle, char *ip, int port);




