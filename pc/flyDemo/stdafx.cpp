// stdafx.cpp : 只包括标准包含文件的源文件
// flyDemo.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

// 日志开关
bool bDebug = true;
bool bConsole = true;
void Report(LPCSTR format, ...)
{
	if (bDebug)
	{
		char _Buff[8192];
		memset(_Buff, 0, sizeof(_Buff));

		va_list arg;
		va_start(arg, format);
		int charSize = _vsnprintf(_Buff, sizeof(_Buff), format, arg);
		va_end(arg);

		OutputDebugStringA(_Buff);

		if (bConsole)
		{
			printf(_Buff);
		}
	}
}