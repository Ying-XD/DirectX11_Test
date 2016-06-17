#include "BoneNode.h"



BoneNode::BoneNode()
	: _name(""),_pFirstChild(NULL), _pSibling(NULL), _pFather(NULL)
{ }


BoneNode::~BoneNode() { }

BoneNode::BoneNode(std::string name)
	:_name(name), _pFirstChild(NULL), _pSibling(NULL), _pFather(NULL) 
{ }

void BoneNode::SetFirstChild(BoneNode * child) {
	_pFirstChild = child;
	child->_pFather =  this;
}

void BoneNode::SetSibling(BoneNode * sibling) {
	_pSibling = sibling;
	sibling->_pFather = _pFather;
}




//	----------- BoneTree ----------- 
BoneTree::BoneTree() { }

BoneTree::BoneTree(BoneNode * root) :m_root(root){
	SetBonesMap(root);
}

BoneTree::~BoneTree() {
	DestoryNode(m_root);
}

BoneNode * BoneTree::GetBoneNodeFromName(std::string name) {
	auto itr = m_mapBones.find(name);
	if ( itr != m_mapBones.end())
		return itr->second;
	return nullptr;
}

void BoneTree::SetRoot(BoneNode * root) {
	if (m_root)
		DestoryNode(m_root);
	m_root = root;
	SetBonesMap(root);
}

void BoneTree::SetBonesMap(BoneNode * node) {
	m_mapBones[node->_name] = node;
	if (node->_pFirstChild)
		SetBonesMap(node->_pFirstChild);
	if (node->_pSibling)
	SetBonesMap(node->_pSibling);
}

void BoneTree::DestoryNode(BoneNode * node) {
	if (node->_pFirstChild)
		DestoryNode(node->_pFirstChild);
	if (node->_pSibling)
		DestoryNode(node->_pSibling);
	delete node;
}
