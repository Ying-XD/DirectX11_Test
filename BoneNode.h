#pragma once
#include <string>
#include <D3DX10math.h>
#include <map>
#include <vector>

struct BoneNode {
public:
	std::string	_name;
	BoneNode*	_pFirstChild;
	BoneNode*	_pSibling;
	BoneNode*	_pFather;
	D3DXMATRIX	_transformMatrix;
	D3DXMATRIX	_offsetMatrix;

public:
	BoneNode();
	~BoneNode();
	BoneNode(std::string name);

	void SetFirstChild(BoneNode* child);
	void SetSibling(BoneNode* sibling);

};

class BoneTree {
private:
	BoneNode*	m_root;
	std::map<std::string, BoneNode*>	m_mapBones;
public:
	BoneTree();
	BoneTree(BoneNode* root);
	~BoneTree();
	BoneNode* GetBoneNodeFromName(std::string name);
	void SetRoot(BoneNode* root);

private:
	void SetBonesMap(BoneNode* node);
	void DestoryNode(BoneNode* node);
};
