#pragma once
#include <string>
#include <D3DX10math.h>
class BoneNode {
private:
	std::string	m_name;
	BoneNode*	m_pFirstChild;
	BoneNode*	m_pSibling;
	BoneNode*	m_pFather;
	D3DXMATRIX	m_transformMatrix;
	D3DXMATRIX	m_offsetMatrix;

public:
	BoneNode();
	~BoneNode();
	BoneNode(std::string name);

	void SetFirstChild(BoneNode* child);
	void SetSibling(BoneNode* sibling);

	BoneNode* GetFirstChild();
	BoneNode* GetSibling();
	BoneNode* GetFather();

	void SetTransfromMatrix(D3DXMATRIX transformMatrix);
	void SetOffsetMatrix(D3DXMATRIX offsetMatrix);
};

