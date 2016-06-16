#include "BoneNode.h"



BoneNode::BoneNode()
	: m_name(""),m_pFirstChild(NULL), m_pSibling(NULL), m_pFather(NULL)
{ }


BoneNode::~BoneNode() { }

BoneNode::BoneNode(std::string name)
	:m_name(name), m_pFirstChild(NULL), m_pSibling(NULL), m_pFather(NULL) 
{ }

void BoneNode::SetFirstChild(BoneNode * child) {
	m_pFirstChild = child;
	child->m_pFather =  this;
}

void BoneNode::SetSibling(BoneNode * sibling) {
	m_pSibling = sibling;
	sibling->m_pFather = m_pFather;
}

inline BoneNode * BoneNode::GetFirstChild() {
	return m_pFirstChild;
}

inline BoneNode * BoneNode::GetSibling() {
	return m_pSibling;
}

inline BoneNode * BoneNode::GetFather() {
	return m_pFather;
}

inline void BoneNode::SetTransfromMatrix(D3DXMATRIX transformMatrix) {
	m_transformMatrix = transformMatrix;
}

inline void BoneNode::SetOffsetMatrix(D3DXMATRIX offsetMatrix) {
	m_offsetMatrix = offsetMatrix;
}
