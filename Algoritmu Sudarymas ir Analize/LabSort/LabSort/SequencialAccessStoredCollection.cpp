#include "PrecompiledHeader.h"
#include "SequencialAccessStoredCollection.h"

SequencialAccessStoredCollection::SequencialAccessStoredCollection() :
	IStoredCollection(GetTemporaryFileName(), FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE)
{
}


SequencialAccessStoredCollection::SequencialAccessStoredCollection(wstring filePath) :
	IStoredCollection(filePath, FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL)
{
}


SequencialAccessStoredCollection::SequencialAccessStoredCollection(wstring filePath, DWORD extraFlags) :
	IStoredCollection(filePath, FILE_FLAG_SEQUENTIAL_SCAN | extraFlags)
{
}


SequencialAccessStoredCollection::~SequencialAccessStoredCollection()
{
}


void SequencialAccessStoredCollection::Seek(long long int index)
{
	long long int step = index > m_Position ? sizeof(int) : -static_cast<long long int>(sizeof(int));
	auto stepCast = *reinterpret_cast<LARGE_INTEGER*>(&step);
	step /= static_cast<long long int>(sizeof(int));			// sizeof(int) is unsigned

	while (m_Position != index)
	{
		if (SetFilePointerEx(m_File, stepCast, nullptr, FILE_CURRENT) == FALSE)
		{
			MessageBox(nullptr, (L"Klaida nustatant indeksà faile \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", 
				MB_OK | MB_ICONERROR);
		} 
		m_Position += step;
	}
}


SequencialAccessStoredCollection& SequencialAccessStoredCollection::GetTempCollection()
{
	static SequencialAccessStoredCollection collection(L"tempSequencialAccess", FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE);
	collection.m_FileLength = 0;

	return collection;
}