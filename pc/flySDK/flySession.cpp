#include "stdafx.h"
#include "flySession.h"
#include "md5.h"
#include "base64.h"
#include "HttpTool.h"

flySession::flySession()
{
	m_dwSessionId = 10000;
	m_vtCallBack.clear();
	m_mpSession.clear();
	m_mpClient.clear();

	m_strSid.Empty();
	m_strToken.Empty();
	m_strAppId.Empty();
	m_strSdkId.Empty();

	m_nSn = 0;
	m_nStatus = 0;

	m_hEvent[0] = NULL;
	m_hEvent[1] = NULL;
	m_hWorkEvent = NULL;
	m_hRecvEvent = NULL;
	m_hWorkThread = NULL;
	m_hRecvThread = NULL;

	m_bPing = false;
	m_vtAPList.clear();

	m_bLocalIp = false;
	m_strLocalIP.Empty();
}

flySession::~flySession()
{

}

void flySession::SetParam(char *sid, char *token, char *appid, char *sdkid)
{
	if (sid != NULL)
	{
		m_strSid.Format("%s", sid);
	}
	if (token != NULL)
	{
		m_strToken.Format("%s", token);
	}
	if (appid != NULL)
	{
		m_strAppId.Format("%s", appid);
	}
	if (sdkid != NULL)
	{
		m_strSdkId.Format("%s", sdkid);
	}
}

void flySession::AddCallBack(CallBackFunc cb)
{
	m_vtCallBack.push_back(cb);
}

void flySession::RemoveCallBack(CallBackFunc cb)
{
	for (size_t i = 0; i < m_vtCallBack.size(); i++)
	{
		if (m_vtCallBack[i] == cb)
		{
			m_vtCallBack.erase(m_vtCallBack.begin() + i);
			break;
		}
	}
}

void flySession::Register()
{
	UnRegister();
	// ��������IP
	m_bLocalIp = true;
	// ��ʼ��Socket
	m_cClient.Init();
	m_nStatus = 0;
	// �����¼�֪ͨ
	m_hEvent[0] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvent[1] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	// �����߳�
	m_hWorkEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hRecvEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hWorkThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
	m_hRecvThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, this, 0, NULL);
}

void flySession::UnRegister()
{
	if (m_nStatus != 0)
	{
		SendLoginOut();
		m_nStatus = 0;
	}
	// �ر�Socket
	m_cClient.Free();
	// �رս����߳�
	SetEvent(m_hRecvEvent);
	// �رչ����߳�
	SetEvent(m_hWorkEvent);
	// �ȴ������߳��˳�
	if (m_hRecvThread != NULL)
	{
		DWORD dwStart = GetTickCount();
		while (1)
		{
			int nRet = ::WaitForSingleObject(m_hRecvThread, 1000);
			if (nRet == WAIT_OBJECT_0)
			{
				break;
			}

			if (GetTickCount() - dwStart > 3000)
			{
				::TerminateThread(m_hRecvThread, 0);
				break;
			}
		}
		::CloseHandle(m_hRecvThread);
		m_hRecvThread = NULL;
	}
	if (m_hRecvEvent != NULL)
	{
		::CloseHandle(m_hRecvEvent);
		m_hRecvEvent = NULL;
	}
	// �رչ����߳�
	if (m_hWorkThread != NULL)
	{
		DWORD dwStart = GetTickCount();
		while (1)
		{
			int nRet = ::WaitForSingleObject(m_hWorkThread, 1000);
			if (nRet == WAIT_OBJECT_0)
			{
				break;
			}

			if (GetTickCount() - dwStart > 3000)
			{
				::TerminateThread(m_hWorkThread, 0);
				break;
			}
		}
		::CloseHandle(m_hWorkThread);
		m_hWorkThread = NULL;
	}
	if (m_hWorkEvent != NULL)
	{
		::CloseHandle(m_hWorkEvent);
		m_hWorkEvent = NULL;
	}
	// �ر��¼�֪ͨ
	if (m_hEvent[0] != NULL)
	{
		::CloseHandle(m_hEvent[0]);
		m_hEvent[0] = NULL;
	}
	if (m_hEvent[1] != NULL)
	{
		::CloseHandle(m_hEvent[1]);
		m_hEvent[1] = NULL;
	}
	// �ͷ�AP�б�
	for (vector<IPORT*>::iterator iter = m_vtAPList.begin(); iter != m_vtAPList.end(); iter++)
	{
		delete (*iter);
	}
	m_vtAPList.clear();
	// �ͷ����ݶ����ϣ�б�
	for (map<DWORD, flyData*>::iterator iter = m_mpSession.begin(); iter != m_mpSession.end(); iter++)
	{
		flyData *pData = iter->second;
		if (pData != NULL)
		{
			pData->Free();
			delete pData;
			pData = NULL;
		}
	}
	m_mpSession.clear();
	// �ͷſͻ��˶����ϣ�б�
	for (map<DWORD, CLIENT*>::iterator iter = m_mpClient.begin(); iter != m_mpClient.end(); iter++)
	{
		CLIENT *pData = iter->second;
		if (pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_mpClient.clear();
	// �ص�״̬
	for (size_t i = 0; i < m_vtCallBack.size(); i++)
	{
		m_vtCallBack[i]->event_cb(EVT_LOGINOUT_SUCCUSS, 0, NULL);
	}
}

void* flySession::CreateSession(int protocol)
{
	flyData *pData = new flyData();
	pData->nProtocol = protocol;
	pData->pSession = this;
	// �ж�Э������
	if (pData->nProtocol == PROTOCOL_UDP)
	{
		// ��������
		if (SendBind(m_dwSessionId))
		{
			// ��ʼ��
			pData->Init();
			// ����MP;
			DWORD dwKey = m_dwSessionId++;
			pData->dwSessionId = dwKey;
			m_mpSession[dwKey] = pData;
			// �ص�״̬
			for (size_t i = 0; i < m_vtCallBack.size(); i++)
			{
				m_vtCallBack[i]->event_cb(EVT_SESSION_BIND_SUCCUSS, 0, NULL);
			}
			return (void*)dwKey;
		}
		else
		{
			// �ͷž��
			delete pData;
			pData = NULL;
			// �ص�״̬
			for (size_t i = 0; i < m_vtCallBack.size(); i++)
			{
				m_vtCallBack[i]->event_cb(EVT_SESSION_BIND_FAILURE, 0, NULL);
			}
		}
	}
	return NULL;
}

void flySession::ListenSession(void *handle, int num)
{
	if (handle != NULL)
	{
		// ��ѯMP
		DWORD dwKey = (DWORD)handle;
		map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwKey);
		if (iter != m_mpSession.end())
		{
			// �ҵ���Ӧ��Session����
			flyData *pData = iter->second;
			if (pData != NULL)
			{
				// �ж�Э������
				if (pData->nProtocol == PROTOCOL_UDP)
				{
					// ��������
					if (SendListen(dwKey))
					{
						// �ص�״̬
						for (size_t i = 0; i < m_vtCallBack.size(); i++)
						{
							m_vtCallBack[i]->event_cb(EVT_SESSION_LISTEN_SUCCUSS, 0, NULL);
						}
						return;
					}
				}
			}
		}
	}
	// �ص�״̬
	for (size_t i = 0; i < m_vtCallBack.size(); i++)
	{
		m_vtCallBack[i]->event_cb(EVT_SESSION_LISTEN_FAILURE, 0, NULL);
	}
}

void flySession::ReleaseSession(void *handle)
{
	if (handle != NULL)
	{
		// ��ѯ���ݶ���
		bool bSuc = false;
		DWORD dwKey = (DWORD)handle;
		map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwKey);
		if (iter != m_mpSession.end())
		{
			// �ҵ���Ӧ��Session����
			flyData *pData = iter->second;
			if (pData != NULL)
			{
				// �ж�Э������
				if (pData->nProtocol == PROTOCOL_UDP)
				{
					// �����ͷ�����
					if (SendRelease(dwKey))
					{
						// �Ƴ����ݶ���
						pData->Free();
						delete pData;
						m_mpSession.erase(iter);
						bSuc = true;
					}
				}
			}
		}
		// ��ѯ�ͻ��˶���
		dwKey = (DWORD)handle;
		map<DWORD, CLIENT*>::iterator iter1 = m_mpClient.find(dwKey);
		if (iter1 != m_mpClient.end())
		{
			CLIENT *pData = iter1->second;
			if (pData != NULL)
			{
				delete pData;
				m_mpClient.erase(iter1);
				bSuc = true;
			}
		}
		if (bSuc)
		{

			// �ص�״̬
			for (size_t i = 0; i < m_vtCallBack.size(); i++)
			{
				m_vtCallBack[i]->event_cb(EVT_SESSION_RELEASE_SUCCUSS, 0, NULL);
			}
			return;
		}
	}
	// �ص�״̬
	for (size_t i = 0; i < m_vtCallBack.size(); i++)
	{
		m_vtCallBack[i]->event_cb(EVT_SESSION_RELEASE_FAILURE, 0, NULL);
	}
}

void flySession::ReleaseSessionRsp(void *handle, BYTE sn, DWORD time)
{
	if (handle != NULL)
	{
		// ��ѯ���ݶ���
		bool bSuc = false;
		DWORD dwKey = (DWORD)handle;
		map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwKey);
		if (iter != m_mpSession.end())
		{
			// �ҵ���Ӧ��Session����
			flyData *pData = iter->second;
			if (pData != NULL)
			{
				// �ж�Э������
				if (pData->nProtocol == PROTOCOL_UDP)
				{
					// �����ͷ�����
					if (SendReleaseRsp(dwKey, sn, time))
					{
						// �Ƴ����ݶ���
						pData->Free();
						delete pData;
						m_mpSession.erase(iter);
						bSuc = true;
					}
				}
			}
		}
		// ��ѯ�ͻ��˶���
		dwKey = (DWORD)handle;
		map<DWORD, CLIENT*>::iterator iter1 = m_mpClient.find(dwKey);
		if (iter1 != m_mpClient.end())
		{
			CLIENT *pData = iter1->second;
			if (pData != NULL)
			{
				delete pData;
				m_mpClient.erase(iter1);
				bSuc = true;
			}
		}
		if (bSuc)
		{

			// �ص�״̬
			for (size_t i = 0; i < m_vtCallBack.size(); i++)
			{
				//m_vtCallBack[i](EVT_SESSION_RELEASE_SUCCUSS, 0, NULL);
			}
			return;
		}
	}
	// �ص�״̬
	for (size_t i = 0; i < m_vtCallBack.size(); i++)
	{
		//m_vtCallBack[i](EVT_SESSION_RELEASE_FAILURE, 0, NULL);
	}
}

bool flySession::ConnectSession(void *handle, char *sdkid)
{
	if (handle != NULL)
	{
		// ��ѯMP
		DWORD dwKey = (DWORD)handle;
		map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwKey);
		if (iter != m_mpSession.end())
		{
			// �ҵ���Ӧ��Session����
			flyData *pData = iter->second;
			if (pData != NULL)
			{
				// �ж�Э������
				if (pData->nProtocol == PROTOCOL_UDP)
				{
					// ����ָ��
					return SendConnect(dwKey, sdkid);
				}
			}
		}
	}
	return false;
}

bool flySession::AcceptSession(void *handle)
{
	if (handle != NULL)
	{
		// ��ѯMP
		DWORD dwKey = (DWORD)handle;
		map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwKey);
		if (iter != m_mpSession.end())
		{
			// �ҵ���Ӧ��Session����
			flyData *pData = iter->second;
			if (pData != NULL)
			{
				// �ж�Э������
				if (pData->nProtocol == PROTOCOL_UDP)
				{
					// ����ָ��
					return SendAccept(dwKey);
				}
			}
		}
	}
	return false;
}

bool flySession::RejectSession(void *handle)
{
	bool bResult = false;
	if (handle != NULL)
	{
		// ��ѯMP
		DWORD dwKey = (DWORD)handle;
		map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwKey);
		if (iter != m_mpSession.end())
		{
			// �ҵ���Ӧ��Session����
			flyData *pData = iter->second;
			if (pData != NULL)
			{
				// �ж�Э������
				if (pData->nProtocol == PROTOCOL_UDP)
				{
					// ����ָ��
					bResult = SendReject(dwKey);
					// ɾ������
					map<DWORD, CLIENT*>::iterator iter1 = m_mpClient.find(dwKey);
					if (iter1 != m_mpClient.end())
					{
						CLIENT *pClient = iter1->second;
						if (pClient != NULL)
						{
							delete pClient;
							m_mpClient.erase(iter1);
						}
					}
				}
			}
		}
	}
	return bResult;
}

bool flySession::Send(void *handle, void *data, int len, int nChannalId)
{
	bool bResult = false;
	if (handle != NULL)
	{
		// ��ѯMP
		DWORD dwKey = (DWORD)handle;
		map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwKey);
		if (iter != m_mpSession.end())
		{
			// �ҵ���Ӧ��Session����
			flyData *pData = iter->second;
			if (pData != NULL)
			{
				// �ж�Э������
				if (pData->nProtocol == PROTOCOL_UDP)
				{
					if (nChannalId < pData->nUse)
					{
						if (nChannalId > 3)
						{
							bResult = pData->udp[nChannalId].Send(pData->szIp2, pData->nPort[nChannalId], (char *)data, len);
						}
						else
						{
							bResult = pData->udp[nChannalId].Send(pData->szIp1, pData->nPort[nChannalId], (char *)data, len);
						}
					}
				}
			}
		}
	}
	return bResult;
}

bool flySession::SendLogin()
{
	MD5 md5;
	CStringA md5Data = "";
	CStringA strTime = GetTimeStr();
	CStringA strSig = m_strSid + m_strToken + strTime;
	// MD5����
	md5.reset();
	md5.update((LPCSTR)strSig, strSig.GetLength());
	md5Data.Format("%s", md5.toString().c_str());
	md5Data.MakeUpper();
	// BASE64����
	int nData = MAX_PATH;
	char szData[MAX_PATH] = { 0 };
	CStringA strAuthen = m_strSid + ":" + strTime;
	base64_encode((const BYTE *)(LPCSTR)strAuthen, strAuthen.GetLength(), (BYTE *)szData, &nData);
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	memset(pData, 0, 1024);
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_REGISTER_REQ;
	head.msg_sn = m_nSn++;
	head.msg_time = GetTickCount();
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body authen
	pData[nDataLen++] = ATR_AUTHEN;
	pData[nDataLen++] = nData + 2;
	memcpy(pData + nDataLen, szData, nData);
	nDataLen += nData;
	// body sid
	pData[nDataLen++] = ATR_SID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSid);
	nDataLen += 32;
	// body appid
	pData[nDataLen++] = ATR_APPID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strAppId);
	nDataLen += 32;
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// body sig
	pData[nDataLen++] = ATR_SIG;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, md5Data);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	ResetEvent(m_hEvent[0]);
	ResetEvent(m_hEvent[1]);
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
		if (dwResult == WAIT_OBJECT_0)
		{
			bResult = true;
		}
		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			bResult = false;
		}
		if (dwResult == WAIT_TIMEOUT)
		{
			bResult = false;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendHeat()
{
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_HB_REQ;
	head.msg_sn = m_nSn++;
	head.msg_time = GetTickCount();
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body aplist
	pData[nDataLen++] = ATR_APLISTSTATUS;
	pData[nDataLen++] = 2 + (BYTE)(m_vtAPList.size() * 7);
	for (int i = 0; i < (int)m_vtAPList.size(); i++)
	{
		IPORT *pNode = m_vtAPList[i];
		pData[nDataLen++] = pNode->nIP[0];
		pData[nDataLen++] = pNode->nIP[1];
		pData[nDataLen++] = pNode->nIP[2];
		pData[nDataLen++] = pNode->nIP[3];
		pData[nDataLen++] = BYTE((pNode->nDelay & 0xFF00) >> 16);
		pData[nDataLen++] = BYTE(pNode->nDelay & 0xFF);
		pData[nDataLen++] = pNode->nLost;
	}
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	ResetEvent(m_hEvent[0]);
	ResetEvent(m_hEvent[1]);
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
		if (dwResult == WAIT_OBJECT_0)
		{
			bResult = true;
		}
		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			bResult = false;
		}
		if (dwResult == WAIT_TIMEOUT)
		{
			bResult = false;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendLoginOut()
{
	MD5 md5;
	CStringA md5Data = "";
	CStringA strTime = GetTimeStr();
	CStringA strSig = m_strSid + m_strToken + strTime;
	// MD5����
	md5.reset();
	md5.update((LPCSTR)strSig, strSig.GetLength());
	md5Data.Format("%s", md5.toString().c_str());
	md5Data.MakeUpper();
	// BASE64����
	int nData = MAX_PATH;
	char szData[MAX_PATH] = { 0 };
	CStringA strAuthen = m_strSid + ":" + strTime;
	base64_encode((const BYTE *)(LPCSTR)strAuthen, strAuthen.GetLength(), (BYTE *)szData, &nData);
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	memset(pData, 0, 1024);
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_UNREGISTER_REQ;
	head.msg_sn = m_nSn++;
	head.msg_time = GetTickCount();
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body authen
	pData[nDataLen++] = ATR_AUTHEN;
	pData[nDataLen++] = nData + 2;
	memcpy(pData + nDataLen, szData, nData);
	nDataLen += nData;
	// body sid
	pData[nDataLen++] = ATR_SID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSid);
	nDataLen += 32;
	// body appid
	pData[nDataLen++] = ATR_APPID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strAppId);
	nDataLen += 32;
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// body sig
	pData[nDataLen++] = ATR_SIG;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, md5Data);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	ResetEvent(m_hEvent[0]);
	ResetEvent(m_hEvent[1]);
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
		if (dwResult == WAIT_OBJECT_0)
		{
			bResult = true;
		}
		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			bResult = false;
		}
		if (dwResult == WAIT_TIMEOUT)
		{
			bResult = false;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendBind(DWORD sessionid)
{
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_BIND_SESSION_REQ;
	head.msg_sn = m_nSn++;
	head.msg_time = GetTickCount();
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body ip
	pData[nDataLen++] = ATR_IP;
	pData[nDataLen++] = 6;
	if (m_strLocalIP != "")
	{
		CStringA str;
		int nStart = 0;
		// ȡ��1λ
		int nStop = m_strLocalIP.Find(".", nStart);
		if (nStop > 0)
		{
			str = m_strLocalIP.Mid(nStart, nStop - nStart);
			pData[nDataLen++] = atoi(str);
			// ȡ��2λ
			nStart = nStop + 1;
			nStop = m_strLocalIP.Find(".", nStart);
			if (nStop > 0)
			{
				str = m_strLocalIP.Mid(nStart, nStop - nStart);
				pData[nDataLen++] = atoi(str);
				// ȡ��3λ
				nStart = nStop + 1;
				nStop = m_strLocalIP.Find(".", nStart);
				if (nStop > 0)
				{
					str = m_strLocalIP.Mid(nStart, nStop - nStart);
					pData[nDataLen++] = atoi(str);
					// ȡ��4λ
					nStart = nStop + 1;
					str = m_strLocalIP.Mid(nStart, m_strLocalIP.GetLength() - nStart);
					pData[nDataLen++] = atoi(str);
				}
			}
		}
	}
	// body aplist
	pData[nDataLen++] = ATR_APLISTSTATUS;
	pData[nDataLen++] = 2 + (BYTE)(m_vtAPList.size() * 7);
	for (int i = 0; i < (int)m_vtAPList.size(); i++)
	{
		IPORT *pNode = m_vtAPList[i];
		pData[nDataLen++] = pNode->nIP[0];
		pData[nDataLen++] = pNode->nIP[1];
		pData[nDataLen++] = pNode->nIP[2];
		pData[nDataLen++] = pNode->nIP[3];
		pData[nDataLen++] = BYTE((pNode->nDelay & 0xFF00) >> 16);
		pData[nDataLen++] = BYTE(pNode->nDelay & 0xFF);
		pData[nDataLen++] = pNode->nLost;
	}
	// body sessionid
	pData[nDataLen++] = ATR_SESSIONID;
	pData[nDataLen++] = 2 + sizeof(DWORD);
	memcpy(pData + nDataLen, &sessionid, sizeof(DWORD));
	nDataLen += sizeof(DWORD);
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	ResetEvent(m_hEvent[0]);
	ResetEvent(m_hEvent[1]);
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
		if (dwResult == WAIT_OBJECT_0)
		{
			bResult = true;
		}
		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			bResult = false;
		}
		if (dwResult == WAIT_TIMEOUT)
		{
			bResult = false;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendListen(DWORD sessionid)
{
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_LISTEN_SESSION_REQ;
	head.msg_sn = m_nSn++;
	head.msg_time = GetTickCount();
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body sessionid
	pData[nDataLen++] = ATR_SESSIONID;
	pData[nDataLen++] = 2 + sizeof(DWORD);
	memcpy(pData + nDataLen, &sessionid, sizeof(DWORD));
	nDataLen += sizeof(DWORD);
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	ResetEvent(m_hEvent[0]);
	ResetEvent(m_hEvent[1]);
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
		if (dwResult == WAIT_OBJECT_0)
		{
			bResult = true;
		}
		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			bResult = false;
		}
		if (dwResult == WAIT_TIMEOUT)
		{
			bResult = false;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendRelease(DWORD sessionid)
{
	MD5 md5;
	CStringA md5Data = "";
	CStringA strTime = GetTimeStr();
	CStringA strSig = m_strSid + m_strToken + strTime;
	// MD5����
	md5.reset();
	md5.update((LPCSTR)strSig, strSig.GetLength());
	md5Data.Format("%s", md5.toString().c_str());
	md5Data.MakeUpper();
	// BASE64����
	int nData = MAX_PATH;
	char szData[MAX_PATH] = { 0 };
	CStringA strAuthen = m_strSid + ":" + strTime;
	base64_encode((const BYTE *)(LPCSTR)strAuthen, strAuthen.GetLength(), (BYTE *)szData, &nData);
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	memset(pData, 0, 1024);
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_DISCONN_SESSION_REQ;
	head.msg_sn = m_nSn++;
	head.msg_time = GetTickCount();
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body authen
	pData[nDataLen++] = ATR_AUTHEN;
	pData[nDataLen++] = nData + 2;
	memcpy(pData + nDataLen, szData, nData);
	nDataLen += nData;
	// body sid
	pData[nDataLen++] = ATR_SID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSid);
	nDataLen += 32;
	// body appid
	pData[nDataLen++] = ATR_APPID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strAppId);
	nDataLen += 32;
	// body sessionid
	pData[nDataLen++] = ATR_SESSIONID;
	pData[nDataLen++] = 2 + sizeof(DWORD);
	memcpy(pData + nDataLen, &sessionid, sizeof(DWORD));
	nDataLen += sizeof(DWORD);
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// body sig
	pData[nDataLen++] = ATR_SIG;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, md5Data);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	ResetEvent(m_hEvent[0]);
	ResetEvent(m_hEvent[1]);
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
		if (dwResult == WAIT_OBJECT_0)
		{
			bResult = true;
		}
		if (dwResult == WAIT_OBJECT_0 + 1)
		{
			bResult = false;
		}
		if (dwResult == WAIT_TIMEOUT)
		{
			bResult = false;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			DWORD dwResult = ::WaitForMultipleObjects(2, m_hEvent, FALSE, 2000);
			if (dwResult == WAIT_OBJECT_0)
			{
				bResult = true;
			}
			if (dwResult == WAIT_OBJECT_0 + 1)
			{
				bResult = false;
			}
			if (dwResult == WAIT_TIMEOUT)
			{
				bResult = false;
			}
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendReleaseRsp(DWORD sessionid, BYTE sn, DWORD time)
{
	MD5 md5;
	CStringA md5Data = "";
	CStringA strTime = GetTimeStr();
	CStringA strSig = m_strSid + m_strToken + strTime;
	// MD5����
	md5.reset();
	md5.update((LPCSTR)strSig, strSig.GetLength());
	md5Data.Format("%s", md5.toString().c_str());
	md5Data.MakeUpper();
	// BASE64����
	int nData = MAX_PATH;
	char szData[MAX_PATH] = { 0 };
	CStringA strAuthen = m_strSid + ":" + strTime;
	base64_encode((const BYTE *)(LPCSTR)strAuthen, strAuthen.GetLength(), (BYTE *)szData, &nData);
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	memset(pData, 0, 1024);
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_DISCONN_SESSION_RSP;
	head.msg_sn = sn;
	head.msg_time = time;
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body authen
	pData[nDataLen++] = ATR_AUTHEN;
	pData[nDataLen++] = nData + 2;
	memcpy(pData + nDataLen, szData, nData);
	nDataLen += nData;
	// body sessionid
	pData[nDataLen++] = ATR_SESSIONID;
	pData[nDataLen++] = 2 + sizeof(DWORD);
	memcpy(pData + nDataLen, &sessionid, sizeof(DWORD));
	nDataLen += sizeof(DWORD);
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// body sig
	pData[nDataLen++] = ATR_SIG;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, md5Data);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	ResetEvent(m_hEvent[0]);
	ResetEvent(m_hEvent[1]);
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		bResult = true;
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			bResult = true;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			bResult = true;
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendConnect(DWORD sessionid, char *peersdkid)
{
	MD5 md5;
	CStringA md5Data = "";
	CStringA strTime = GetTimeStr();
	CStringA strSig = m_strSid + m_strToken + strTime;
	// MD5����
	md5.reset();
	md5.update((LPCSTR)strSig, strSig.GetLength());
	md5Data.Format("%s", md5.toString().c_str());
	md5Data.MakeUpper();
	// BASE64����
	int nData = MAX_PATH;
	char szData[MAX_PATH] = { 0 };
	CStringA strAuthen = m_strSid + ":" + strTime;
	base64_encode((const BYTE *)(LPCSTR)strAuthen, strAuthen.GetLength(), (BYTE *)szData, &nData);
	// ���
	int nDataLen = 0;
	char *pData = new char[1024];
	memset(pData, 0, 1024);
	// ��ͷ
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	head.msg_type = MSG_CONNECT_SESSION;
	head.msg_sn = m_nSn++;
	head.msg_time = GetTickCount();
	head.msg_len = 0;
	nDataLen += nHeadLen;
	// body authen
	pData[nDataLen++] = ATR_AUTHEN;
	pData[nDataLen++] = nData + 2;
	memcpy(pData + nDataLen, szData, nData);
	nDataLen += nData;
	// body sid
	pData[nDataLen++] = ATR_SID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSid);
	nDataLen += 32;
	// body appid
	pData[nDataLen++] = ATR_APPID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strAppId);
	nDataLen += 32;
	// body sessionid
	pData[nDataLen++] = ATR_SESSIONID;
	pData[nDataLen++] = 2 + sizeof(DWORD);
	memcpy(pData + nDataLen, &sessionid, sizeof(DWORD));
	nDataLen += sizeof(DWORD);
	// body sdkid
	pData[nDataLen++] = ATR_SDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, m_strSdkId);
	nDataLen += 32;
	// body sig
	pData[nDataLen++] = ATR_SIG;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, md5Data);
	nDataLen += 32;
	// body peersdkid
	pData[nDataLen++] = ATR_PEERSDKID;
	pData[nDataLen++] = 34;
	strcpy(pData + nDataLen, peersdkid);
	nDataLen += 32;
	// head len
	head.msg_len = htons(nDataLen - nHeadLen);
	memcpy(pData, &head, nHeadLen);

	bool bResult = false;
	if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
	{
		bResult = true;
	}

	if (!bResult)
	{
		// ���ɹ�����������2��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			bResult = true;
		}
	}

	if (!bResult)
	{
		// ���ɹ�����������3��
		if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
		{
			bResult = true;
		}
	}

	delete[]pData;
	return bResult;
}

bool flySession::SendAccept(DWORD sessionid)
{
	bool bResult = false;
	map<DWORD, CLIENT*>::iterator iter = m_mpClient.find(sessionid);
	if (iter != m_mpClient.end())
	{
		CLIENT *pClient = iter->second;
		if (pClient != NULL)
		{
			MD5 md5;
			CStringA md5Data = "";
			CStringA strTime = GetTimeStr();
			CStringA strSig = m_strSid + m_strToken + strTime;
			// MD5����
			md5.reset();
			md5.update((LPCSTR)strSig, strSig.GetLength());
			md5Data.Format("%s", md5.toString().c_str());
			md5Data.MakeUpper();
			// BASE64����
			int nData = MAX_PATH;
			char szData[MAX_PATH] = { 0 };
			CStringA strAuthen = m_strSid + ":" + strTime;
			base64_encode((const BYTE *)(LPCSTR)strAuthen, strAuthen.GetLength(), (BYTE *)szData, &nData);
			// ���
			int nDataLen = 0;
			char *pData = new char[1024];
			memset(pData, 0, 1024);
			// ��ͷ
			MSG_HEAD head;
			int nHeadLen = sizeof(MSG_HEAD);
			head.msg_type = MSG_ACCEPT_SESSION;
			head.msg_sn = pClient->m_nPeerSn;
			head.msg_time = pClient->m_dwPeerTime;
			head.msg_len = 0;
			nDataLen += nHeadLen;
			// body authen
			pData[nDataLen++] = ATR_AUTHEN;
			pData[nDataLen++] = nData + 2;
			memcpy(pData + nDataLen, szData, nData);
			nDataLen += nData;
			// body sessionid
			pData[nDataLen++] = ATR_SESSIONID;
			pData[nDataLen++] = 2 + sizeof(DWORD);
			memcpy(pData + nDataLen, &sessionid, sizeof(DWORD));
			nDataLen += sizeof(DWORD);
			// body sdkid
			pData[nDataLen++] = ATR_SDKID;
			pData[nDataLen++] = 34;
			strcpy(pData + nDataLen, pClient->m_strPeerSdkid);
			nDataLen += 32;
			// body peersdkid
			pData[nDataLen++] = ATR_PEERSDKID;
			pData[nDataLen++] = 34;
			strcpy(pData + nDataLen, pClient->m_strPeerSdkPeerId);
			nDataLen += 32;
			// head len
			head.msg_len = htons(nDataLen - nHeadLen);
			memcpy(pData, &head, nHeadLen);

			if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
			{
				bResult = true;
			}

			if (!bResult)
			{
				// ���ɹ�����������2��
				if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
				{
					bResult = true;
				}
			}

			if (!bResult)
			{
				// ���ɹ�����������3��
				if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
				{
					bResult = true;
				}
			}
			delete[]pData;
		}
	}
	return bResult;
}

bool flySession::SendReject(DWORD sessionid)
{
	bool bResult = false;
	map<DWORD, CLIENT*>::iterator iter = m_mpClient.find(sessionid);
	if (iter != m_mpClient.end())
	{
		CLIENT *pClient = iter->second;
		if (pClient != NULL)
		{
			MD5 md5;
			CStringA md5Data = "";
			CStringA strTime = GetTimeStr();
			CStringA strSig = m_strSid + m_strToken + strTime;
			// MD5����
			md5.reset();
			md5.update((LPCSTR)strSig, strSig.GetLength());
			md5Data.Format("%s", md5.toString().c_str());
			md5Data.MakeUpper();
			// BASE64����
			int nData = MAX_PATH;
			char szData[MAX_PATH] = { 0 };
			CStringA strAuthen = m_strSid + ":" + strTime;
			base64_encode((const BYTE *)(LPCSTR)strAuthen, strAuthen.GetLength(), (BYTE *)szData, &nData);
			// ���
			int nDataLen = 0;
			char *pData = new char[1024];
			memset(pData, 0, 1024);
			// ��ͷ
			MSG_HEAD head;
			int nHeadLen = sizeof(MSG_HEAD);
			head.msg_type = MSG_REJECT_SESSION;
			head.msg_sn = pClient->m_nPeerSn;
			head.msg_time = pClient->m_dwPeerTime;
			head.msg_len = 0;
			nDataLen += nHeadLen;
			// body authen
			pData[nDataLen++] = ATR_AUTHEN;
			pData[nDataLen++] = nData + 2;
			memcpy(pData + nDataLen, szData, nData);
			nDataLen += nData;
			// body sessionid
			pData[nDataLen++] = ATR_SESSIONID;
			pData[nDataLen++] = 2 + sizeof(DWORD);
			memcpy(pData + nDataLen, &sessionid, sizeof(DWORD));
			nDataLen += sizeof(DWORD);
			// body sdkid
			pData[nDataLen++] = ATR_SDKID;
			pData[nDataLen++] = 34;
			strcpy(pData + nDataLen, pClient->m_strPeerSdkid);
			nDataLen += 32;
			// body peersdkid
			pData[nDataLen++] = ATR_PEERSDKID;
			pData[nDataLen++] = 34;
			strcpy(pData + nDataLen, pClient->m_strPeerSdkPeerId);
			nDataLen += 32;
			// head len
			head.msg_len = htons(nDataLen - nHeadLen);
			memcpy(pData, &head, nHeadLen);

			if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
			{
				bResult = true;
			}

			if (!bResult)
			{
				// ���ɹ�����������2��
				if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
				{
					bResult = true;
				}
			}

			if (!bResult)
			{
				// ���ɹ�����������3��
				if (m_cClient.Send((char *)gSManageIP, gSManagePort, pData, nDataLen))
				{
					bResult = true;
				}
			}
			delete[]pData;
		}
	}
	return bResult;
}

void flySession::OnRecvData(unsigned char *pData, int nSize)
{
	if (nSize < sizeof(MSG_HEAD))
	{
		return;
	}
	// ��ȡ��ͷ����
	MSG_HEAD head;
	int nHeadLen = sizeof(MSG_HEAD);
	memcpy(&head, pData, nHeadLen);
	// ��ӡ���
	CStringA str;
	str.Format("OnRecvData msg_type = %d, msg_sn = %d\r\n", head.msg_type, head.msg_sn);
	OutputDebugStringA(str);
	// ��������
	switch (head.msg_type)
	{
	case MSG_REGISTER_RSP:
		// ��¼��Ӧ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			SetEvent(m_hEvent[1]);
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				SetEvent(m_hEvent[1]);
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_REGISTER_RSP Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_REGISTER_RSP retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);

				if (nRetCode != 0)
				{
					// �����쳣
					SetEvent(m_hEvent[1]);
					return;
				}
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_REGISTER_RSP sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_APLIST)
			{
				// AP�б�����PING����
				m_vtAPList.clear();
				int nNum = (stTlv.usLen) / 6;
				for (int i = 0; i < nNum; i++)
				{
					CStringA strIp;
					strIp.Format("%d.%d.%d.%d", stTlv.pValue[i * 6 + 0], stTlv.pValue[i * 6 + 1], stTlv.pValue[i * 6 + 2], stTlv.pValue[i * 6 + 3]);
					int nPort = stTlv.pValue[i * 6 + 4] * 256 + stTlv.pValue[i * 6 + 5];

					IPORT *pstIPort = new IPORT();
					strcpy(pstIPort->szIp, strIp);
					pstIPort->nIP[0] = stTlv.pValue[i * 6 + 0];
					pstIPort->nIP[1] = stTlv.pValue[i * 6 + 1];
					pstIPort->nIP[2] = stTlv.pValue[i * 6 + 2];
					pstIPort->nIP[3] = stTlv.pValue[i * 6 + 3];
					pstIPort->nPort = nPort;
					pstIPort->nDelay = 3000;
					pstIPort->nLost = 0;
					m_vtAPList.push_back(pstIPort);

					str.Format("OnRecvData MSG_REGISTER_RSP ap%d Ip = %s\r\n", i + 1, strIp);
					OutputDebugStringA(str);
					str.Format("OnRecvData MSG_REGISTER_RSP ap%d Port = %d\r\n", i + 1, nPort);
					OutputDebugStringA(str);
				}
				m_bPing = true;
			}
		}
		SetEvent(m_hEvent[0]);
	}
	break;
	case MSG_HB_RSP:
		// ������Ӧ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			SetEvent(m_hEvent[1]);
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				SetEvent(m_hEvent[1]);
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_HB_RSP Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_HB_RSP retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_HB_RSP sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		SetEvent(m_hEvent[0]);
	}
	break;
	case MSG_BIND_SESSION_RSP:
		// �󶨻Ự��Ӧ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			SetEvent(m_hEvent[1]);
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				SetEvent(m_hEvent[1]);
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_BIND_SESSION_RSP Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_BIND_SESSION_RSP retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);

				if (nRetCode != 0)
				{
					// �����쳣
					SetEvent(m_hEvent[1]);
					return;
				}
			}
			if (stTlv.ulTag == ATR_SESSIONID)
			{
				unsigned int nSessionId = 0;
				memcpy(&nSessionId, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_BIND_SESSION_RSP sessionid = 0x%x\r\n", nSessionId);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_BIND_SESSION_RSP sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		SetEvent(m_hEvent[0]);
	}
	break;
	case MSG_DISCONN_SESSION_REQ:
		// �ͷŻỰ������Ϣ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_DISCONN_SESSION_REQ Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_SESSIONID)
			{
				unsigned int nSessionId = 0;
				memcpy(&nSessionId, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_DISCONN_SESSION_REQ sessionid = 0x%x\r\n", nSessionId);
				OutputDebugStringA(str);

				// �Ƴ�
				ReleaseSessionRsp((void*)nSessionId, head.msg_sn, head.msg_time);
			}
		}
	}
	break;
	case MSG_DISCONN_SESSION_RSP:
		// �ͷŻỰ������Ϣ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			SetEvent(m_hEvent[1]);
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				SetEvent(m_hEvent[1]);
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_DISCONN_SESSION_RSP Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_DISCONN_SESSION_RSP retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);
				/*
				if (nRetCode != 0)
				{
					// �����쳣
					SetEvent(m_hEvent[1]);
					return;
				}*/
			}
			if (stTlv.ulTag == ATR_SESSIONID)
			{
				unsigned int nSessionId = 0;
				memcpy(&nSessionId, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_DISCONN_SESSION_RSP sessionid = 0x%x\r\n", nSessionId);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_DISCONN_SESSION_RSP sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		SetEvent(m_hEvent[0]);
	}
	break;
	case MSG_LISTEN_SESSION_RSP:
		// �Ự������Ӧ��Ϣ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			SetEvent(m_hEvent[1]);
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				SetEvent(m_hEvent[1]);
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_LISTEN_SESSION_RSP Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_LISTEN_SESSION_RSP retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);

				if (nRetCode != 0)
				{
					// �����쳣
					SetEvent(m_hEvent[1]);
					return;
				}
			}
			if (stTlv.ulTag == ATR_SESSIONID)
			{
				unsigned int nSessionId = 0;
				memcpy(&nSessionId, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_LISTEN_SESSION_RSP sessionid = 0x%x\r\n", nSessionId);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_LISTEN_SESSION_RSP sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		SetEvent(m_hEvent[0]);
	}
	break;
	case MSG_CONNECT_SESSION:
		// �յ���������
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			return;
		}
		// �����ͻ��˶���
		int nUsePort = 0;
		DWORD dwKey = 0;
		CLIENT *pClient = new CLIENT();
		pClient->m_nPeerSn = head.msg_sn;
		pClient->m_dwPeerTime = head.msg_time;
		// ��������
		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				delete pClient;
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_CONNECT_SESSION Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_SESSIONID)
			{
				// ���ص�sessionid
				DWORD dwSessionId = 0;
				memcpy(&dwSessionId, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_CONNECT_SESSION sessionid = 0x%x\r\n", dwSessionId);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_APINFO)
			{
				// ����ı��ؽ����,����PING����
				if (m_dwSessionId != 0)
				{
					// �����������
					flyData *pData = new flyData();
					pData->nProtocol = PROTOCOL_UDP;
					pData->pSession = this;
					// ��ʼ��
					pData->Init();
					// ����MP;
					dwKey = m_dwSessionId++;
					pData->dwSessionId = dwKey;
					m_mpSession[dwKey] = pData;
					m_mpClient[dwKey] = pClient;

					int nIpIndex = 0;
					int nPortIndex = 0;
					int nPosLen = 0;
					while (nPosLen < stTlv.usLen)
					{
						// ����1��IP
						TLV stTlvSub = { 0 };
						stTlvSub.ulTag = stTlv.pValue[nPosLen++];
						stTlvSub.usLen = stTlv.pValue[nPosLen++] - 2;
						if (stTlvSub.usLen + nPosLen > stTlv.usLen)
						{
							// ���ݳ����쳣
							return;
						}
						memcpy(stTlvSub.pValue, stTlv.pValue + nPosLen, stTlvSub.usLen);
						nPosLen += (stTlvSub.usLen);

						if (stTlvSub.ulTag == 1)
						{
							// �����IP
							if (nIpIndex == 0)
							{
								sprintf(pData->szIp1, "%d.%d.%d.%d", stTlvSub.pValue[0], stTlvSub.pValue[1], stTlvSub.pValue[2], stTlvSub.pValue[3]);
								nIpIndex++;

								str.Format("OnRecvData MSG_CONNECT_SESSION ap srcIp1 = %s\r\n", pData->szIp1);
								OutputDebugStringA(str);
							}
							else if (nIpIndex == 1)
							{
								sprintf(pData->szIp2, "%d.%d.%d.%d", stTlvSub.pValue[0], stTlvSub.pValue[1], stTlvSub.pValue[2], stTlvSub.pValue[3]);
								nIpIndex++;

								str.Format("OnRecvData MSG_CONNECT_SESSION ap srcIp2 = %s\r\n", pData->szIp2);
								OutputDebugStringA(str);
							}
						}
						if (stTlvSub.ulTag == 2)
						{
							pData->nPort[nPortIndex] = stTlvSub.pValue[0] * 256 + stTlvSub.pValue[1];
							str.Format("OnRecvData MSG_CONNECT_SESSION ap srcPort%d = %d\r\n", nPortIndex + 1, pData->nPort[nPortIndex]);
							OutputDebugStringA(str);
							nPortIndex++;
						}
					}
					// ��¼���õĶ˿���
					pData->nUse = nPortIndex;
					nUsePort = nPortIndex;
					// ����̽���
					SendNatPack(pData);
				}
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				// ����sdkid
				strcpy(pClient->m_strPeerSdkid, (const char*)stTlv.pValue);

				str.Format("OnRecvData MSG_CONNECT_SESSION sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_PEERSDKID)
			{
				// ����sdkid
				strcpy(pClient->m_strPeerSdkPeerId, (const char*)stTlv.pValue);

				str.Format("OnRecvData MSG_CONNECT_SESSION peersdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		// �ص�״̬
		for (size_t i = 0; i < m_vtCallBack.size(); i++)
		{
			m_vtCallBack[i]->event_cb(EVT_SESSION_INCOMING, nUsePort, (void *)dwKey);
		}
	}
	break;
	case MSG_ACCEPT_SESSION:
		// �Ự������Ӧ��Ϣ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			return;
		}
		// ����
		int nUsePort = 0;
		DWORD dwSessionId = 0;

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_ACCEPT_SESSION Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_ACCEPT_SESSION retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SESSIONID)
			{
				// ���ص�sessionid
				memcpy(&dwSessionId, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_ACCEPT_SESSION sessionid = 0x%x\r\n", dwSessionId);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_APINFO)
			{
				// ����ı��ؽ����,����PING����
				if (dwSessionId != 0)
				{
					// ��ѯMP
					map<DWORD, flyData*>::iterator iter = m_mpSession.find(dwSessionId);
					if (iter != m_mpSession.end())
					{
						// �ҵ���Ӧ��Session����
						flyData *pData = iter->second;
						if (pData != NULL)
						{
							int nIpIndex = 0;
							int nPortIndex = 0;
							int nPosLen = 0;
							while (nPosLen < stTlv.usLen)
							{
								// ����1��IP
								TLV stTlvSub = { 0 };
								stTlvSub.ulTag = stTlv.pValue[nPosLen++];
								stTlvSub.usLen = stTlv.pValue[nPosLen++] - 2;
								if (stTlvSub.usLen + nPosLen > stTlv.usLen)
								{
									// ���ݳ����쳣
									return;
								}
								memcpy(stTlvSub.pValue, stTlv.pValue + nPosLen, stTlvSub.usLen);
								nPosLen += (stTlvSub.usLen);

								if (stTlvSub.ulTag == 1)
								{
									// �����IP
									if (nIpIndex == 0)
									{
										sprintf(pData->szIp1, "%d.%d.%d.%d", stTlvSub.pValue[0], stTlvSub.pValue[1], stTlvSub.pValue[2], stTlvSub.pValue[3]);
										nIpIndex++;

										str.Format("OnRecvData MSG_ACCEPT_SESSION ap srcIp1 = %s\r\n", pData->szIp1);
										OutputDebugStringA(str);
									}
									else if (nIpIndex == 1)
									{
										sprintf(pData->szIp2, "%d.%d.%d.%d", stTlvSub.pValue[0], stTlvSub.pValue[1], stTlvSub.pValue[2], stTlvSub.pValue[3]);
										nIpIndex++;

										str.Format("OnRecvData MSG_ACCEPT_SESSION ap srcIp2 = %s\r\n", pData->szIp2);
										OutputDebugStringA(str);
									}
								}
								if (stTlvSub.ulTag == 2)
								{
									pData->nPort[nPortIndex] = stTlvSub.pValue[0] * 256 + stTlvSub.pValue[1];
									str.Format("OnRecvData MSG_ACCEPT_SESSION ap srcPort%d = %d\r\n", nPortIndex + 1, pData->nPort[nPortIndex]);
									OutputDebugStringA(str);
									nPortIndex++;
								}
							}
							// ��¼���õĶ˿���
							pData->nUse = nPortIndex;
							nUsePort = nPortIndex;
							// ����̽���
							SendNatPack(pData);
						}
					}
				}
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_ACCEPT_SESSION sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		// �ص�״̬
		for (size_t i = 0; i < m_vtCallBack.size(); i++)
		{
			m_vtCallBack[i]->event_cb(EVT_SESSION_ACCEPT, nUsePort, NULL);
		}
	}
	break;
	case MSG_REJECT_SESSION:
		// �Ự������Ӧ��Ϣ
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_REJECT_SESSION Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_REJECT_SESSION retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SESSIONID)
			{
				unsigned int nSessionId = 0;
				memcpy(&nSessionId, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_REJECT_SESSION sessionid = 0x%x\r\n", nSessionId);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_REJECT_SESSION sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		// �ص�״̬
		for (size_t i = 0; i < m_vtCallBack.size(); i++)
		{
			m_vtCallBack[i]->event_cb(EVT_SESSION_REJECT, 0, NULL);
		}
	}
	break;
	case MSG_UNREGISTER_RSP:
	{
		// ������Ϣ�峤��
		int nBodyLen = ntohs(head.msg_len);
		if (nBodyLen + nHeadLen > nSize)
		{
			// ���ݳ����쳣
			SetEvent(m_hEvent[1]);
			return;
		}

		int nPos = 0;
		unsigned char *pBody = pData + nHeadLen;
		while (nPos < nBodyLen)
		{
			TLV stTlv = { 0 };
			stTlv.ulTag = pBody[nPos++];
			stTlv.usLen = pBody[nPos++] - 2;
			if (stTlv.usLen + nPos > nBodyLen)
			{
				// ���ݳ����쳣
				SetEvent(m_hEvent[1]);
				return;
			}
			memcpy(stTlv.pValue, pBody + nPos, stTlv.usLen);
			nPos += (stTlv.usLen);

			// ��ӡ���
			str.Format("OnRecvData MSG_UNREGISTER_RSP Tag = %d, Len = %d\r\n", stTlv.ulTag, stTlv.usLen);
			OutputDebugStringA(str);

			// ����TLV����
			if (stTlv.ulTag == ATR_RETCODE)
			{
				// retCode
				unsigned int nRetCode = 0;
				memcpy(&nRetCode, stTlv.pValue, stTlv.usLen);

				str.Format("OnRecvData MSG_UNREGISTER_RSP retCode = %d\r\n", nRetCode);
				OutputDebugStringA(str);
			}
			if (stTlv.ulTag == ATR_SDKID)
			{
				str.Format("OnRecvData MSG_UNREGISTER_RSP sdkid = %s\r\n", stTlv.pValue);
				OutputDebugStringA(str);
			}
		}
		SetEvent(m_hEvent[0]);
	}
	break;
	default:
		break;
	}
}

DWORD WINAPI flySession::WorkThread(LPVOID lpParam)
{
	flySession *pSession = (flySession *)lpParam;

	static int nCount = 1;
	static int nHeatCount = 0;
	while (1)
	{
		// ��ȡ����IP
		if (pSession->m_bLocalIp)
		{
			pSession->GetLocalIp();
			pSession->m_bLocalIp = false;
		}
		// ��¼����
		if (pSession->m_nStatus == 0)
		{
			// ��¼,ʧ�ܺ�30�����
			nCount = 30;
			if (pSession->SendLogin())
			{
				pSession->m_nStatus = 1;
				// �ص��ϲ����ӳɹ�
				for (size_t i = 0; i < pSession->m_vtCallBack.size(); i++)
				{
					pSession->m_vtCallBack[i]->event_cb(EVT_REGISTER_SUCCUSS, 0, NULL);
				}
			}
		}
		if (pSession->m_nStatus == 1)
		{
			if (pSession->SendHeat())
			{
				nCount = 60;
				nHeatCount = 0;
			}
			else
			{
				nCount = 3;
				nHeatCount++;
			}
			// 3��ʧ�ܾ��˳�
			if (nHeatCount == 3)
			{
				nCount = 3;
				nHeatCount = 0;
				pSession->m_nStatus = 0;
				// �ص��ϲ����ӶϿ�
				for (size_t i = 0; i < pSession->m_vtCallBack.size(); i++)
				{
					pSession->m_vtCallBack[i]->event_cb(EVT_REGISTER_FAILURE, 0, NULL);
				}
			}
			// ��ȡAP��Delay
			if (pSession->m_bPing)
			{
				pSession->GetAPDelay();
				pSession->m_bPing = false;
			}
		}

		if (::WaitForSingleObject(pSession->m_hWorkEvent, 1000 * nCount) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

DWORD WINAPI flySession::RecvThread(LPVOID lpParam)
{
	flySession *pSession = (flySession *)lpParam;
	while (1)
	{
		char szIP[256] = { 0 };
		int nPort = 0;
		char szRead[2048] = { 0 };
		int nSize = 2048;
		if (pSession->m_cClient.Receive(szIP, nPort, szRead, nSize))
		{
			pSession->OnRecvData((unsigned char *)szRead, nSize);
		}

		if (::WaitForSingleObject(pSession->m_hRecvEvent, 100) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

CStringA flySession::GetTimeStr()
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	int year, month, day, hour, min, sec;
	year = 1900 + timeinfo->tm_year;
	month = 1 + timeinfo->tm_mon;
	day = timeinfo->tm_mday;
	hour = timeinfo->tm_hour;
	min = timeinfo->tm_min;
	sec = timeinfo->tm_sec;
	CStringA str;
	str.Format("%4d%02d%02d%02d%02d%02d", year, month, day, hour, min, sec);
	return str;
}

void flySession::GetLocalIp()
{
	CStringA str = "";
	CStringA straResult = "";
	CString strUrl;
	strUrl.Format(L"%s", gGetIpUrl);
	// ����HTTP����
	HttpTool mHttpTool;
	OutputDebugStringA("GetLocalIp start\r\n");
	if (mHttpTool.HttpGet(strUrl, str, straResult))
	{
		std::string src;
		src.append(straResult);
		// ��ӡ����
		CStringA strTmp;
		strTmp.Format("GetLocalIp resp = %s\r\n", straResult);
		OutputDebugStringA(strTmp);
		// ��������
		try
		{
			Json json(src);
			JsonDict *pRoot = json.getRoot()->toDict();
			if (pRoot != NULL)
			{
				if (pRoot->contain("ipaddr"))
				{
					JsonValue *pValue = pRoot->getValue("ipaddr");
					std::string dst;
					pValue->writeToStr(dst);
					// ���汾�ع���IP
					m_strLocalIP.Format("%s", dst.c_str());
					m_strLocalIP.Replace("\"", "");
				}
			}
		}
		catch (Exception e)
		{
			// ��ӡ����
			strTmp.Format("GetLocalIp fail = %s\r\n", e.getDesc().c_str());
			OutputDebugStringA(strTmp);
		}
	}
}

void flySession::GetAPDelay()
{
	CStringA str;
	str.Format("GetAPDelay start ap count = %d\r\n", m_vtAPList.size());
	OutputDebugStringA(str);
	// ��ʼ��udp
	UdpBase cClient;
	cClient.Init();
	// ����AP�б�
	for (int i = 0; i < (int)m_vtAPList.size(); i++)
	{
		IPORT *pIPort = m_vtAPList[i];
		int nLostCount = 0;
		int nSendCount = 10;
		vector<int> vtTime;
		// ����10�ΰ������㶪���ʺ���ʱ
		for (int j = 0; j < nSendCount; j++)
		{
			// ���㿪ʼʱ��
			int nStart = ::GetTickCount();
			// �������ݰ�
			char szTmp[8] = "";
			sprintf(szTmp, "pong%d", j);
			cClient.Send(pIPort->szIp, pIPort->nPort, szTmp, strlen(szTmp));
			// ����������Ӧ
			char szIP[16] = { 0 };
			int nPort = 0;
			char szRecv[8] = { 0 };
			int nSize = 8;
			if (cClient.Recv(szIP, nPort, szRecv, nSize, 3, 0))
			{
				// �������ʱ��
				int nStop = ::GetTickCount();
				// ����RTT
				vtTime.push_back(nStop - nStart);
			}
			else
			{
				// ��ʱ
				vtTime.push_back(0);
				// ���Ӷ���
				nLostCount++;
			}
		}
		// ���㶪����
		pIPort->nLost = nLostCount * 100 / nSendCount;
		// ����ƽ��RTT
		if (nLostCount >= 10)
		{
			pIPort->nDelay = 3000;
		}
		else if (nLostCount <= 0)
		{
			int nNum = 0;
			for (int k = 0; k < (int)vtTime.size(); k++)
			{
				nNum += vtTime[k];
			}

			if (nNum > 0)
			{
				pIPort->nDelay = nNum / nSendCount;
			}
			else
			{
				pIPort->nDelay = 3000;
			}
		}
		else
		{
			int nNum = 0;
			for (int k = 0; k < (int)vtTime.size(); k++)
			{
				nNum += vtTime[k];
			}

			if (nNum > 0)
			{
				pIPort->nDelay = nNum / (nSendCount - nLostCount);
			}
			else
			{
				pIPort->nDelay = 3000;
			}
		}
		// ��ʱ������һ��AP
		Sleep(100);
		// ��ӡ���
		str.Format("GetAPDelay ip = %s, delay = %d, lost = %d\r\n", pIPort->szIp, pIPort->nDelay, pIPort->nLost);
		OutputDebugStringA(str);
	}
	// �ͷ�udp
	cClient.Free();
}

void flySession::SendNatPack(flyData *pData)
{
	if (pData != NULL)
	{
		// �ж�Э������
		if (pData->nProtocol == PROTOCOL_UDP)
		{
			unsigned char szData[16] = { 0xFF,0x55,0xFF,0x55,0xFF,0x77,0xFF,0x77 };
			for (int i = 0; i < pData->nUse; i++)
			{
				if (i > 3)
				{
					pData->udp[i].Send(pData->szIp2, pData->nPort[i], (char*)szData, 8);

					CStringA str;
					str.Format("SendNatPack ip = %s, port = %d\r\n", pData->szIp2, pData->nPort[i]);
					OutputDebugStringA(str);
				}
				else
				{
					pData->udp[i].Send(pData->szIp1, pData->nPort[i], (char*)szData, 8);

					CStringA str;
					str.Format("SendNatPack ip = %s, port = %d\r\n", pData->szIp1, pData->nPort[i]);
					OutputDebugStringA(str);
				}
			}
		}
	}
}