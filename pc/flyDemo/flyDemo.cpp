// flyDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <vector>

#include "../flySDK/flySDK.h"
#pragma comment(lib, "../flySDK/output/flySDK.lib")

int nMaxChannal = 0;
bool bClient = false;
HANDLE dwHandleServer = NULL;
HANDLE dwHandleClient = NULL;
std::vector<HANDLE> vtList;
void CallBackEventCB(int nEvent, int nCode, void *pData);
void CallBackRecvCB(void* pHandle, void *pData, int nLen, int nChannalId);

int main()
{
	/*
	char sid[64] = "809d2c33e8ba713460083ee65de42a71";
	char token[64] = "980e7daebdf7fc4aa6f55bda74b41d17";
	char appid[64] = "47201f1d7bd043c18069375172f82572";
	char sdkid[64] = "62395051197522";
	char peersdkid[64] = "62395051197523";
	*/
	
	char sid[64] = "b64e977c108810429b9056208059d362";
	char token[64] = "cd1e4ce88775dcaf8bbf9236e9811c4a";
	char appid[64] = "57993353d8724285904ba22a20d51ee9";
	char sdkid[64] = "62395051197525";
	char peersdkid[64] = "62395051197526";
	
	CallBackFunc pCallBack = new CallBackCB();
	pCallBack->event_cb = CallBackEventCB;
	pCallBack->recv_cb = CallBackRecvCB;

	// 模式(0-服务器端,1-对称模式客户端,2-非对称模式客户端,3-TCP非对称模式)
	int nMode = 3;
	if (nMode == 0)
	{
		FlyCanInit(sid, token, appid, sdkid);
		FlyCanAddCallBack(pCallBack);
		FlyCanRegister();
		Sleep(2000);
		dwHandleServer = FlyCanCreateSession(PROTOCOL_UDP);
		Sleep(2000);
		FlyCanListenSession(dwHandleServer, 5);
		Sleep(20000);
		if (!bClient)
		{
			for (int i = 0; i < (int)vtList.size(); i++)
			{
				FlyCanReleaseSession(vtList[i]);
			}
			vtList.clear();
		}
		Sleep(2000);
		FlyCanUnRegister();
		FlyCanRemoveCallBack(pCallBack);
		FlyCanUnInit();
	}
	else if (nMode == 1)
	{
		FlyCanInit(sid, token, appid, sdkid);
		FlyCanAddCallBack(pCallBack);
		FlyCanRegister();
		Sleep(2000);
		dwHandleClient = FlyCanCreateSession(PROTOCOL_UDP);
		Sleep(2000);
		FlyCanConnectSession(dwHandleClient, peersdkid);
		Sleep(20000);
		if (bClient)
		{
			FlyCanReleaseSession(dwHandleClient);
		}
		Sleep(2000);
		FlyCanUnRegister();
		FlyCanRemoveCallBack(pCallBack);
		FlyCanUnInit();
	}
	else if (nMode == 2)
	{
		FlyCanInit(sid, token, appid, sdkid);
		FlyCanAddCallBack(pCallBack);
		FlyCanRegister();
		Sleep(2000);
		dwHandleClient = FlyCanCreateSession(PROTOCOL_UDP);
		Sleep(2000);
		FlyCanConnectServer(dwHandleClient, "114.112.83.110", 29099);
		Sleep(20000);
		if (bClient)
		{
			FlyCanReleaseSession(dwHandleClient);
		}
		Sleep(2000);
		FlyCanUnRegister();
		FlyCanRemoveCallBack(pCallBack);
		FlyCanUnInit();
	}
	else if (nMode == 3)
	{
		FlyCanInit(sid, token, appid, sdkid);
		FlyCanAddCallBack(pCallBack);
		FlyCanRegister();
		Sleep(2000);
		dwHandleClient = FlyCanCreateSession(PROTOCOL_TCP);
		Sleep(2000);
		FlyCanConnectServer(dwHandleClient, "114.112.83.110", 9099);
		Sleep(20000);
		if (bClient)
		{
			FlyCanReleaseSession(dwHandleClient);
		}
		Sleep(2000);
		FlyCanUnRegister();
		FlyCanRemoveCallBack(pCallBack);
		FlyCanUnInit();
	}
    return 0;
}

void CallBackEventCB(int nEvent, int nCode, void *pData)
{
	CStringA str;
	str.Format("nEvent = %d, nCode = %d\r\n", nEvent, nCode);
	OutputDebugStringA(str);

	if (nEvent == EVT_SESSION_INCOMING)
	{
		nMaxChannal = nCode;
		bClient = false;
		// 接收会话
		FlyCanAcceptSession(pData);
		vtList.push_back(pData);
		// 拒绝会话
		//FlyCanRejectSession(pData);
	}
	if (nEvent == EVT_SESSION_ACCEPT)
	{
		nMaxChannal = nCode;
		bClient = true;
		// 更新代码
		char szData[32] = "Only you can love me!";
		for (int i = 0; i < nMaxChannal; i++)
		{
			for (size_t j = 0; j < 20; j++)
			{
				FlyCanSend(dwHandleClient, szData, strlen(szData), i);

				CStringA str;
				str.Format("FlyCanSend = %s, nChannal = %d, MaxChannal = %d\r\n", szData, i, nMaxChannal);
				OutputDebugStringA(str);

				Sleep(10);
			}
		}
	}
	if (nEvent == EVT_SESSION_REJECT)
	{
		
	}
}

void CallBackRecvCB(void* pHandle, void *pData, int nLen, int nChannalId)
{
	CStringA str;
	str.Format("EVT_PACKET_RCVD data = %s,nChannalId = %d\r\n", (char*)pData, nChannalId);
	OutputDebugStringA(str);
}

