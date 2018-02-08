

#ifndef _CIRCLEBUF_H_
#define _CIRCLEBUF_H_


#include <Foundation/Foundation.h>


class CCircleBuf
{
public:
	CCircleBuf();
	~CCircleBuf();

	bool Create(int size);
	void Release();
	int Read(void* buf, int size);
	int Write(void* buf, int size);
	int GetStock();
	void Reset();

	//这个函数用来特殊用处
	char * ReadOneFrame(int &len);
    //char* ReadOneFrame1(int &len, VIDEO_BUF_HEAD & videobufhead);

private:
	int Read1(void* buf, int size);
		

protected:
	char* m_pBuf;
	int m_nSize;
	int m_nStock;
	int m_nReadPos;
	int m_nWritePos;

	int  m_nTimeout;
    
    NSCondition *m_Lock;

private:

	int m_n;

};

#endif

