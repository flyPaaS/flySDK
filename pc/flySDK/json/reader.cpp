#include "stdafx.h"
#include "reader.h"

/// @brief ���ı�����
/// @param[in] srcText Ҫ����ƥ���ԭʼ�ַ���
Reader::Reader(const std::string& srcText)
	:m_str(srcText), m_pos(0)
{
	m_len = m_str.size();
}

/// @brief ��������
/// @param[in] pStream Ҫ����ƥ���ԭʼ������
Reader::Reader(std::ifstream* pStream)
	:m_pos(0)
{
	char buf[128];
	if (!pStream)
		throw Exception("Reader::Reader", "Param 'pStream' is null.");
	while (!pStream->eof())
	{
		pStream->getline(buf, 128);
		m_str += buf;
	}
	m_len = m_str.size();
}

Reader::~Reader()
{
}

bool Reader::isEOF()  ///< @brief �Ƿ����
{
	return (m_pos >= m_len);
}

int Reader::getLine()
{
	int tLine = 0;
	char tLast = '\0';
	for (int i = 0; i < m_pos; ++i)
	{
		if (tLast == '\n')
			tLine++;
		tLast = m_str[i];
	}
	return tLine;
}

int Reader::getRow()
{
	int tRow = 0;
	char tLast = '\0';
	for (int i = 0; i < m_pos; ++i)
	{
		if (tLast == '\n')
			tRow = 0;
		tRow++;
		tLast = m_str[i];
	}
	return tRow;
}

char Reader::readChar()  ///< @brief ��ȡһ���ַ�
{
	if (isEOF())
		throw ReaderException("Reader::readChar", "EOF.", getLine(), getRow());
	char tRet = m_str[m_pos];
	m_pos++;
	return tRet;
}

char Reader::peekChar()  ///< @brief Ԥ��ȡһ���ַ�
{
	if (isEOF())
		throw ReaderException("Reader::readChar", "EOF.", getLine(), getRow());
	char tRet = m_str[m_pos];
	return tRet;
}

void Reader::ignoreSpace()  ///< @brief �����հ��ַ�
{
	while (!isEOF())
	{
		char tRet = peekChar();
		if (isspace(tRet))
			readChar();
		else
			break;
	}
}

/// @brief ƥ��һ���ַ�
/// @note  ƥ��һ���ַ���ʧ���������
/// @param[in] ch Ҫƥ����ַ�
/// @parampin] bIgnoreSpace ���Կո�
void Reader::match(char ch, bool bIgnoreSpace)
{
	if (bIgnoreSpace)
		ignoreSpace();

	char tChar = readChar();
	if (tChar != ch)
	{
		char tTextBuffer[1024];
		sprintf_s(tTextBuffer, "'%c' expected, buf found '%c'.", ch, tChar);
		throw ReaderException("Reader::match", tTextBuffer, getLine(), getRow());
	}
}

/// @brief ƥ��һ���ַ���
/// @note  ƥ��һ���ַ�����ʧ���������
/// @param[in] str Ҫƥ����ַ���
/// @param[in] bIgnoreSpace ���Կո�
void Reader::match(std::string str, bool bIgnoreSpace)
{
	if (bIgnoreSpace)
		ignoreSpace();

	int tLen = str.size();

	for (int i = 0; i < tLen; ++i)
	{
		char tChar = readChar();
		if (tChar != str[i])
		{
			char tTextBuffer[1024];
			sprintf_s(tTextBuffer, "'%s' expected, buf found '%c'.", str, tChar);
			throw ReaderException("Reader::match", tTextBuffer, getLine(), getRow());
		}
	}
}

/// @brief ����ƥ��һ���ַ�
/// @note  ����ƥ��һ���ַ�����ʧ���������
/// @param[in] ch Ҫƥ����ַ�
/// @param[in] bIgnoreSpace ���Կո�
/// @param[in] bMatch �����ƥ�䣬�Ƿ�ƥ������ַ�
/// @return  true=����ƥ�䣬false=ƥ��ʧ��
bool Reader::tryMatch(char ch, bool bIgnoreSpace, bool bMatch)
{
	int tPos = m_pos;

	if (bIgnoreSpace)
		ignoreSpace();

	if (isEOF())
	{
		m_pos = tPos;
		return false;
	}

	char tChar = readChar();

	if (tChar != ch)
	{
		//��ƥ�䣬���ؼ�
		m_pos = tPos;
		return false;
	}
	else
	{
		//ƥ��
		if (!bMatch)
		{
			m_pos = tPos;
		}
		return true;
	}
}

/// @brief ����ƥ��һ���ַ���
/// @note  ����ƥ��һ���ַ�����ʧ���������
/// @param[in] str Ҫƥ����ַ���
/// @param[in] bIgnoreSpace ���Կո�
/// @param[in] bMatch �����ƥ�䣬�Ƿ�ƥ������ַ�
/// @return  true=����ƥ�䣬false=ƥ��ʧ��
bool Reader::tryMatch(std::string str, bool bIgnoreSpace, bool bMatch)
{
	int tPos = m_pos;
	int tLen = str.size();
	if (bIgnoreSpace)
		ignoreSpace();

	if (m_str.size() - m_pos < tLen)
	{
		//���Ȳ��㣬����ƥ��
		m_pos = tPos;
		return false;
	}

	for (int i = 0; i < tLen; ++i)
	{
		if (m_str[m_pos + i] != str[i])
		{
			//����ƥ��
			m_pos = tPos;
			return false;
		}
	}

	if (bMatch)
	{
		m_pos += tLen;
	}
	else
	{
		m_pos = tPos;
	}
	return true;
}