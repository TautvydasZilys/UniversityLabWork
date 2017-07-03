#include "PrecompiledHeader.h"
#include "RandomAccessStoredCollection.h"
#include "SequencialAccessStoredCollection.h"

#undef min
#undef max

const wstring FileNames[] = { L"original.txt", L"mergeSortRandomAccess.txt", L"mergeSortSequencialAccess.txt", L"heapSortRandomAccess.txt",
	L"heapSortSequencialAccess.txt", L"radixSortRandomAccess.txt", L"radixSortSequencialAccess.txt" };

bool g_TestMode = false;
long long int g_OpCount = 0;

double StartPerformanceCounter();
void CreateData();
void CreateNonRandomData();

void PrintCollection(IStoredCollection& collection);
void Validate(wstring fileName);

template <class T>
void MergeSort(T& collection, long long int from, long long int to);
template <class T>
void Merge(T& collection, long long int from, long long int to);
template <>
void Merge<SequencialAccessStoredCollection>(SequencialAccessStoredCollection& collection, long long int from, long long int to);

template <class T>
void HeapSort(T& collection, long long int from, long long int to);
template <class T>
void Heapify(T& collection, long long int from, long long int to);
template <class T>
void ForwardDown(T& collection, long long int from, long long int to, long long int index);
template <class T>
void ForwardUp(T& collection, long long int from, long long int to, long long int index);

template <class T>
void RadixSort(T& collection, long long int from, long long int to);

template <class T>
void TestSort(wstring sortName, function<void(T&, long long int, long long int)> sortFunction, wstring collectionType, wstring fileName);

int main(int argc, char** argv)
{
#ifdef _DEBUG
	MessageBox(nullptr, L"You can now attach a debugger.", L"Debug", MB_OK);
#endif

	setlocale(LC_ALL, "lithuanian");

	wcout.setf(ios::fixed);
	wcout.precision(8);

	if (argc > 1 && strcmp(argv[1], "--test") == 0)
	{
		g_TestMode = true;
	}

	StartPerformanceCounter();
	CreateData();

	TestSort<RandomAccessStoredCollection>(L"Merge", MergeSort<RandomAccessStoredCollection>, L"random access", FileNames[1]);
	TestSort<RandomAccessStoredCollection>(L"Heap", HeapSort<RandomAccessStoredCollection>, L"random access", FileNames[3]);
	TestSort<RandomAccessStoredCollection>(L"Radix", RadixSort<RandomAccessStoredCollection>, L"random access", FileNames[5]);	

	TestSort<SequencialAccessStoredCollection>(L"Merge", MergeSort<SequencialAccessStoredCollection>, L"sequencial access", FileNames[2]);
	TestSort<SequencialAccessStoredCollection>(L"Heap", HeapSort<SequencialAccessStoredCollection>, L"sequencial access", FileNames[4]);
	TestSort<SequencialAccessStoredCollection>(L"Radix", RadixSort<SequencialAccessStoredCollection>, L"sequencial access", FileNames[6]);

	if (!g_TestMode)
	{
		system("pause");
	}

	return 0;
}


double StartPerformanceCounter()
{
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&counter);		// Returns frequency in ticks per second

	return counter.QuadPart / 1000.0;
}


double GetPerformanceCounter()
{
	static double s_CounterFrequency = StartPerformanceCounter();

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return counter.QuadPart / s_CounterFrequency;
}


void CreateData()
{
	long long int n;
	
	if (!g_TestMode)
	{
		wcout << L"Áveskite masyvo nariø skaièiø: " << endl;
	}

	cin >> n;
	
	vector<int> numbers(n);
	mt19937 randomEngine;
	uniform_int_distribution<int> randomDistribution(0, numeric_limits<int>::max());
	auto randomGenerator = bind(randomDistribution, ref(randomEngine));

	for_each (begin(numbers), end(numbers), [&randomGenerator](int& number)
	{
		number = randomGenerator();
	});

	for_each (begin(FileNames), end(FileNames), [&numbers](const wstring& fileName)
	{
		ofstream out(fileName, ios_base::binary);
		out.write(reinterpret_cast<char*>(&numbers[0]), numbers.size() * sizeof(int));
		out.close();
	});
	
	if (!g_TestMode)
	{
		wcout << L"Duomenys sugeneruoti!" << endl;
	}
}


void CreateNonRandomData()
{
	const int numberOfNumbers = 10;
	int numbers[] = { 3, 2, 1, 5, 4, 6, 7, 10, 9, 8 };
	
	for_each (begin(FileNames), end(FileNames), [&numbers, numberOfNumbers](const wstring& fileName)
	{
		ofstream out(fileName, ios_base::binary);
		out.write(reinterpret_cast<char*>(numbers), numberOfNumbers * sizeof(int));
		out.close();
	});
}


void PrintCollection(IStoredCollection& collection)
{
	for (int i = 0; i < collection.Length(); i++)
	{
		wcout << L"\t" << collection[i] << endl;
	}
}


template <class T>
void MergeSort(T& collection, long long int from, long long int to)
{
	if (to - from == 1)
	{
		return;
	}
	
	MergeSort(collection, from, from + (to - from) / 2);
	MergeSort(collection, from + (to - from) / 2, to);
	Merge(collection, from, to);

	g_OpCount += 3;
}

template <class T>
void Merge(T& collection, long long int from, long long int to)
{
	auto& tempCollection = T::GetTempCollection();

	auto half = from + (to - from) / 2;
	auto i = from, 
		 j = half;
	auto u = 0LL;
	
	g_OpCount += 5;

	while (i < half && j < to)
	{
		int valueI = collection[i];
		int valueJ = collection[j];

		if (valueI < valueJ)
		{
			tempCollection[u] = valueI;
			i++;
		}
		else
		{
			tempCollection[u] = valueJ;
			j++;
		}

		u++;

		g_OpCount += 6;
	}

	g_OpCount += 2 * (half + to - i - j) + 4 * (to - from) + 2;

	for (; i < half; i++)
	{
		tempCollection[u++] = collection[i];
	}

	for (; j < to; j++)
	{
		tempCollection[u++] = collection[j];
	}

	for (i = from, u = 0; i < to; i++, u++)
	{
		collection[i] = tempCollection[u];
	}
}

template <>
void Merge<SequencialAccessStoredCollection>(SequencialAccessStoredCollection& collection, long long int from, long long int to)
{
	auto& tempCollection = SequencialAccessStoredCollection::GetTempCollection();

	auto half = from + (to - from) / 2;
	auto u = 0LL;

	
	{
		auto i = SequencialAccessStoredCollection::Iterator(collection, from),
			 j = SequencialAccessStoredCollection::Iterator(collection, half);
		
		g_OpCount += 5;

		while (i.Position() < half && j.Position() < to)
		{
			int valueI = *i;
			int valueJ = *j;

			if (valueI < valueJ)
			{
				tempCollection[u] = valueI;
				++i;
			}
			else
			{
				tempCollection[u] = valueJ;
				++j;
			}

			u++;

			g_OpCount += 8;
		}

		g_OpCount += 2 * (half + to - i.Position() - j.Position()) + 4 * (to - from) + 2;

		for (; i.Position() < half; ++i)
		{
			tempCollection[u++] = *i;
		}

		for (; j.Position() < to; ++j)
		{
			tempCollection[u++] = *j;
		}
	}

	u = 0;
	for (auto i = from; i < to; i++, u++)
	{
		collection[i] = tempCollection[u];
	}
}

template <class T>
void ForwardDown(T& collection, long long int from, long long int to, long long int index)
{
	auto childIndex = (index - from + 1) * 2 + from - 1;
	g_OpCount += 2;

	if (childIndex >= to)
	{
		return;
	}

	auto current = collection[index];
	auto childLeft = collection[childIndex];
	auto childRight = collection[childIndex + 1];

	if (childRight > childLeft && current < childRight)
	{
		childLeft = childRight;
		childIndex++;
		
		g_OpCount += 2;
	}
	
	g_OpCount += 6;

	if (current < childLeft)
	{
		collection[childIndex] = current;
		collection[index] = childLeft;

		ForwardDown(collection, from, to, childIndex);

		g_OpCount += 3;
	}
}

template <class T>
void ForwardUp(T& collection, long long int from, long long int index)
{
	g_OpCount++;

	if (index == from)
	{
		return;
	}

	auto parentIndex = (index - from + 1 / 2) + from - 1;
	auto parent = collection[parentIndex];
	auto current = collection[index];

	if (current > parent)
	{
		collection[index] = parent;
		collection[parentIndex] = current;

		ForwardUp(collection, from, parentIndex);
		
		g_OpCount += 3;
	}
	
	g_OpCount += 4;
}

template <class T>
void Heapify(T& collection, long long int from, long long int to)
{
	g_OpCount += 3 * (to - from) + 1;

	for (auto i = from + 1; i < to; i++)
	{
		ForwardUp(collection, from, i);
	}
}

template <class T>
void HeapSort(T& collection, long long int from, long long int to)
{
	int temp;
	g_OpCount += 6 * (to - from) + 1;

	Heapify(collection, from, to);

	for (; to > from; to--)
	{
		temp = collection[from];
		collection[from] = collection[to - 1];
		collection[to - 1] = temp;

		ForwardDown(collection, from, to, from);
	}
}

template <class T>
void RadixSort(T& collection, long long int from, long long int to)
{
	T temporaryCollections[256];
	
	g_OpCount += 3;
	for (unsigned int i = 0, mask = 0xFF; i < 4; i++, mask <<= 8)
	{
		for_each (temporaryCollections, temporaryCollections + 256, [](T& col)
		{
			col.Clear();
		});

		for (auto j = from; j < to; j++)
		{
			auto& tempCollection = temporaryCollections[(collection[j] & mask) >> (8 * i)];
			tempCollection[tempCollection.Length()] = collection[j];
		}

		auto v = from;
		for_each (temporaryCollections, temporaryCollections + 256, [&v, &collection](T& col)
		{
			g_OpCount += col.Length();

			for (auto u = 0LL; u < col.Length(); u++)
			{
				collection[v++] = col[u];
			}			
		});

		g_OpCount += 260 + 2 * (to - from);
	}
}


template <class T>
void TestSort(wstring sortName, function<void(T&, long long int, long long int)> sortFunction, wstring collectionType, wstring fileName)
{
	double algorithmStartTime, algorithmDuration;
	g_OpCount = 0;

	{
		T sortCollection(fileName);
		
		algorithmStartTime = GetPerformanceCounter();
		sortFunction(sortCollection, 0, sortCollection.Length());
		algorithmDuration = GetPerformanceCounter() - algorithmStartTime;
	}

	
	if (!g_TestMode)
	{
		wcout << sortName << L" sort su " << collectionType << L" uþtruko " << algorithmDuration << L" ms ir padarë " << g_OpCount << " operacijø." << endl;
	}
	else
	{
		wcout << algorithmDuration << " " << g_OpCount << endl;
	}

	Validate(fileName);
}

void Validate(wstring fileName)
{
	ifstream in(fileName);

	if (!in.is_open())
	{
		MessageBox(nullptr, (L"Nepavyko atidaryti failo \"" + fileName + L"\" patikrinimui!").c_str(), L"Klaida!", MB_OK | MB_ICONERROR);
	}

	in.seekg(0, ios::end);
	long long int fileLength = in.tellg() / sizeof(int);
	in.seekg(0, ios::end);

	int a, b;
	in.read(reinterpret_cast<char*>(&a), sizeof(int)); 

	for (int i = 0; i < fileLength; i++)
	{
		in.read(reinterpret_cast<char*>(&b), sizeof(int));
		if (a > b)
		{
			MessageBox(nullptr, 
				(L"Failas \"" + fileName + L"\" pozicijoje " + to_wstring(i) + L" nëra surikiuotas [" + to_wstring(a) + L" > " + to_wstring(b) + L"]!").c_str(), 
				L"Klaida!",	MB_OK | MB_ICONERROR);
			in.close();
			return;
		}
	}

	in.close();
}