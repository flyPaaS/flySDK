// flyDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>

#include "./flySDK.h"
#pragma comment(lib, "./flySDK.lib")

int nMaxChannal = 0;
HANDLE dwHandle = NULL;
void CallBackEventCB(int nEvent, int nCode, void *pData);
void CallBackRecvCB(void* pHandle, void *pData, int nLen, int nChannalId);

int main()
{
	/* 
	char sid[64] = "809d2c33e8ba713460083ee65de42a71";
	char token[64] = "980e7daebdf7fc4aa6f55bda74b41d17";
	char appid[64] = "47201f1d7bd043c18069375172f82572";*/

	char sid[64] = "b64e977c108810429b9056208059d362";
	char token[64] = "cd1e4ce88775dcaf8bbf9236e9811c4a";
	char appid[64] = "57993353d8724285904ba22a20d51ee9";

	CallBackFunc pCallBack = new CallBackCB();
	pCallBack->event_cb = CallBackEventCB;
	pCallBack->recv_cb = CallBackRecvCB;

	if (1)
	{
		char sdkid[64] = "62395051197526";
		char peersdkid[64] = "62395051197525";

		FlyCanInit(sid, token, appid, sdkid);
		FlyCanAddCallBack(pCallBack);
		FlyCanRegister();
		Sleep(2000);
		dwHandle = FlyCanCreateSession(0);
		Sleep(2000);
		FlyCanListenSession(dwHandle, 5);
		Sleep(20000);
		FlyCanReleaseSession(dwHandle);
		Sleep(2000);
		FlyCanUnRegister();
		FlyCanUnInit();
	}
	else
	{
		char peersdkid[64] = "62395051197526";
		char sdkid[64] = "62395051197525";

		FlyCanInit(sid, token, appid, sdkid);
		FlyCanAddCallBack(pCallBack);
		FlyCanRegister();
		Sleep(2000);
		dwHandle = FlyCanCreateSession(0);
		Sleep(2000);
		FlyCanListenSession(dwHandle, 5);
		Sleep(2000);
		FlyCanConnectSession(dwHandle, peersdkid);
		Sleep(5000);
		FlyCanReleaseSession(dwHandle);
		Sleep(2000);
		FlyCanUnRegister();
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
		FlyCanAcceptSession(pData);
		//FlyCanRejectSession(pData);
	}
	if (nEvent == EVT_SESSION_ACCEPT)
	{
		nMaxChannal = nCode;
		char szData[32] = "Only you can love me!";
		for (int i = 0; i < nMaxChannal; i++)
		{
			FlyCanSend(dwHandle, szData, strlen(szData), i);

			str.Format("FlyCanSend = %s, nChannal = %d\r\n", szData, i);
			OutputDebugStringA(str);
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

