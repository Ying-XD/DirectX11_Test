#include "BW_ModelClass.h"



BW_ModelClass::BW_ModelClass() {
	m_vertexBuffer	= NULL;
	m_indexBuffer	= NULL;
	m_Texture		= NULL;
	m_pData			= NULL;

	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}


BW_ModelClass::~BW_ModelClass() {
}

bool BW_ModelClass::LoadModel(ID3D11Device * device, const VerticesAsset & vAssets, const IndicesAsset & iAssets) {
	m_vertexStride = UnpackVertex(&m_pData, vAssets);
	m_vertexCount = vAssets.data.size() / vAssets.typeInfo().stride;

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.ByteWidth = m_vertexCount * m_vertexStride;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA	vertexData = {};
	vertexData.pSysMem = m_pData;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	CHECK_HRESULT(device->CreateBuffer(&vertexBufDesc, &vertexData, &m_vertexBuffer));

	bool is_list16 = (iAssets.format == "list") ? (true) : (false);
	m_indexCount = (is_list16) ? iAssets.index16.size() : iAssets.index32.size();
	m_indexFormat = (is_list16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.ByteWidth = (is_list16) ? (2 * m_indexCount) : (4 * m_indexCount);
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = (is_list16) ? (void*)iAssets.index16.data() : (void*)iAssets.index32.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	CHECK_HRESULT(device->CreateBuffer(&indexBufDesc, &indexData, &m_indexBuffer));
	return true;
}

bool BW_ModelClass::LoadTexture(ID3D11Device * device, WCHAR * filename) {
	m_Texture = new TextureClass;
	if (!m_Texture || !m_Texture->Initialize(device, filename))
		return false;
	return true;
}

void BW_ModelClass::SetBoneList(const BoneNodeList & bonelist) {
	m_boneNodeList = bonelist;
}

void BW_ModelClass::Shutdown() {
	SHUTDOWN_DELETE(m_Texture);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_DELETE(m_pData);
}

void BW_ModelClass::GetPosition(FLOAT & x, FLOAT & y, FLOAT & z) {
	x = m_pos.x;
	y = m_pos.y;
	z = m_pos.z;
}

void BW_ModelClass::Render(ID3D11DeviceContext * context) {
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_vertexStride, 0);
	context->IASetIndexBuffer(m_indexBuffer, m_indexFormat, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int BW_ModelClass::GetIndexCount() {
	return m_indexCount;
}

ID3D11ShaderResourceView * BW_ModelClass::GetTexture() {
	return m_Texture->GetTexture();
}

UINT BW_ModelClass::UnpackVertex(void** pData, const VerticesAsset & vAssets) {
	SAFE_DELETE(*pData);
	const std::string & format = vAssets.format;
	if (format == "xyznuv") {
		*pData = UnpackVertex__<VertexXYZNUV>(vAssets);
		return sizeof(VertexXYZNUV);
	}
	else if (format == "xyznuvtb") {
		*pData = UnpackVertex__<VertexXYZNUVTBPC, VertexXYZNUVTB>(vAssets);
		return sizeof(VertexXYZNUVTBPC);
	}
	else if (format == "xyznuviiiww") {
		*pData = UnpackVertex__<VertexXYZNUVIIIWWPC, VertexXYZNUVIIIWW>(vAssets);
		return sizeof(VertexXYZNUVIIIWWPC);
	}
	else if (format == "xyznuviiiwwtb") {
		*pData = UnpackVertex__<VertexXYZNUVIIIWWTBPC, VertexXYZNUVIIIWWTB>(vAssets);
		return sizeof(VertexXYZNUVIIIWWTBPC);
	}
	else if (format == "xyznuviiiiwww") {
		*pData = UnpackVertex__<VertexXYZNUVIIIIWWWPC, VertexXYZNUVIIIIWWW>(vAssets);
		return sizeof(VertexXYZNUVIIIIWWWPC);
	}
	else if (format == "xyznuviiiiwwwtb") {
		*pData = UnpackVertex__<VertexXYZNUVIIIIWWWTBPC, VertexXYZNUVIIIIWWWTB>(vAssets);
		return  sizeof(VertexXYZNUVIIIIWWWTBPC);
	}
	
	return 0;
}
