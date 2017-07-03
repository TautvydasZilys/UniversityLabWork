#include "PrecompiledHeader.h"
#include "StoredValue.h"
#include "IStoredCollection.h"

StoredValue::StoredValue(const IStoredCollection* collection, long long int index) :
	m_Collection(const_cast<IStoredCollection*>(collection)), m_Index(index)
{
}

StoredValue::~StoredValue()
{
}

StoredValue::operator int() const 
{
	return m_Collection->Get(m_Index); 
}

int StoredValue::operator=(int value)
{
	m_Collection->Set(m_Index, value); 

	return value;
}

int StoredValue::operator=(const StoredValue& value)
{
	int val = value.m_Collection->Get(value.m_Index);
	m_Collection->Set(m_Index, val);

	//OutputDebugString((m_Collection->m_FilePath + L"[" + to_wstring(m_Index) + L"] = " + value.m_Collection->m_FilePath + L"[" + to_wstring(value.m_Index) + L"] = " + to_wstring(val) + L";\r\n").c_str());
	return val;
}