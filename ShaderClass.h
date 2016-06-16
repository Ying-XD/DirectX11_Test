#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include "Log.h"
#include "global.h"
#include "baseShader.h"


class ShaderClass {
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
	};

	struct PS_LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
	};

	struct VS_LightBufferType
	{
		D3DXVECTOR3 lightPosition;
		UINT32		smWidth;
	};
	
public:
	ShaderClass();
	ShaderClass(const ShaderClass&);
	~ShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void SetLightParameters(D3DXVECTOR3 lightPosition,	D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor);

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
				D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
				D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
				ID3D11ShaderResourceView* texture);
private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, 
						  WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
							 ID3D11ShaderResourceView* texture, 
							 D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor);

	void RenderShader(ID3D11DeviceContext*, int);
	
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*	m_pixelShader;

	ID3D11InputLayout*	m_layout;

	ID3D11Buffer*		m_matrixBuffer;
	ID3D11Buffer*		m_lightBuffer;
	ID3D11Buffer*		m_lightBuffer2;

	D3DXVECTOR3 m_lightPos;
	D3DXVECTOR4 m_ambientColor, m_diffuseColor;


	//	----	Sampler		----
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11SamplerState* m_sampleStateBorder;

};

