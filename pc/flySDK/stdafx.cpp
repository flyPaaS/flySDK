// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// flySDK.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������

CString StringA2String(CStringA str)
{
	int nDstLen = MultiByteToWideChar(CP_UTF8, 0, LPCSTR(str), -1, NULL, 0);
	wchar_t *dst = new wchar_t[nDstLen];
	memset(dst, 0, nDstLen * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, LPCSTR(str), -1, dst, nDstLen);

	CString strDest = L"";
	strDest.Format(L"%s", dst);
	delete[] dst;
	return strDest;
}

CStringA String2StringA(CString str)
{
	int nDstLen = WideCharToMultiByte(CP_UTF8, 0, LPCWSTR(str), -1, NULL, 0, NULL, NULL);
	char *dst = new char[nDstLen];
	memset(dst, 0, nDstLen * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, LPCWSTR(str), -1, dst, nDstLen, NULL, NULL);

	CStringA strDest = "";
	strDest.Format("%s", dst);
	delete[] dst;
	return strDest;
}

CString StringAToString(CStringA str)
{
	int nDstLen = MultiByteToWideChar(CP_ACP, 0, LPCSTR(str), -1, NULL, 0);
	wchar_t *dst = new wchar_t[nDstLen];
	memset(dst, 0, nDstLen * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(str), -1, dst, nDstLen);

	CString strDest = L"";
	strDest.Format(L"%s", dst);
	delete[] dst;
	return strDest;
}

CStringA StringToStringA(CString str)
{
	int nDstLen = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(str), -1, NULL, 0, NULL, NULL);
	char *dst = new char[nDstLen];
	memset(dst, 0, nDstLen * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(str), -1, dst, nDstLen, NULL, NULL);

	CStringA strDest = "";
	strDest.Format("%s", dst);
	delete[] dst;
	return strDest;
}
