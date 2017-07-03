#pragma once

#include "IStoredCollection.h"

class SequencialAccessStoredCollection :
	public IStoredCollection
{
private:
	virtual void Seek(long long int index);
private:	
	SequencialAccessStoredCollection(const SequencialAccessStoredCollection &);
	SequencialAccessStoredCollection(wstring filePath, DWORD extraFlags);
public:
	SequencialAccessStoredCollection();
	SequencialAccessStoredCollection(wstring filePath);
	~SequencialAccessStoredCollection();
	
	static SequencialAccessStoredCollection& GetTempCollection();
};

