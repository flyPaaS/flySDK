// stdafx.cpp : 只包括标准包含文件的源文件
// flySDK.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

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
