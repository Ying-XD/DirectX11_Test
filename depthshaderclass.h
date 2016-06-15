////////////////////////////////////////////////////////////////////////////////
// Filename: depthshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include "global.h"
#include "Log.h"
#include <stdlib.h>
#include "baseShader.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: DepthShaderClass
////////////////////////////////////////////////////////////////////////////////

class DepthShaderClass
{
private:
	struct MatrixBufferType {
		D3DXMATRIX	worldMat;
		D3DXMATRIX	viewMat;
		D3DXMATRIX	projMat;
		D3DXMATRIX	lightViewMat;
		D3DXMATRIX	lightProjMat;
	};
	
	struct PosBufferType {
		D3DXVECTOR3 cameraPos;
		FLOAT		padding;
		D3DXVECTOR3 lightPos;
		FLOAT		padding2;
	};
public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount,
				D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix,
				D3DXVECTOR3 cameraPos, ShaderType texType);
	bool Render_ShadowDepth(ID3D11DeviceContext* context, int indexCount,
				D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,D3DXMATRIX projMatrix,
				D3DXMATRIX lightViewMatrix, D3DXMATRIX ligthProjMatrtix, D3DXVECTOR3 lightPos,
				ID3D11ShaderResourceView* texture,
				ShaderType texType = ShaderType::ShadowDepth);


private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	bool InitialiseShadowDepShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
							 D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjMatrix,
							 D3DXVECTOR3 cameraPos, D3DXVECTOR3 lightPos, 
							 ID3D11ShaderResourceView* texture);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, ShaderType texType);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*	m_pixelShader;
	ID3D11PixelShader*	m_nvPShader;

	ID3D11VertexShader* m_shadowDepVS;
	ID3D11PixelShader*	m_shadowDepPS;

	ID3D11InputLayout*	m_layout;
	ID3D11InputLayout*	m_layout2;

	ID3D11Buffer*		m_matrixBuffer;
	ID3D11Buffer*		m_posBuffer;

	ID3D11SamplerState* m_samplerClamp;

	BOOL				m_flag;

};

