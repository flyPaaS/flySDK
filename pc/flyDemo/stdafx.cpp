// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// flyDemo.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������

// ��־����
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