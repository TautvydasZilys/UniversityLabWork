// Tautvydas �ilys, IFF-1
// Link with /SUBSYSTEM:WINDOWS

#include <codecvt>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>

using namespace std;

#undef min
#undef max

vector<pair<int, int>> ReadInput(wstring fileName);
vector<vector<pair<int, int>>> FindMinimumCosts(const vector<pair<int, int>>& matrices);
wstring MatrixToString(const pair<int, int>& matrix);
pair<wstring, vector<pair<int, int>>> AddParanthesesToInput(const vector<pair<int, int>>& matrices);
wstring MakeAnswerString(const vector<pair<int, int>>& matrices, const vector<vector<pair<int, int>>>& minimumCosts, int from, int to);
void WriteResult(wstring fileName, wstring matrixMultiplicationOrder, int minimumNumberOfOpsNeeded);

int __stdcall WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
)
{
	auto const matrices = ReadInput(L"input.txt");
	auto const minimumCosts = FindMinimumCosts(matrices);
	auto const answer = MakeAnswerString(matrices, minimumCosts, 0, matrices.size() - 1);

	WriteResult(L"output.txt", answer, minimumCosts[0][matrices.size() - 1].first);
	ShellExecute(nullptr, L"open", L"output.txt", nullptr, nullptr, SW_SHOWNORMAL);

	return 0;
}

// Takes file name
// Returns vector< pair< matrix row count, matrix column count >>
vector<pair<int, int>> ReadInput(wstring fileName)
{
	ifstream in(fileName);
	vector<pair<int, int>> matrices;
	pair<int, int> input;
	int lastColumnSize;

	if (!in.is_open())
	{
		vector<wchar_t> fullPath(1024, '\0');
		GetFullPathName(fileName.c_str(), 1024, fullPath.data(), nullptr);

		MessageBox(nullptr, (wstring(L"Nepavyko atidaryti duomenų failo\r\n\"") + fullPath.data() + L"\"").c_str(), L"Klaida!", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	while (!in.eof())
	{
		in >> input.first >> input.second;

		if (!in.fail())
		{
			if (matrices.size() != 0 && input.first != lastColumnSize)
			{
				MessageBox(nullptr, (L"Blogi duomenys! Kairiojo dauginamojo stulpelių skaičius nesutampa su dešiniojo dauginamojo "
						L"eilučių skaičiumi " + to_wstring(matrices.size()) + L"-ajame daugybos veiksme.").c_str(), L"Klaida!", MB_OK | MB_ICONERROR);
				exit(-1);
			}
			else
			{
				lastColumnSize = input.second;
				matrices.emplace_back(input.first, input.second);
			}
		}
	}

	return matrices;
}

// Takes vector< pair< matrix row count, matrix column count >>
// Returns vector< vector< multiplication cost, separator index >> - first dimension index is where multiplication starts, second - where multiplication ends minus first dimension index
vector<vector<pair<int, int>>> FindMinimumCosts(const vector<pair<int, int>>& matrices)
{
	vector<vector<pair<int, int>>> minimumCosts;
	vector<int> matrixSizes(matrices.size() + 1);

	minimumCosts.reserve(matrices.size());
	for (auto i = 0u; i < matrices.size(); i++)
	{
		minimumCosts.emplace_back(matrices.size() - i, make_pair(0, 0));
	}

	matrixSizes[0] = matrices[0].first;
	for (auto i = 0u; i < matrices.size(); i++)
	{
		matrixSizes[i + 1] = matrices[i].second;
	}

	
	for (auto u = 1u; u < matrices.size(); u++)
	{
		for (auto i = 0u; i < matrices.size() - u; i++)
		{
			auto j = i + u;
			
			int minimum = numeric_limits<int>::max();
			int bestK = 0;

			for (auto k = 0u; k < j - i; k++)
			{
				auto value = minimumCosts[i][k].first + minimumCosts[i + k + 1][j - i - k - 1].first + matrixSizes[i] * matrixSizes[i + k + 1] * matrixSizes[j + 1];

				if (value < minimum)
				{
					minimum = value;
					bestK = k;
				}
			}

			minimumCosts[i][j - i] = make_pair(minimum, bestK);
		}
	}

	return minimumCosts;
}

wstring MatrixToString(const pair<int, int>& matrix)
{
	return L"{" + to_wstring(matrix.first) + L"; " + to_wstring(matrix.second) + L"}";
}

// Takes vector< pair< matrix row count, matrix column count >>
// Returns pair< matrix multiplication string representation, vector< pair< matrix location, matrix length in characters >>>
pair<wstring, vector<pair<int, int>>> AddParanthesesToInput(const vector<pair<int, int>>& matrices)
{
	wstring str;
	vector<pair<int, int>> matrixLocationsAndLengths(matrices.size());

	for (auto i = 0u; i < matrices.size(); i++)
	{
		int location = str.length();
		str += MatrixToString(matrices[i]);
		matrixLocationsAndLengths[i] = make_pair(location, str.length() - location);
		
		if (i != matrices.size() - 1)
		{
			str += L" * ";
		}
	}
	
	return make_pair(str, matrixLocationsAndLengths);
}

// Takes:
//		* vector< pair< matrix row count, matrix column count >>
//		* vector< vector< multiplication cost, separator index >> - first dimension index is where multiplication starts, second - where multiplication ends minus first dimension index
//		* Start of multiplication region
//		* End of multiplication region
wstring MakeAnswerString(const vector<pair<int, int>>& matrices, const vector<vector<pair<int, int>>>& minimumCosts, int from, int to)
{
	if (from == to)
	{
		return MatrixToString(matrices[from]);
	}
	else
	{
		auto separator = minimumCosts[from][to - from].second;

		return L"(" + MakeAnswerString(matrices, minimumCosts, from, from + separator) + L" * " + MakeAnswerString(matrices, minimumCosts, from + separator + 1, to) + L")";
	}
}

void WriteResult(wstring fileName, wstring matrixMultiplicationOrder, int minimumNumberOfOpsNeeded)
{
	const locale utf8(locale(), new codecvt_utf8<wchar_t>());
	wofstream out(fileName);
	out.imbue(utf8);

	wstring answer = L"Mažiausias sandaugų skaičius: " + to_wstring(minimumNumberOfOpsNeeded) + L"\r\n" 
					 L"Daugybos tvarka: " + matrixMultiplicationOrder;

	if (!out.is_open())
	{
		vector<wchar_t> fullPath(1024, '\0');
		GetFullPathName(fileName.c_str(), 1024, fullPath.data(), nullptr);

		MessageBox(nullptr, (wstring(L"Nepavyko atidaryti rezultatų failo\r\n\"") + fullPath.data() + L"\" rašymui. \r\n\r\n" + answer).c_str(), L"Klaida!", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	out << answer << endl;
	out.close();
}