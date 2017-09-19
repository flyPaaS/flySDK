// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>

// TODO:  在此处引用程序需要的其他头文件
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

// UNCODE和UTF8转换
CString StringA2String(CStringA str);
CStringA String2StringA(CString str);
// UNICODE和ANSI转换
CString StringAToString(CStringA str);
CStringA StringToStringA(CString str);