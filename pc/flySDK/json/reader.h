#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "exception.h"
////////////////////////////////////////////////
/// @brief  �ַ�����ȡ��
////////////////////////////////////////////////
class Reader
{
private:
	std::string m_str;  ///< @brief �����������ַ�����
	int m_pos;          ///< @brief ��ǰָ���ַ����е�λ��
	int m_len;          ///< #brief �ַ����г���
public:
	bool isEOF();     ///< @brief �Ƿ����
	int getLine();    ///< @brief �����к�
	int getRow();     ///< @brief �����к�

	char readChar();  ///< @brief ��ȡһ���ַ�
	char peekChar();  ///< @brief Ԥ��ȡһ���ַ�

	void ignoreSpace();  ///< @brief �����հ��ַ�

	/// @brief ƥ��һ���ַ�
	/// @note  ƥ��һ���ַ���ʧ���������
	/// @param[in] ch Ҫƥ����ַ�
	/// @parampin] bIgnoreSpace ���Կո�
	void match(char ch, bool bIgnoreSpace);

	/// @brief ƥ��һ���ַ���
	/// @note  ƥ��һ���ַ�����ʧ���������
	/// @param[in] str Ҫƥ����ַ���
	/// @param[in] bIgnoreSpace ���Կո�
	void match(std::string str, bool bIgnoreSpace);

	/// @brief ����ƥ��һ���ַ�
	/// @note  ����ƥ��һ���ַ���ʧ���������
	/// @param[in] ch Ҫƥ����ַ�
	/// @param[in] bIgnoreSpace ���Կո�
	/// @param[in] bMatch �����ƥ�䣬�Ƿ�ƥ������ַ�
	/// @return  true=����ƥ�䣬false=ƥ��ʧ��
	bool tryMatch(char ch, bool bIgnoreSpace, bool bMatch);

	/// @brief ����ƥ��һ���ַ���
	/// @note  ����ƥ��һ���ַ�����ʧ���������
	/// @param[in] str Ҫƥ����ַ���
	/// @param[in] bIgnoreSpace ���Կո�
	/// @param[in] bMatch �����ƥ�䣬�Ƿ�ƥ������ַ�
	/// @return  true=����ƥ�䣬false=ƥ��ʧ��
	bool tryMatch(std::string str, bool bIgnoreSpace, bool bMatch);
public:
	/// @brief ���ı�����
	/// @param[in] srcText Ҫ����ƥ���ԭʼ�ַ���
	Reader(const std::string& srcText);

	/// @brief ��������
	/// @param[in] pStream Ҫ����ƥ���ԭʼ������
	Reader(std::ifstream* pStream);
	~Reader();
};

