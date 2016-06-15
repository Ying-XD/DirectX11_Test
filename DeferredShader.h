#pragma once
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX10math.h>
#include "global.h"
#include "Log.h"
#include "baseShader.h"

class DeferredShader {
private:
	struct VS_CBufferType {
		D3DXMATRIX	worldMat;
		D3DXMATRIX	viewMat;
		D3DXMATRIX	projMat;
		D3DXVECTOR3 lightPos;
		FLOAT		padding;
	};
	struct PS_CBufferType {
		D3DXVECTOR4	ambientColor;
		D3DXVECTOR4	diffuseColor;
	};
	struct Tess_CBufferType {
		D3DXMATRIX	worldMat;
		D3DXMATRIX	viewMat;
		D3DXMATRIX	projMat;
		FLOAT		tessAmount;
		D3DXVECTOR3	padding;
	};
public:
	DeferredShader();
	~DeferredShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render_Color(ID3D11DeviceContext* context, int indexCount,
					  D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
					  D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
					  ID3D11ShaderResourceView* texture);
private:
	bool InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	
	bool SetShaderParameters_Color(ID3D11DeviceContext * context,D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
								   D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
								   ID3D11ShaderResourceView* texture);

	void RenderShader_Color(ID3D11DeviceContext* context, int indexCount);

private:
	ID3D11VertexShader *	m_vertexShader;
	ID3D11PixelShader*		m_pixelShader;
	ID3D11SamplerState *	m_samplerWrap;
	ID3D11InputLayout*		m_layout;

	ID3D11Buffer* m_vsCBuffer, *m_psCBuffer;
};
