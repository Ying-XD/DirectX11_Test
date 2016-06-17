#pragma once
#include <D3D11.h>
#include <d3dx10math.h>

#include "parser_header.h"
#include "textureclass.h"
class BW_ModelClass {
public:
	BW_ModelClass();
	~BW_ModelClass();
	
	bool Initialize(ID3D11Device* device, const VerticesAsset& vAssets, const IndicesAsset& iAssets);
private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	size_t m_vertexCount, m_indexCount;
	TextureClass *m_Texture;
	D3DXVECTOR3	  m_pos;
};

