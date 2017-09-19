// flySDK.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "flySDK.h"
#include "flySession.h"

// 全局对象
flySession g_flySession;

DLL_EXPORT_IMPORT void FlyCanInit(char *sid, char *token, char *appid, char *sdkid)
{
	g_flySession.SetParam(sid, token, appid, sdkid);
}

DLL_EXPORT_IMPORT void FlyCanUnInit()
{
	// 暂不实现
}

DLL_EXPORT_IMPORT void FlyCanAddCallBack(CallBackFunc cb)
{
	g_flySession.AddCallBack(cb);
}

DLL_EXPORT_IMPORT void FlyCanRemoveCallBack(CallBackFunc cb)
{
	g_flySession.RemoveCallBack(cb);
}

DLL_EXPORT_IMPORT void FlyCanRegister()
{
	g_flySession.Register();
}

DLL_EXPORT_IMPORT void FlyCanUnRegister()
{
	g_flySession.UnRegister();
}

DLL_EXPORT_IMPORT void* FlyCanCreateSession(int protocol)
{
	return g_flySession.CreateSession(protocol);
}

DLL_EXPORT_IMPORT void FlyCanReleaseSession(void *handle)
{
	g_flySession.ReleaseSession(handle);
}

DLL_EXPORT_IMPORT void FlyCanListenSession(void *handle, int nNum)
{
	g_flySession.ListenSession(handle, nNum);
}

DLL_EXPORT_IMPORT bool FlyCanConnectSession(void *handle, char *peersdkid)
{
	return g_flySession.ConnectSession(handle, peersdkid);
}

DLL_EXPORT_IMPORT bool FlyCanAcceptSession(void *handle)
{
	return g_flySession.AcceptSession(handle);
}

DLL_EXPORT_IMPORT bool FlyCanRejectSession(void *handle)
{
	return g_flySession.RejectSession(handle);
}

DLL_EXPORT_IMPORT bool FlyCanSend(void *handle, void *pData, int nLen, int nChannalId)
{
	return g_flySession.Send(handle, pData, nLen, nChannalId);
}