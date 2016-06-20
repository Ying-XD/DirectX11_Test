#pragma once
#include <D3D11.h>
#include <d3dx10math.h>
#include "BoneNode.h"
#include "parser_header.h"


#include "textureclass.h"
class BW_ModelClass {
public:
	BW_ModelClass();
	~BW_ModelClass();
	
	bool LoadModel(ID3D11Device* device, const VerticesAsset& vAssets, const IndicesAsset& iAssets);
	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	void SetBoneList(const BoneNodeList& bonelist);

	void Shutdown();

	void SetPosition(D3DXVECTOR3 pos);
	void GetPosition(FLOAT& x, FLOAT& y, FLOAT& z);
	void Render(ID3D11DeviceContext* context);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
private:

	UINT UnpackVertex(void** pData, const VerticesAsset & vAssets);
	template<typename T>
	void* UnpackVertex__(const VerticesAsset& vAssets);
	template<typename TOUT, typename TIN>
	void* UnpackVertex__(const VerticesAsset& vAssets);

	//bool InitializeBuffers(ID3D11Device* device);
private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	size_t m_vertexCount, m_indexCount;
	TextureClass *m_Texture;
	D3DXVECTOR3	  m_pos;
	UINT		  m_vertexStride;
	DXGI_FORMAT	  m_indexFormat;
	void*		  m_pData;
public:
	BoneNodeList	m_boneNodeList;
};

template<typename TOUT, typename TIN>
inline void * BW_ModelClass::UnpackVertex__(const VerticesAsset& vAssets) {
	UINT	nVertices = vAssets.data.size() / vAssets.typeInfo().stride;
	TOUT* pData = new TOUT[nVertices];
	UINT  stride = vAssets.typeInfo().stride;
	const auto & vdata = vAssets.data.data();
	for (UINT i = 0, j = 0;i < nVertices;i++, j += stride) {
		pData[i] = TOUT(*(TIN*)(vdata + j));
	}

	return pData;
}
template<typename T>
inline void* BW_ModelClass::UnpackVertex__(const VerticesAsset& vAssets) {
	size_t	nVertices = vAssets.data.size() / vAssets.typeInfo().stride;
	T *pData = new T[nVertices];
	memcpy(pData, vAssets.data.data(), vAssets.data.size());
	return pData;
}
