#pragma once

class IStoredCollection;
class StoredValue
{
private:
	long long int m_Index;
	IStoredCollection* m_Collection;
public:
	StoredValue(const IStoredCollection* collection, long long int index);
	~StoredValue();

	operator int() const;			// Getter
	int operator=(int value);		// Setter
	int operator=(const StoredValue& value);	// Another setter
};