#include "PrecompiledHeader.h"
#include "Node.h"


Node::Node(shared_ptr<Node> parent, double score, wstring name) :
	m_Parent(parent), m_Score(score), m_Name(name), m_MaxIncludedScore(0.0), m_MaxExcludedScore(0.0)
{
}

Node::~Node()
{
}

void Node::AddChild(shared_ptr<Node> child)
{
	if (m_Child == nullptr)
	{
		m_Child = child;
	}
	else
	{
		auto baseChild = m_Child;

		while (baseChild->m_Sibling != nullptr)
		{
			baseChild = baseChild->m_Sibling;
		}

		baseChild->m_Sibling = child;
	}
}

void Node::AddSibling(shared_ptr<Node> sibling)
{
	if (m_Sibling == nullptr)
	{
		m_Sibling = sibling;
	}
	else
	{
		auto baseSibling = m_Sibling;

		while (baseSibling->m_Sibling != nullptr)
		{
			baseSibling = baseSibling->m_Sibling;
		}

		baseSibling->m_Sibling = sibling;
	}
}

void Node::CalculateScores()
{
	m_MaxIncludedScore = m_Score;
	m_MaxExcludedScore = 0.0f;

	auto baseChild = m_Child;
	while (baseChild != nullptr)
	{
		baseChild->CalculateScores();

		m_MaxIncludedScore += baseChild->m_MaxExcludedScore;

		if (baseChild->m_MaxExcludedScore > baseChild->m_MaxIncludedScore)
		{
			m_MaxExcludedScore += baseChild->m_MaxExcludedScore;
			m_ChildExclusivityList.emplace_back(baseChild, false);
		}
		else
		{
			m_MaxExcludedScore += baseChild->m_MaxIncludedScore;
			m_ChildExclusivityList.emplace_back(baseChild, true);
		}

		baseChild = baseChild->m_Sibling;
	}
}


void Node::AppendGuestList(vector<shared_ptr<Node>>& guestList)
{
	auto thisPtr = GetThisPtr();

	if (thisPtr == nullptr)
	{
		throw exception("Couldn't get this ptr!");
	}

	if (m_MaxExcludedScore > m_MaxIncludedScore)
	{
		AppendGuestListInternal(guestList, thisPtr, false);
	}
	else
	{
		AppendGuestListInternal(guestList, thisPtr, true);
	}
}

void Node::AppendGuestListInternal(vector<shared_ptr<Node>>& guestList, shared_ptr<Node> thisPtr, bool imIncluded)
{
	if (imIncluded)
	{
		guestList.push_back(thisPtr);

		for_each (m_ChildExclusivityList.begin(), m_ChildExclusivityList.end(), [&guestList](const pair<shared_ptr<Node>, bool>& child)
		{
			child.first->AppendGuestListInternal(guestList, child.first, false);
		});
	}
	else
	{
		for_each (m_ChildExclusivityList.begin(), m_ChildExclusivityList.end(), [&guestList](const pair<shared_ptr<Node>, bool>& child)
		{
			child.first->AppendGuestListInternal(guestList, child.first, child.second);
		});		
	}
}

shared_ptr<Node> Node::GetThisPtr()
{
	if (m_Parent.lock() == nullptr)
	{
		if (m_Child == nullptr)
		{
			return nullptr;
		}

		return m_Child->m_Parent.lock();
	}

	auto ptr = m_Parent.lock()->m_Child;

	while (ptr != nullptr)
	{
		if (ptr.get() == this)
		{
			return ptr;
		}

		ptr = ptr->m_Sibling;	
	}

	return nullptr;
}