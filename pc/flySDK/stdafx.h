// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "Base.h"
#include "UdpBase.h"
#include <time.h>
// http
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
// socket
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
// stl
#include <vector>
#include <map>
using namespace std;
// json
#include "./json/exception.h"
#include "./json/reader.h"
#include "./json/json.h"
#pragma comment(lib, "./output/json.lib")

// UNCODE��UTF8ת��
CString StringA2String(CStringA str);
CStringA String2StringA(CString str);
// UNICODE��ANSIת��
CString StringAToString(CStringA str);
CStringA StringToStringA(CString str);