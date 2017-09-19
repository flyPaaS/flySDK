#pragma once
#include <string>
//////////////////////////////////////////////////////
/// @brief �쳣�������
/// @note  �κ��쳣�Ӹ�������
///////////////////////////////////////////////////////
class Exception
{
protected:
	int m_Time;            ///< @brief ��¼�쳣�¼���
	std::string m_ExcpSrc; ///< @brief ��¼�쳣��Դ
	std::string m_ExcpDesc;///< @brief ��¼�쳣��Ϣ
public:
	int getTime() const;           ///< @brief ����쳣ʱ��
	std::string getSrc() const;    ///< @brief ����쳣��Դ
	std::string getDesc() const;   ///< @brief ����쳣��Ϣ
public:
	/// @brief �չ��캯��
	Exception();
	/// @brief      ���캯��
	/// @warning    �������ó���512���ַ�
	/// @param[in] Src       �쳣Դ
	/// @param[in] DescFmt   �쳣��ʽ
	Exception(std::string Src, const char* DescFmt, ...);
	Exception(const Exception& Org);
	~Exception();
};


//////////////////////////////////////////////////////////
/// @brief   json�����쳣
/// @note    ������Exception,���������ʷ�����
//////////////////////////////////////////////////////////
class ReaderException :public Exception
{
private:
	int m_Line;    ///< @brief �����쳣��������
	int m_Row;     ///< @brief �洢�쳣��������
public:
	int getLine();   ///< @brief ����쳣��������
	int getRow();    ///< @brief ����쳣��������
public:
	/// @brief             ���캯��
	/// @note              ���캯�����Զ����кź��к���ӵ�Desc�ֶ�
	/// @param[in] Src     �쳣Դ
	/// @param[in] Desc    �쳣����
	/// @param[in] Line    �쳣�������к�
	/// @param[in] Row     �쳣��������
	ReaderException(std::string Src, const char* Desc, int Line, int Row);
	~ReaderException();
};
