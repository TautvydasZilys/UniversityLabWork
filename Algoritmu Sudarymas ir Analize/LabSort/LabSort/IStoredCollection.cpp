#include "PrecompiledHeader.h"

#include "IStoredCollection.h"
#include "StoredValue.h"


IStoredCollection::IStoredCollection(wstring filePath, DWORD flags) :
	m_Position(0),
	m_FilePath(filePath)
{
	m_File = CreateFile(m_FilePath.c_str(), 
						GENERIC_READ | GENERIC_WRITE, 
						FILE_SHARE_READ,
						nullptr,
						OPEN_EXISTING, 
						flags,
						nullptr);

	if (m_File == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			m_File = CreateFile(m_FilePath.c_str(), 
								GENERIC_READ | GENERIC_WRITE, 
								FILE_SHARE_READ,
								nullptr,
								CREATE_NEW, 
								flags,
								nullptr);

			if (m_File == INVALID_HANDLE_VALUE)
			{
				MessageBox(nullptr, (L"Nepavyko atidaryti failo \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", MB_OK | MB_ICONERROR);
				exit(-1);
			}
		}
		else
		{
			MessageBox(nullptr, (L"Nepavyko atidaryti failo \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", MB_OK | MB_ICONERROR);
			exit(-1);
		}
	}

	if (GetFileSizeEx(m_File, reinterpret_cast<PLARGE_INTEGER>(&m_FileLength)) == FALSE)
	{
		MessageBox(nullptr, (L"Nepavyko suþinoti failo \"" + m_FilePath + L"\" dydþio! " + GetErrorText(GetLastError())).c_str(), L"Klaida", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	m_FileLength /= sizeof(int);
}

IStoredCollection::~IStoredCollection()
{
	if (CloseHandle(m_File) == FALSE)
	{
		MessageBox(nullptr, (L"Nepavyko uþdaryti failo \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Áspëjimas", MB_OK | MB_ICONWARNING);
	}
}

void IStoredCollection::Preview()
{
	auto oldPos = m_Position;

	vector<int> contents(m_FileLength);

	for (int i = 0; i < m_FileLength; i++)
	{
		contents[i] = (*this)[i];
	}
	
	ofstream out("preview.txt");
	for_each (contents.begin(), contents.end(), [&out](int number)
	{
		out << number << endl;
	});

	out.close();
	system("preview.txt");
	DeleteFile(L"preview.txt");

	Seek(oldPos);
}

long long int IStoredCollection::RealPos()
{
	static LARGE_INTEGER zero;
	zero.QuadPart = 0;
	long long int realPosition;

	if (SetFilePointerEx(m_File, zero, reinterpret_cast<LARGE_INTEGER*>(&realPosition), FILE_CURRENT) == FALSE)
	{
		MessageBox(nullptr, (L"Klaida nustatant indeksà faile \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", 
			MB_OK | MB_ICONERROR);
	}
	
	return realPosition;
}

void IStoredCollection::Set(long long int index, int value)
{
	if (m_Position != index)
	{
		Seek(index);
	}
	
	static DWORD numberOfBytesWritten = 0;
	if (WriteFile(m_File, &value, sizeof(int), &numberOfBytesWritten, nullptr) == FALSE)
	{
		MessageBox(nullptr, (L"Klaida raðant á failà \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", 
			MB_OK | MB_ICONERROR);
	}
	m_Position++;

	if (m_Position >= m_FileLength)
	{
		m_FileLength = m_Position;
	}
}

int IStoredCollection::Get(long long int index)
{
	if (index >= m_FileLength)
	{
		for (auto i = m_FileLength; i <= index; i++)
		{
			Set(i, 0);
		}
	}

	if (m_Position != index)
	{
		Seek(index);
	}

	static int value = 0;
	static DWORD numberOfBytesRead = 0;
	if (ReadFile(m_File, &value, sizeof(int), &numberOfBytesRead, nullptr) == FALSE)
	{
		MessageBox(nullptr, (L"Klaida skaitant failà \"" + m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", 
			MB_OK | MB_ICONERROR);
	}
	m_Position++;

	return value;
}

StoredValue IStoredCollection::operator[](long long int index)
{
	return StoredValue(this, index);
}

IStoredCollection::Iterator::Iterator(IStoredCollection& collection, long long int pos) :
	m_Collection(collection), m_Pos(pos)
{
	m_Collection.Seek(pos);
}

const int IStoredCollection::Iterator::operator*() const
{	
	auto offset = (m_Pos - m_Collection.m_Position) * static_cast<long long int>(sizeof(int));
	if (SetFilePointerEx(m_Collection.m_File, *reinterpret_cast<LARGE_INTEGER*>(&offset), nullptr, FILE_CURRENT) == FALSE)
	{
		MessageBox(nullptr, (L"Klaida nustatant indeksà faile \"" + m_Collection.m_FilePath + L"\"! " + GetErrorText(GetLastError())).c_str(), L"Klaida", 
			MB_OK | MB_ICONERROR);
	}
	m_Collection.m_Position = m_Pos;

	return m_Collection.Get(m_Pos);
}