#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include "Log.h"
#include "global.h"
#include "baseShader.h"


class ShadowShaderClass {
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
		UINT32		rtwFlag;
		D3DXVECTOR3 padding;
	};

	struct VS_LightBufferType
	{
		D3DXVECTOR3 lightPosition;
		UINT32		smWidth;
	};
	
//	----	tessellation	----
private:
	struct TessMatrixBufferType {
		D3DXMATRIX	worldMatr;
		D3DXMATRIX	viewMatr;
		D3DXMATRIX	projMatr;
		D3DXMATRIX	lightViewMatr;
		D3DXMATRIX	lightProjMatr;
		D3DXVECTOR3 lightPos;
		UINT32		smWidth;
	};
	struct TessBufferType {
		FLOAT		tessAmount;
		D3DXVECTOR3 padding;
	};
	struct TessLightBufferType {
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
	};



public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void SetLightParameters(D3DXVECTOR3 lightPosition,	D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor);

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
				D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
				D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
				ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* warpMaps = NULL);
	bool Render_Tess(ID3D11DeviceContext* deviceContext, int indexCount,
					 D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
					 D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
					 ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* warpMaps,
					 FLOAT tessAmount);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, 
						  WCHAR* vsFilename, WCHAR* psFilename);
	bool InitializeShader_Tess(ID3D11Device* device, HWND hwnd,
							   WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename,	WCHAR* psFilename);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
							 ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* warpMap,
							 D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor);

	bool SetShaderParameters_Tess(ID3D11DeviceContext* context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjMatrix,
								  ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* warpMap,
								  D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
								  FLOAT tessAmount
								  );
	void RenderShader(ID3D11DeviceContext*, int);
	void RenderShader_Tess(ID3D11DeviceContext* context, int indexCount);
	
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*	m_pixelShader;

	ID3D11InputLayout*	m_layout;

	ID3D11Buffer*		m_matrixBuffer;
	ID3D11Buffer*		m_lightBuffer;
	ID3D11Buffer*		m_lightBuffer2;

	D3DXVECTOR3 m_lightPos;
	D3DXVECTOR4 m_ambientColor, m_diffuseColor;

	//	----	tessellation	----
	ID3D11VertexShader* m_tessVS;
	ID3D11HullShader*	m_tessHS;
	ID3D11DomainShader*	m_tessDS;
	ID3D11PixelShader*	m_tessPS;
	ID3D11InputLayout*	m_tessLayout;

	ID3D11Buffer*		m_hsBuffer;
	ID3D11Buffer*		m_dsBuffer;
	ID3D11Buffer*		m_psBuffer;

	//	----	Sampler		----
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11SamplerState* m_sampleStateBorder;

};

