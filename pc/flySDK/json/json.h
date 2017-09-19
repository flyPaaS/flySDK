////////////////////////////////////////////////////////////////////////////////
/// @file  Json.h
/// @brief Json������
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "reader.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief JSONԪ������
////////////////////////////////////////////////////////////////////////////////
enum JSONVALUETYPE
{
	JSONVALUETYPE_NULL,   ///< @brief ������
	JSONVALUETYPE_BOOL,   ///< @brief �߼�����
	JSONVALUETYPE_NUMBER, ///< @brief ��������
	JSONVALUETYPE_STRING, ///< @brief �ַ�������
	JSONVALUETYPE_LIST,   ///< @brief ������
	JSONVALUETYPE_DICT,   ///< @brief �ֵ�����
};

class JsonString;
class JsonList;
class JsonDict;

////////////////////////////////////////////////////////////////////////////////
/// @brief JSONֵ
////////////////////////////////////////////////////////////////////////////////
class JsonValue
{
	friend class JsonString;
	friend class JsonList;
	friend class JsonDict;
	friend class Json;
protected:
	JSONVALUETYPE m_Type;  ///< @brief JSON��������
	/// @brief ������
	union
	{
		bool m_ValueBool;     ///< @brief �߼���
		double m_ValueNumber; ///< @brief ������
	};
public:
	/// @brief      д���ַ���
	/// @param[out] OutStr �����Ŀ���ַ���
	virtual void writeToStr(std::string& OutStr);
public: //���ڽӿ�ת��
	virtual JsonString* toString();   ///< @brief ת�����ַ���
	virtual JsonList* toList();       ///< @brief ת��������
	virtual JsonDict* toDict();       ///< @brief ת�����ֵ�
public: //����ת��
	JSONVALUETYPE getType();       ///< @brief ��������

	bool toNull();                 ///< @brief ת����NULL
	///< @note  true��ʾNULL��false��ʾ��NULL
	bool toBool();                 ///< @brief ת����Bool
	///< @note  true��ʾtrue��false��ʾ��false
	double toNumber();             ///< @brief ������
	///< @brief ��Number����0��ʾ
public: // ֵ���Ͳ�������ֵ���ͷ���false
	bool setValue();                 ///< @brief ����ΪNULL
	///< @return ���������ֵ���ͣ��򷵻ؼ�
	bool setValue(bool Value);		 ///< @brief ����Ϊbool
	///< @return ���������ֵ���ͣ��򷵻ؼ�
	bool setValue(double Value);     ///< @brief ����Ϊ����
	///< @return ���������ֵ���ͣ��򷵻ؼ�
public: // �����ֶ�����
	JsonValue();                       ///< @brief ����Ϊnullֵ����
	JsonValue(bool Value);             ///< @brief ����Ϊboolֵ����
	JsonValue(double Value);           ///< @brief ����Ϊ��ֵ����
	JsonValue(JSONVALUETYPE Type);     ///< @brief ����ΪString/List/Dict
	virtual ~JsonValue();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief JSON�ַ���
////////////////////////////////////////////////////////////////////////////////
class JsonString : public JsonValue
{
protected:
	std::string m_Str;   ///< @brief �ַ�������ֵ
public:
	void writeToStr(std::string& OutStr);
public: //���ڽӿ�ת��
	JsonString* toString();   ///< @brief ת�����ַ���
public:
	std::string getStr();           ///< @brief ����ַ���
	void setStr(std::string Value); ///< @brief �����ַ���
public:
	/// @brief     ���캯��
	/// @param[in] Value �ַ���ֵ
	JsonString(std::string Value);
	~JsonString();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief JSON����
/// @note  JsonList���Ḻ�������Ӷ���
////////////////////////////////////////////////////////////////////////////////
class JsonList : public JsonValue
{
protected:
	std::vector<JsonValue*> m_ObjList;   ///< @brief �ڲ�����
public:
	void writeToStr(std::string& OutStr);
public: //���ڽӿ�ת��
	JsonList* toList();       ///< @brief ת��������

public:
	/// @brief     ��ö���
	/// @param[in] Index ����
	/// @return    ������������ڷ���NULL�����򷵻ض���ָ��
	JsonValue* getValue(int Index);

	/// @brief     ���ö���
	/// @param[in] Index ����
	/// @param[in] pNew  ����ָ��
	/// @return    ���ز����Ƿ�ɹ�
	bool setValue(int Index, JsonValue* pNew);

	/// @brief     ������ĩ��׷�Ӷ���
	/// @param[in] pNew ����ָ��
	void append(JsonValue* pNew);

	/// @brief ���
	void clear();

	/// @brief ���ض�������
	int getCount();
public:
	/// @brief ���캯��
	JsonList();
	~JsonList();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief JSON�ֵ�
////////////////////////////////////////////////////////////////////////////////
class JsonDict : public JsonValue
{
protected:
	std::vector<std::string> m_ObjList;                      ///< @brief ��������
	std::unordered_map<std::string, JsonValue*> m_Cache;  ///< @brief �����ֵ仺��
public:
	void writeToStr(std::string& OutStr);
public: //���ڽӿ�ת��
	JsonDict* toDict();       ///< @brief ת�����ֵ�
public:
	/// @brief      ���ض���
	/// @param[in]  Index ��������
	/// @param[out] pKeyOut ��
	/// @return     ���ض���ָ�룬������Խ�緵��NULL
	JsonValue* getValue(int Index, std::string* pKeyOut = NULL);

	/// @brief     ���ض���
	/// @param[in] Name �������
	/// @return    ���ض���ָ�룬�������ڶ��󷵻�NULL
	JsonValue* getValue(std::string Name);

	/// @brief     ���ö���
	/// @note      ��������Ѿ���������ͷ�ԭ�ж���
	/// @param[in] Name �������
	/// @param[in] pNew ����ָ��
	void setValue(std::string Name, JsonValue* pNew);

	/// @brief     �Ƿ��������
	/// @param[in] Name ����ļ���
	/// @return    true=��������false=û��������
	bool contain(std::string Name);

	/// @brief     �Ƴ�����
	/// @param[in] Index �������
	/// @return    true=�ɹ���false=ʧ��
	bool remove(std::string Index);

	/// @brief ���
	void clear();

	/// @brief ����Ԫ�ظ���
	int getCount();
public:
	/// @brief ���캯��
	JsonDict();
	~JsonDict();
};

////////////////////////////////////////////////////////////////////////////////
/// @brief JSON������
/// @note  ע��ý�����ֻ�������һ��Ԫ�ء�
////////////////////////////////////////////////////////////////////////////////
class Json
{
private:
	JsonValue* m_Root;  ///< @brief ���ڵ�
private: // ��������
	JsonValue* parseValue(Reader& Context);    ///< @brief ����һ��ֵ
	JsonValue* parseNumber(Reader& Context);   ///< @brief ����һ����ֵ
	JsonString* parseString(Reader& Context);  ///< @brief ����һ���ַ���
	JsonList* parseList(Reader& Context);      ///< @brief ����һ����
	JsonDict* parseDict(Reader& Context);      ///< @brief ����һ���ֵ�
public:
	JsonValue* getRoot();              ///< @brief ����һ�����ڵ�
	void setRoot(JsonValue* pValue);   ///< @brief ���ø��ڵ�

	void writeToStr(std::string& pOut); ///< @brief д���ַ���
	void writeToStream(std::ofstream* pOut); ///< @brief д����
public:
	Json();                           ///< @brief ���캯��
	Json(const std::string& Str);    ///< @brief ���ı�����JSON
	Json(std::ifstream* pStream);         ///< @brief ��������JSON
	~Json();
};
/// @}
