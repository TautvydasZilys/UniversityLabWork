#include "PrecompiledHeader.h"
#include "RandomAccessStoredCollection.h"

RandomAccessStoredCollection::RandomAccessStoredCollection() :
	IStoredCollection(GetTemporaryFileName(), FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE)
{
}

RandomAccessStoredCollection::RandomAccessStoredCollection(wstring filePath) :
	IStoredCollection(filePath, FILE_FLAG_RANDOM_ACCESS | FILE_ATTRIBUTE_NORMAL)
{
}


RandomAccessStoredCollection::RandomAccessStoredCollection(wstring filePath, DWORD extraFlags) :
	IStoredCollection(filePath, FILE_FLAG_RANDOM_ACCESS | extraFlags)
{
}

RandomAccessStoredCollection::~RandomAccessStoredCollection()
{
}


void RandomAccessStoredCollection::Seek(long long int index)
{
	auto offset = (index - m_Position) * static_cast<long long int>(sizeof(int));
	if (SetFilePointerEx(m_File, *reinterpret_cast<LARGE_INTEGER*>(&offset), nullptr, FILE_CURRENT) == FALSE)
	{
		MessageBox(nullptr, (L"Klaida nustatant indeksà faile \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", 
			MB_OK | MB_ICONERROR);
	}
	m_Position = index;
}


RandomAccessStoredCollection& RandomAccessStoredCollection::GetTempCollection()
{
	static RandomAccessStoredCollection collection(L"tempRandomAccess", FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE);
	collection.m_FileLength = 0;

	return collection;
}