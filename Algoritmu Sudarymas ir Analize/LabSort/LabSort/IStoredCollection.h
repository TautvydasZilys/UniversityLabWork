#pragma once

#include "StoredValue.h"

class IStoredCollection
{
protected:
	HANDLE m_File;
	long long int m_FileLength;
	long long int m_Position;
	wstring m_FilePath;

	void Set(long long int index, int value);
	int Get(long long int index);

	virtual void Seek(long long int index) = 0;
	
	void Preview();
	long long int RealPos();

	friend class StoredValue;
private:
	IStoredCollection(const IStoredCollection &);
protected:
	IStoredCollection(wstring filePath, DWORD extraFlags);
public:
	virtual ~IStoredCollection();

	long long int Length() const { return m_FileLength; }	
	StoredValue operator[](long long int index);
	
	void Clear() { m_FileLength = 0; }

	class Iterator
	{
	private:
		IStoredCollection& m_Collection;
		long long int m_Pos;

	public:
		Iterator(IStoredCollection& collection, long long int pos);

		long long int Position() const { return m_Pos; }
		Iterator& Next() { m_Pos++; return *this; }
		Iterator& Previous() { m_Pos--; return *this; }
		Iterator& operator++() { m_Pos++; return *this; }
		Iterator& operator--() { m_Pos--; return *this; }
		const int operator*() const;
	};
};