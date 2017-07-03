#pragma once

class Node
{
private:
	weak_ptr<Node> m_Parent;
	shared_ptr<Node> m_Sibling;
	shared_ptr<Node> m_Child;

	double m_Score;
	wstring m_Name;
	
	double m_MaxIncludedScore;
	double m_MaxExcludedScore;
	vector<pair<shared_ptr<Node>, bool>> m_ChildExclusivityList;
	
	void AppendGuestListInternal(vector<shared_ptr<Node>>& guestList, shared_ptr<Node> thisPtr, bool imIncluded);
	shared_ptr<Node> GetThisPtr();

public:
	Node(shared_ptr<Node> parent, double score, wstring name);
	~Node();

	void AddChild(shared_ptr<Node> child);
	void AddSibling(shared_ptr<Node> sibling);
	
	shared_ptr<Node> GetParent() { return m_Parent.lock(); }
	shared_ptr<Node> GetSibling() { return m_Sibling; }
	shared_ptr<Node> GetChild() { return m_Child; }
	double GetMaximalScore() { return max(m_MaxIncludedScore, m_MaxExcludedScore); }

	const wstring& GetName() { return m_Name; }
	double GetScore() { return m_Score; }

	void CalculateScores();
	void AppendGuestList(vector<shared_ptr<Node>>& guestList);
};

