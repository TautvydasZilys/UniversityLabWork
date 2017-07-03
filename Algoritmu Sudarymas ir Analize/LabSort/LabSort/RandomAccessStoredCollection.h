#pragma once

#include "IStoredCollection.h"

class RandomAccessStoredCollection : 
	public IStoredCollection
{
protected:
	virtual void Seek(long long int index);
private:
	RandomAccessStoredCollection(const RandomAccessStoredCollection &);
	RandomAccessStoredCollection(wstring filePath, DWORD extraFlags);
public:
	RandomAccessStoredCollection();
	RandomAccessStoredCollection(wstring filePath);
	~RandomAccessStoredCollection();

	static RandomAccessStoredCollection& GetTempCollection();
};

