#pragma once

class HttpTool
{
public:
	HttpTool();
	virtual ~HttpTool();

public:
	// HTTP Get ��ʽ����(UTF-8)
	BOOL HttpGet(CString strUrl, CStringA strData, CStringA &straResult);
	// HTTP Post ��ʽ����(UTF-8)
	BOOL HttpPost(CString strUrl, CStringA strData, CStringA &straResult);

private:
	// ����URL��ȷ��
	BOOL parseURL(LPCWSTR url);

	// ����һ��Session
	HINTERNET OpenSession(LPCWSTR userAgent = 0);
	// �����������
	HINTERNET Connect(HINTERNET hSession, LPCWSTR serverAddr, int portNo);
	// ��������
	HINTERNET OpenRequest(HINTERNET hConnect, LPCWSTR verb, LPCWSTR objectName, int scheme);
	// ����ͷ����
	BOOL AddRequestHeaders(HINTERNET hRequest, LPCWSTR header);
	// ��������
	BOOL SendRequest(HINTERNET hRequest, const void* body, DWORD size, BOOL bGet = TRUE);
	// �������󣬽�������
	BOOL EndRequest(HINTERNET hRequest);
	// ��ȡ����ͷ����
	BOOL ReadHeader(HINTERNET hRequest, CStringA &straHead);
	// ��ȡ��������
	BOOL ReadData(HINTERNET hRequest, CStringA &straData, CStringA &strError);
	// �ر�����
	void CloseInternetHandle(HINTERNET hInternet);

private:
	// ���Ӳ���
	int m_scheme;
	CString m_host;
	int m_port;
	CString m_path;
};

