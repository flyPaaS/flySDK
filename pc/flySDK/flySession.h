#pragma once
#include "flySDK.h"
#include "flyData.h"

class flySession
{
public:
	flySession();
	virtual ~flySession();

public:
	// 设置账号
	void SetParam(char *sid, char *token, char *appid, char *sdkid);
	// 设置回调
	void AddCallBack(CallBackFunc cb);
	// 移除回调
	void RemoveCallBack(CallBackFunc cb);

	// 启动SDK与SManage通信
	void Register();
	// 停止SDK与SManage通信
	void UnRegister();

	// 创建会话
	void* CreateSession(int protocol);
	// 释放会话
	void ReleaseSession(void *handle);
	// 监听会话
	void ListenSession(void *handle, int num);

	// 释放会话响应
	void ReleaseSessionRsp(void *handle, BYTE sn, DWORD time);

	// 告诉flyCan需要连接的目的地地址(类似拨打电话)
	bool ConnectSession(void *handle, char *peersdkid);
	// 目的地收到连接请求，接收请求(类似来电接听)
	bool AcceptSession(void *handle);
	// 目的地收到连接请求，拒绝请求(类似来电拒接)
	bool RejectSession(void *handle);

	// 发送数据(接收数据通过回调返回)
	bool Send(void *handle, void *data, int len, int nChannalId);

private:
	// 登录SManage
	bool SendLogin();
	// 心跳SManage
	bool SendHeat();
	// 退录SManage
	bool SendLoginOut();

	// 发送绑定指令
	bool SendBind(DWORD sessionid);
	// 发送退出指令
	bool SendRelease(DWORD sessionid);
	// 发送监听指令
	bool SendListen(DWORD sessionid);

	// 发送会话释放响应
	bool SendReleaseRsp(DWORD sessionid, BYTE sn, DWORD time);

	// 发送连接指令
	bool SendConnect(DWORD sessionid, char *peersdkid);
	// 发送接收指令
	bool SendAccept(DWORD sessionid);
	// 发送拒接指令
	bool SendReject(DWORD sessionid);
	
	// 处理接收的数据包
	void OnRecvData(unsigned char *pData, int nSize);
	
	// SDK和SManage通讯 心跳线程
	DWORD static WINAPI WorkThread(LPVOID lpParam);
	// SDK和SManage通讯 接收线程
	DWORD static WINAPI RecvThread(LPVOID lpParam);

	// 获取时间戳
	CStringA GetTimeStr();

	// 获取本地公网IP
	void GetLocalIp();
	// 获取AP列表的RTT
	void GetAPDelay();
	// 发送穿透包
	void SendNatPack(flyData *pData);

public:
	// 回调函数
	vector<CallBackFunc> m_vtCallBack;
	// 数据对象哈希
	map<DWORD, flyData*> m_mpSession;
	// 客户端对象哈希
	map<DWORD, CLIENT*> m_mpClient;

private:
	// SessionId(递增)
	DWORD m_dwSessionId;

	// 校验参数本地保存
	CStringA m_strSid;
	CStringA m_strToken;
	CStringA m_strAppId;
	CStringA m_strSdkId;

	// SN
	BYTE m_nSn;
	// 连接状态
	int m_nStatus;
	// 接收回应
	HANDLE m_hEvent[2];
	// 线程标记
	HANDLE m_hWorkEvent;
	HANDLE m_hRecvEvent;
	HANDLE m_hWorkThread;
	HANDLE m_hRecvThread;
	// UDP连接对象
	UdpBase m_cClient;

	// 启动服务
	bool m_bPing;
	// 返回的AP列表
	vector<IPORT*> m_vtAPList;

	// 启动本地IP服务
	bool m_bLocalIp;
	// 保存本地IP
	CStringA m_strLocalIP;
};

