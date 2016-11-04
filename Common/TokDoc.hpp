/** \file
\brief Persistent DOM model for a format based on Tokenizer.
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_TokDoc \n
Module components: \ref code_tokdoc
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_TOKDOC_H_
#define COMMON_TOKDOC_H_

#include "Math.hpp"

namespace common {

#ifdef WIN32
struct GameTime;
#endif

class Tokenizer;
class TokenWriter;

/** \addtogroup code_tokdoc TokDoc Module
Documentation: \ref Module_TokDoc \n
Header: TokDoc.hpp */
//@{
namespace tokdoc {

/// Main class that contains a tree node of TokDoc document. Can have name, value and subnodes.
class Node
{
public:
	tstring Name, Value;

	Node();
	Node(const Node &src);
	Node(const tstring &value);
	Node(const tstring &name, const tstring &value);
	~Node();

	Node & operator = (const Node &src);

	void CopyFrom(const Node &src);
	void CopyChildrenFrom(const Node &src);
	void MoveChildrenFrom(Node *src);

	/** \param tok Must be configured WITHOUT flag FLAG_TOKEN_EOL. */
	void LoadChildren(Tokenizer &tok);
	void SaveChildren(TokenWriter &tok) const;

	Node * GetParent() const { return m_Parent; }
	Node * GetFirstChild() const { return m_FirstChild; }
	Node * GetLastChild() const { return m_LastChild; }
	Node * GetPrevSibling() const { return m_PrevSibling; }
	Node * GetNextSibling() const { return m_NextSibling; }

	Node * FindFirstChild(const tstring &name) const;
	Node * FindLastChild(const tstring &name) const;
	Node * FindPrevSibling(const tstring &name) const;
	Node * FindNextSibling(const tstring &name) const;
	Node & MustFindFirstChild(const tstring &name) const;

	size_t CalcChildCount() const;
	size_t CalcChildCount(const tstring &name) const;
	bool HasChildren() const { return m_FirstChild != NULL; }

	void Clear();
	void DeleteAllChildren();
	void DeleteAllChildren(const tstring &name);

	void InsertChildAtBegin(const Node &addThis);
	void InsertChildAtEnd(const Node &addThis);
	void InsertChildBefore(Node *beforeThis, const Node &addThis);
	void InsertChildAfter(Node *afterThis, const Node &addThis);
	void DeleteChild(Node *deleteThis);

	void LinkChildAtBegin(Node *addThis);
	void LinkChildAtEnd(Node *addThis);
	void LinkChildBefore(Node *beforeThis, Node *addThis);
	void LinkChildAfter(Node *afterThis, Node *addThis);
	void UnlinkChild(Node *deleteThis);

private:
	Node *m_Parent;
	Node *m_FirstChild, *m_LastChild;
	Node *m_PrevSibling, *m_NextSibling;
};

/// Converts value of any type (supported by SthToStr) to string stored into Value of given node.
template <typename T>
inline void NodeValueFromSth(Node &node, const T &val)
{
	SthToStr<T>(&node.Value, val);
}

/// Parses string from Value of given node to value of any type (supported by StrToSth).
template <typename T>
inline bool NodeValueToSth(T &out, const Node &node, bool required)
{
	if (required)
		MustStrToSth<T>(&out, node.Value);
	else
	{
		T tmpVal;
		if (StrToSth(&tmpVal, node.Value))
			out = tmpVal;
		else
			return false;
	}
	return true;
}

/// Converts enumeration value (given as uint) into string with the value name stored into Value of given node.
/** If val is other than values of items given, val is converted to numeric string, like "123".
\param itemValues Can be NULL, which means consecutive items have values 0, 1, 2, ... */
void NodeValueFromEnum(Node &node, uint val,
	const tchar * const *itemNames, const uint *itemValues, uint itemCount);
/// Parses string from Value of given node to enumeration value (given as uint&).
/** node.Value may be one of given item names or a numeric string, like "123".
\param itemValues Can be NULL, which means consecutive items have values 0, 1, 2, ... */
bool NodeValueToEnum(uint &out, const Node &node, bool required,
	const tchar * const *itemNames, const uint *itemValues, uint itemCount);

template <typename T>
inline void NodeFromVector(Node &node, const std::vector<T> &vec)
{
	node.DeleteAllChildren();
	node.Value.clear();

	for (size_t i = 0; i < vec.size(); i++)
	{
		Node *childNode = new Node();
		node.LinkChildAtEnd(childNode);
		NodeFrom(*childNode, vec[i]);
	}
}

template <typename T>
inline bool NodeToVector(std::vector<T> &out, const Node &node, bool required)
{
	out.clear();
	T tmpVal;
	for (Node *childNode = node.FindFirstChild(EMPTY_STRING); childNode; childNode = childNode->FindNextSibling(EMPTY_STRING))
	{
		if (!NodeTo(tmpVal, childNode->Value, required))
			return false;
		out.push_back(tmpVal);
	}
	return true;
}

/// \internal
inline bool _StrIsUintWithBase(const tstring &s)
{
	return s.length() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X');
}

/// Parses Value string of given node to unsigned integer of type T, while supporting standard numbers like "123" as well as hexadecimal notation like "0x123FF".
template <typename T>
inline bool NodeValueToUint(T &out, const Node &node, bool required)
{
	if (_StrIsUintWithBase(node.Value))
	{
		tstring valStr = node.Value.substr(2);
		bool ok = StrToUint(&out, valStr, 16) == 0;
		if (required && !ok)
			throw Error(_T("Cannot parse TokDoc node \"") + node.Name + _T("\" = \"") + node.Value + _T("\" to unsigned integer."), __TFILE__, __LINE__);
		return ok;
	}
	else
		return NodeValueToSth(out, node, required);
}

template <typename T, typename UT>
inline bool NodeValueToInt(T &out, const Node &node, bool required)
{
	if (_StrIsUintWithBase(node.Value))
		return NodeValueToUint((UT&)out, node, required);
	else
		return NodeValueToSth(out, node, required);
}

inline void NodeFrom(Node &node, bool val) { NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, tchar val) { node.DeleteAllChildren(); node.Value.resize(1); node.Value[0] = val; }
inline void NodeFrom(Node &node, const tstring &val) { node.DeleteAllChildren(); node.Value = val; }
inline void NodeFrom(Node &node, const tchar *val) { node.DeleteAllChildren(); node.Value = val; }
inline void NodeFrom(Node &node, uint8 val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, uint16 val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, uint32 val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, uint64 val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, int16 val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, int32 val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, int64 val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, unsigned long val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, long val) {node.DeleteAllChildren();  NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, float val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }
inline void NodeFrom(Node &node, double val) { node.DeleteAllChildren(); NodeValueFromSth(node, val); }

inline bool NodeTo(bool &out, const Node &node, bool required) { return NodeValueToSth(out, node, required); }
inline bool NodeTo(tchar &out, const Node &node, bool required) { return NodeValueToSth(out, node, required); }
inline bool NodeTo(tstring &out, const Node &node, bool required) { out = node.Value; return true; }
inline bool NodeTo(uint8 &out, const Node &node, bool required) { return NodeValueToUint(out, node, required); }
inline bool NodeTo(uint16 &out, const Node &node, bool required) { return NodeValueToUint(out, node, required); }
inline bool NodeTo(uint32 &out, const Node &node, bool required) { return NodeValueToUint(out, node, required); }
inline bool NodeTo(uint64 &out, const Node &node, bool required) { return NodeValueToUint(out, node, required); }
inline bool NodeTo(unsigned long &out, const Node &node, bool required) { return NodeValueToUint(out, node, required); }
inline bool NodeTo(int16 &out, const Node &node, bool required) { return NodeValueToInt<int16, uint16>(out, node, required); }
inline bool NodeTo(int32 &out, const Node &node, bool required) { return NodeValueToInt<int32, uint32>(out, node, required); }
inline bool NodeTo(int64 &out, const Node &node, bool required) { return NodeValueToInt<int64, uint64>(out, node, required); }
inline bool NodeTo(long &out, const Node &node, bool required) { return NodeValueToInt<long, unsigned long>(out, node, required); }
inline bool NodeTo(float &out, const Node &node, bool required) { return NodeValueToSth(out, node, required); }
inline bool NodeTo(double &out, const Node &node, bool required) { return NodeValueToSth(out, node, required); }

// ======== Time types ========

#ifdef WIN32
void NodeFrom(Node &node, GameTime val);
bool NodeTo(GameTime &out, const Node &node, bool required);
#endif // #ifdef WIN32

// ======== Math types ========

void NodeFrom(Node &node, const POINT_ &val);
void NodeFrom(Node &node, const RECTI &val);
void NodeFrom(Node &node, const VEC2 &val);
void NodeFrom(Node &node, const VEC3 &val);
void NodeFrom(Node &node, const VEC4 &val);
void NodeFrom(Node &node, COLOR val);
void NodeFrom(Node &node, const COLORF &val);
void NodeFrom(Node &node, const RECTF &val);
void NodeFrom(Node &node, const BOX &val);
void NodeFrom(Node &node, const QUATERNION &val);
void NodeFrom(Node &node, const MATRIX &val);
void NodeFrom(Node &node, const AFFINE2D &val);
void NodeFrom(Node &node, const LINE2D &val);
void NodeFrom(Node &node, const PLANE &val);

bool NodeTo(POINT_ &out, const Node &node, bool required);
bool NodeTo(RECTI &out, const Node &node, bool required);
bool NodeTo(VEC2 &out, const Node &node, bool required);
bool NodeTo(VEC3 &out, const Node &node, bool required);
bool NodeTo(VEC4 &out, const Node &node, bool required);
bool NodeTo(COLOR &out, const Node &node, bool required);
bool NodeTo(COLORF &out, const Node &node, bool required);
bool NodeTo(RECTF &out, const Node &node, bool required);
bool NodeTo(BOX &out, const Node &node, bool required);
bool NodeTo(QUATERNION &out, const Node &node, bool required);
bool NodeTo(MATRIX &out, const Node &node, bool required);
bool NodeTo(AFFINE2D &out, const Node &node, bool required);
bool NodeTo(LINE2D &out, const Node &node, bool required);
bool NodeTo(PLANE &out, const Node &node, bool required);


template <typename T>
void SubnodeFrom(Node &node, const tstring &subnodeName, const T &val)
{
	Node *subnode = node.FindFirstChild(subnodeName);
	if (subnode == NULL)
		node.LinkChildAtEnd(subnode = new Node(subnodeName, EMPTY_STRING));
	NodeFrom(*subnode, val);
}

/// Condition for SubnodeTo() telling about how to treat errors.
enum SUBNODE_COND
{
	SUBNODE_OPTIONAL_CORRECT, ///< Optional, but must be correct if exists. Subnode not exists: return false. Parse error: throw exception.
	SUBNODE_REQUIRED,         ///< Strictly required. Subnode not exists or parse error: throw exception.
	SUBNODE_OPTIONAL,         ///< Libearlly optional. Subnode not exists or parse error: return false.
};

template <typename T>
bool SubnodeTo(T &out, const Node &node, const tstring &subnodeName, SUBNODE_COND required)
{
	Node *subnode = node.FindFirstChild(subnodeName);
	if (subnode == NULL)
	{
		if (required == SUBNODE_REQUIRED)
			throw Error(_T("TokDoc subnode \"") + subnodeName + _T("\" not found"), __TFILE__, __LINE__);
		return false;
	}
	return NodeTo(out, *subnode, required != SUBNODE_OPTIONAL);
}


//@}
// code_tokdoc
} // namespace tokdoc
} // namespace common

#endif
