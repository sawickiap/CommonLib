/** \file
\brief Persistent DOM model for a format based on Tokenizer.
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://regedit.gamedev.pl/ \n

Part of CommonLib library\n
License: GNU LGPL. \n
Documentation: \ref Module_TokDoc \n
Module elements: \ref code_tokdoc
*/
#include "Base.hpp"
#include "TokDoc.hpp"
#include "Tokenizer.hpp"

#ifdef WIN32
#include "DateTime.hpp"
#endif

namespace common {
namespace tokdoc {

////////////////////////////////////////////////////////////////////////////////
// class Node

Node::Node()
: m_Parent(NULL)
, m_FirstChild(NULL), m_LastChild(NULL)
, m_PrevSibling(NULL), m_NextSibling(NULL)
{

}

Node::Node( const Node &src )
: Name(src.Name)
, Value(src.Value)
, m_Parent(NULL)
, m_FirstChild(NULL), m_LastChild(NULL)
, m_PrevSibling(NULL), m_NextSibling(NULL)
{
	CopyChildrenFrom(src);
}

Node::Node( const tstring &value )
: Value(value)
, m_Parent(NULL)
, m_FirstChild(NULL), m_LastChild(NULL)
, m_PrevSibling(NULL), m_NextSibling(NULL)
{

}

Node::Node( const tstring &name, const tstring &value )
: Name(name)
, Value(value)
, m_Parent(NULL)
, m_FirstChild(NULL), m_LastChild(NULL)
, m_PrevSibling(NULL), m_NextSibling(NULL)
{

}

Node::~Node()
{
	delete m_FirstChild;
	delete m_NextSibling;
}

Node & Node::operator=( const Node &src )
{
	if (&src != this)
		CopyFrom(src);
	return *this;
}

void Node::CopyFrom( const Node &src )
{
	Name = src.Name;
	Value = src.Value;
	CopyChildrenFrom(src);
}

void Node::CopyChildrenFrom( const Node &src )
{
	DeleteAllChildren();
	for (Node *srcChild = src.GetFirstChild(); srcChild; srcChild = srcChild->GetNextSibling())
		LinkChildAtEnd(new Node(*srcChild));
}

void Node::MoveChildrenFrom( Node *src )
{
	DeleteAllChildren();
	for (Node *srcChild = src->GetFirstChild(); srcChild; srcChild = srcChild->GetNextSibling())
		srcChild->m_Parent = this;
	m_FirstChild = src->m_FirstChild;
	m_LastChild = src->m_LastChild;
	src->m_FirstChild = NULL;
	src->m_LastChild = NULL;
}

void Node::LoadChildren( Tokenizer &tok )
{
	DeleteAllChildren();

	Node *newChild;
	bool atEnd = tok.QueryEOF() || tok.QuerySymbol(_T('}'));
	while (!atEnd)
	{
		if (tok.QuerySymbol(_T(',')) || tok.QuerySymbol(_T(';')))
		{
			tok.Next();
			continue;
		}

		newChild = NULL;

		// Can be name
		if (tok.QueryToken(Tokenizer::TOKEN_IDENTIFIER, Tokenizer::TOKEN_STRING))
		{
			tstring s = tok.GetString();
			tok.Next();

			// It was a name
			if (tok.QuerySymbol(_T('=')))
			{
				tok.Next();
				// Name = Value
				if (tok.QueryToken(Tokenizer::TOKEN_IDENTIFIER)
					|| tok.QueryToken(Tokenizer::TOKEN_STRING)
					|| tok.QueryToken(Tokenizer::TOKEN_CHAR)
					|| tok.QueryToken(Tokenizer::TOKEN_INTEGER)
					|| tok.QueryToken(Tokenizer::TOKEN_FLOAT))
				{
					LinkChildAtEnd(newChild = new Node(s, tok.GetString()));
					tok.Next();
				}
				// No value
				else
					LinkChildAtEnd(newChild = new Node(s, tstring()));
			}
			// It was a value
			else
				LinkChildAtEnd(newChild = new Node(s));
		}
		// Must be value
		else if (tok.QueryToken(Tokenizer::TOKEN_CHAR)
			|| tok.QueryToken(Tokenizer::TOKEN_INTEGER)
			|| tok.QueryToken(Tokenizer::TOKEN_FLOAT))
		{
			LinkChildAtEnd(newChild = new Node(tok.GetString()));
			tok.Next();
		}

		// { Children... }
		if (tok.QuerySymbol(_T('{')))
		{
			tok.Next();

			if (newChild == NULL)
				LinkChildAtEnd(newChild = new Node());

			newChild->LoadChildren(tok);

			tok.AssertSymbol(_T('}'));
			tok.Next();
		}

		// Separator ',' or ';' (not needed at end)
		atEnd = tok.QueryEOF() || tok.QuerySymbol(_T('}'));
		bool isSeparator = atEnd ? false : (tok.QuerySymbol(_T(',')) || tok.QuerySymbol(_T(';')));
		if (isSeparator)
			tok.Next();
		else if (!atEnd)
			tok.CreateError();
	}
}

/*void Node::SaveChildren( TokenWriter &tok ) const // TODO
{
}*/

Node * Node::FindFirstChild( const tstring &name ) const
{
	Node *child = m_FirstChild;
	while (child && child->Name != name)
		child = child->GetNextSibling();
	return child;
}

Node * Node::FindLastChild( const tstring &name ) const
{
	Node *child = m_LastChild;
	while (child && child->Name != name)
		child = child->GetPrevSibling();
	return child;
}

Node * Node::FindPrevSibling( const tstring &name ) const
{
	Node *sibling = m_PrevSibling;
	if (sibling && sibling->Name != name)
		sibling = sibling->GetPrevSibling();
	return sibling;
}

Node * Node::FindNextSibling( const tstring &name ) const
{
	Node *sibling = m_NextSibling;
	if (sibling && sibling->Name != name)
		sibling = sibling->GetNextSibling();
	return sibling;
}

size_t Node::CalcChildCount() const
{
	size_t count = 0;
	for (Node *child = m_FirstChild; child; child = child->GetNextSibling())
		count++;
	return count;
}

size_t Node::CalcChildCount( const tstring &name ) const
{
	size_t count = 0;
	for (Node *child = m_FirstChild; child; child = child->GetNextSibling())
		if (child->Name == name)
			count++;
	return count;
}

void Node::Clear()
{
	Name.clear();
	Value.clear();
	DeleteAllChildren();
}

void Node::DeleteAllChildren()
{
	delete m_FirstChild;
	m_FirstChild = NULL;
	m_LastChild = NULL;
}

void Node::DeleteAllChildren( const tstring &name )
{
	Node *child, *nextChild;
	for (child = m_FirstChild; child; child = nextChild)
	{
		nextChild = child->GetNextSibling();
		if (child->Name == name)
			DeleteChild(child);
	}
}

void Node::InsertChildAtBegin( const Node &addThis )
{
	LinkChildAtBegin(new Node(addThis));
}

void Node::InsertChildAtEnd( const Node &addThis )
{
	LinkChildAtEnd(new Node(addThis));
}

void Node::InsertChildBefore( Node *beforeThis, const Node &addThis )
{
	LinkChildBefore(beforeThis, new Node(addThis));

}

void Node::InsertChildAfter( Node *afterThis, const Node &addThis )
{
	LinkChildAfter(afterThis, new Node(addThis));
}

void Node::DeleteChild( Node *deleteThis )
{
	UnlinkChild(deleteThis);
	delete deleteThis;
}

void Node::LinkChildAtBegin( Node *addThis )
{
	assert(addThis && addThis->m_Parent == NULL && addThis->m_PrevSibling == NULL && addThis->m_NextSibling == NULL);
	
	Node *beforeThis = m_FirstChild;
	
	addThis->m_Parent = this;
	addThis->m_NextSibling = beforeThis;
	
	m_FirstChild = addThis;
	if (beforeThis)
		beforeThis->m_PrevSibling = addThis;
	else
		m_LastChild = addThis;
}

void Node::LinkChildAtEnd( Node *addThis )
{
	assert(addThis && addThis->m_Parent == NULL && addThis->m_PrevSibling == NULL && addThis->m_NextSibling == NULL);
	
	Node *afterThis = m_LastChild;
	
	addThis->m_Parent = this;
	addThis->m_PrevSibling = afterThis;
	
	m_LastChild = addThis;
	if (afterThis)
		afterThis->m_NextSibling = addThis;
	else
		m_FirstChild = addThis;
}

void Node::LinkChildBefore( Node *beforeThis, Node *addThis )
{
	assert(beforeThis && beforeThis->m_Parent == this);
	assert(addThis && addThis->m_Parent == NULL && addThis->m_PrevSibling == NULL && addThis->m_NextSibling == NULL);
	
	Node *afterThis = beforeThis->m_PrevSibling;
	
	addThis->m_Parent = this;
	addThis->m_PrevSibling = afterThis;
	addThis->m_NextSibling = beforeThis;
	
	if (beforeThis)
		beforeThis->m_PrevSibling = addThis;
	else
		m_LastChild = addThis;
	if (afterThis)
		afterThis->m_NextSibling = addThis;
	else
		m_FirstChild = addThis;
}

void Node::LinkChildAfter( Node *afterThis, Node *addThis )
{
	assert(afterThis && afterThis->m_Parent == this);
	assert(addThis && addThis->m_Parent == NULL && addThis->m_PrevSibling == NULL && addThis->m_NextSibling == NULL);

	Node *beforeThis = afterThis->m_NextSibling;

	addThis->m_Parent = this;
	addThis->m_PrevSibling = afterThis;
	addThis->m_NextSibling = beforeThis;

	if (beforeThis)
		beforeThis->m_PrevSibling = addThis;
	else
		m_LastChild = addThis;
	if (afterThis)
		afterThis->m_NextSibling = addThis;
	else
		m_FirstChild = addThis;
}

void Node::UnlinkChild( Node *deleteThis )
{
	assert(deleteThis && deleteThis->m_Parent == this);

	Node *prev = deleteThis->m_PrevSibling;
	Node *next = deleteThis->m_NextSibling;

	if (prev)
		prev->m_NextSibling = next;
	else
		m_FirstChild = next;
	if (next)
		next->m_PrevSibling = prev;
	else
		m_LastChild = prev;

	deleteThis->m_Parent = NULL;
	deleteThis->m_PrevSibling = NULL;
	deleteThis->m_NextSibling = NULL;
}

Node & Node::MustFindFirstChild( const tstring &name ) const
{
	Node *node = FindFirstChild(name);
	if (node == NULL)
		throw Error(_T("TokDoc Node \"") + name + _T("\" not found"), __TFILE__, __LINE__);
	return *node;
}

void Node::SaveChildren( TokenWriter &tok ) const
{
	for (Node *child = GetFirstChild(); child; child = child->GetNextSibling())
	{
		if (child->Name.empty() && child->Value.empty() && !child->HasChildren())
			continue;

		if (!child->Name.empty())
		{
			tok.WriteString(child->Name);
			tok.WriteSymbol(_T('='));
		}

		if (!child->Value.empty())
			tok.WriteString(child->Value);

		if (child->HasChildren())
		{
			tok.WriteSymbol(_T('{'));
			tok.WriteEOL();
			child->SaveChildren(tok);
			tok.WriteSymbol(_T('}'));
		}

		tok.WriteSymbol(_T(';'));
		tok.WriteEOL();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Globals

void NodeValueFromEnum( Node &node, uint val, const tchar * const *itemNames, const uint *itemValues, uint itemCount )
{
	node.DeleteAllChildren();

	if (itemValues)
	{
		uint i;
		for (i = 0; i < itemCount; i++)
		{
			if (itemValues[i] == val)
			{
				node.Value = itemNames[i];
				return;
			}
		}
	}
	else
	{
		if (val < itemCount)
		{
			node.Value = itemNames[val];
			return;
		}
	}

	UintToStr(&node.Value, val);
}

void NodeFrom( Node &node, const POINT_ &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.y);
}

void NodeFrom( Node &node, const RECTI &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Min.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Min.y);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Max.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Max.y);
}

void NodeFrom( Node &node, const VEC2 &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.y);
}

void NodeFrom( Node &node, const VEC3 &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.y);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.z);
}

void NodeFrom( Node &node, const VEC4 &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.y);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.z);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.w);
}

void NodeFrom( Node &node, COLOR val )
{
	node.DeleteAllChildren();

	node.Value.reserve(10);
	tstring valStr;
	UintToStr2(&valStr, val.ARGB, 8, 16);
	node.Value = _T("0x");
	node.Value += valStr;
}

void NodeFrom( Node &node, const COLORF &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.R);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.G);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.B);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.A);
}

void NodeFrom( Node &node, const RECTF &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Min.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Min.y);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Max.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Max.y);
}

void NodeFrom( Node &node, const BOX &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Min.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Min.y);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Min.z);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Max.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Max.y);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.Max.z);
}

void NodeFrom( Node &node, const QUATERNION &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.x);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.y);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.z);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.w);
}

void NodeFrom( Node &node, const MATRIX &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;

	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._11);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._12);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._13);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._14);

	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._21);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._22);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._23);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._24);

	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._31);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._32);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._33);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._34);

	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._41);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._42);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._43);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val._44);
}

void NodeFrom( Node &node, const AFFINE2D &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;

	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.a);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.b);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.c);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.d);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.e);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.f);
}

void NodeFrom( Node &node, const LINE2D &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.a);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.b);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.c);
}

void NodeFrom( Node &node, const PLANE &val )
{
	node.DeleteAllChildren();
	node.Value.clear();

	Node *childNode;
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.a);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.b);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.c);
	node.LinkChildAtEnd(childNode = new Node()); NodeFrom(*childNode, val.d);
}

#ifdef WIN32

void NodeFrom( Node &node, GameTime val )
{
	NodeFrom(node, val.ToMilliseconds());
}

bool NodeTo( GameTime &out, const Node &node, bool required )
{
	__int64 milliseconds;
	if (!NodeTo(milliseconds, node, required))
		return false;
	out = MillisecondsToGameTime(milliseconds);
	return true;
}

#endif // #ifdef WIN32

bool NodeValueToEnum( uint &out, const Node &node, bool required, const tchar * const *itemNames, const uint *itemValues, uint itemCount )
{
	for (uint i = 0; i < itemCount; i++)
	{
		if (node.Value == itemNames[i])
		{
			out = itemValues ? itemValues[i] : i;
			return true;
		}
	}

	if (StrToUint(&out, node.Value) == 0)
		return true;

	if (required)
		throw Error(_T("Cannot parse TokDoc node \"") + node.Name + _T("\" = \"") + node.Value + _T("\" to enum."), __TFILE__, __LINE__);
	return false;
}

bool NodeTo( POINT_ &out, const Node &node, bool required )
{
	POINT_ tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 2; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( RECTI &out, const Node &node, bool required )
{
	RECTI tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 4; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( VEC2 &out, const Node &node, bool required )
{
	VEC2 tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 2; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( VEC3 &out, const Node &node, bool required )
{
	VEC3 tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 3; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( VEC4 &out, const Node &node, bool required )
{
	VEC4 tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 4; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( COLOR &out, const Node &node, bool required )
{
	return NodeTo(out.ARGB, node, required);
}

bool NodeTo( COLORF &out, const Node &node, bool required )
{
	COLORF tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 4; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( RECTF &out, const Node &node, bool required )
{
	RECTF tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 4; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( BOX &out, const Node &node, bool required )
{
	BOX tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 6; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( QUATERNION &out, const Node &node, bool required )
{
	QUATERNION tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 4; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( MATRIX &out, const Node &node, bool required )
{
	MATRIX tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 16; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( AFFINE2D &out, const Node &node, bool required )
{
	AFFINE2D tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 6; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( LINE2D &out, const Node &node, bool required )
{
	LINE2D tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 3; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

bool NodeTo( PLANE &out, const Node &node, bool required )
{
	PLANE tmpVal;
	uint index; Node *childNode;
	for (childNode = node.FindFirstChild(EMPTY_STRING), index = 0; childNode && index < 4; childNode = childNode->FindNextSibling(EMPTY_STRING), index++)
		if (!NodeTo(tmpVal[index], *childNode, required))
			return false;
	out = tmpVal;
	return true;
}

} // namespace tokdoc
} // namespace common
