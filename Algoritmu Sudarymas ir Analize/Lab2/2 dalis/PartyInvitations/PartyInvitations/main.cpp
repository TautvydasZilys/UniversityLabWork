#include "PrecompiledHeader.h"	
#include "Node.h"

shared_ptr<Node> InputTree(wstring fileName);
pair<vector<shared_ptr<Node>>, double> ComputeGuestListAndScore(shared_ptr<Node> rootNode);
double AppendGuestListFromChildren(vector<shared_ptr<Node>>& guestList, shared_ptr<Node> parent);
void WriteResults(wstring fileName, const vector<shared_ptr<Node>>& guestList, double score);

int __stdcall WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
)
{
	auto rootNode = InputTree(L"input.txt");
	auto const guestListAndScore = ComputeGuestListAndScore(rootNode);

	WriteResults(L"output.txt", guestListAndScore.first, guestListAndScore.second);
	ShellExecute(nullptr, L"open", L"output.txt", nullptr, nullptr, SW_SHOWNORMAL);

	return 0;
}

shared_ptr<Node> InputTree(wstring fileName)
{
	vector<tuple<wstring, double, int, int, shared_ptr<Node>>> tempStorage;
	wstring name;
	double score;
	int childIndex;
	int siblingIndex;

	wifstream in(fileName);
	
	if (!in.is_open())
	{
		vector<wchar_t> fullPath(1024, '\0');
		GetFullPathName(fileName.c_str(), 1024, fullPath.data(), nullptr);

		MessageBox(nullptr, (wstring(L"Nepavyko atidaryti duomenų failo\r\n\"") + fullPath.data() + L"\"").c_str(), L"Klaida!", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	while (!in.eof())
	{
		in >> name >> score >> childIndex >> siblingIndex;

		if (!in.fail())
		{
			tempStorage.emplace_back(name, score, childIndex - 1, siblingIndex - 1, nullptr);
		}
	}

	in.close();

	get<4>(tempStorage[0]) = make_shared<Node>(nullptr, get<1>(tempStorage[0]), get<0>(tempStorage[0]));

	for (auto i = 0u; i < tempStorage.size(); i++)
	{
		auto childIndex = get<2>(tempStorage[i]);
		auto siblingIndex = get<3>(tempStorage[i]);

		if (childIndex > 0)
		{
			shared_ptr<Node> child(make_shared<Node>(get<4>(tempStorage[i]), get<1>(tempStorage[childIndex]), get<0>(tempStorage[childIndex])));
			get<4>(tempStorage[i])->AddChild(child);
			get<4>(tempStorage[childIndex]) = child;
		}

		if (siblingIndex > 0)
		{
			shared_ptr<Node> sibling(make_shared<Node>(get<4>(tempStorage[i])->GetParent(), get<1>(tempStorage[siblingIndex]), get<0>(tempStorage[siblingIndex])));
			get<4>(tempStorage[i])->AddSibling(sibling);
			get<4>(tempStorage[siblingIndex]) = sibling;
		}
	}

	return get<4>(tempStorage[0]);
}


pair<vector<shared_ptr<Node>>, double> ComputeGuestListAndScore(shared_ptr<Node> rootNode)
{
	vector<shared_ptr<Node>> guestList;

	double score = rootNode->GetScore();
	guestList.push_back(rootNode);

	auto child = rootNode->GetChild();

	while (child != nullptr)
	{
		score += AppendGuestListFromChildren(guestList, child);
		child = child->GetSibling();
	}

	return make_pair(guestList, score);
}

double AppendGuestListFromChildren(vector<shared_ptr<Node>>& guestList, shared_ptr<Node> parent)
{
	double score = 0.0f;
	auto child = parent->GetChild();

	while (child != nullptr)
	{
		child->CalculateScores();
		child->AppendGuestList(guestList);
		score += child->GetMaximalScore();

		child = child->GetSibling();
	}

	return score;
}

void WriteResults(wstring fileName, const vector<shared_ptr<Node>>& guestList, double score)
{	
	const locale utf8(locale(), new codecvt_utf8<wchar_t>());
	wofstream out(fileName);
	out.imbue(utf8);

	wstringstream answer;
	
	answer.setf(ios::fixed);
	answer.precision(3);

	answer << L"Didžiausias draugystės taškų skaičius: " << score << endl << endl;;
	answer << L"Svečių sąrašas: " << endl;

	for_each (guestList.begin(), guestList.end(), [&answer](shared_ptr<Node> guest)
	{
		answer << setw(20) << left << guest->GetName() << setw(10) << guest->GetScore() << endl;
	});

	if (!out.is_open())
	{
		vector<wchar_t> fullPath(1024, '\0');
		GetFullPathName(fileName.c_str(), 1024, fullPath.data(), nullptr);

		MessageBox(nullptr, (wstring(L"Nepavyko atidaryti rezultatų failo\r\n\"") + fullPath.data() + L"\" rašymui. \r\n\r\n" + answer.str()).c_str(), L"Klaida!", MB_OK | MB_ICONERROR);
		exit(-1);
	}

	out << answer.str() << endl;
	out.close();
}