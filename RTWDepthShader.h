#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include "global.h"
#include "Log.h"
#include <stdlib.h>


class RTWDepthShader {
private:
	struct CBufferType {
		D3DXMATRIX	worldMatrix;
		D3DXMATRIX	viewMatrix;
		D3DXMATRIX	projMatrix;
	};
	struct Tess_CBufferType {
		D3DXMATRIX	worldMatrix;
		D3DXMATRIX	viewMatrix;
		D3DXMATRIX	projMatrix;
		FLOAT		tessAmount;
		D3DXVECTOR3	padding;
	};
public:
	RTWDepthShader();
	~RTWDepthShader();

	bool Initialize(ID3D11Device* device);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, 
				D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix,
				ID3D11ShaderResourceView* warpMaps);

	bool Render_Tessellation(ID3D11DeviceContext* context, int indexCount,
					 D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix,
					 ID3D11ShaderResourceView* warpMaps, float tessAmount);
	void ShutdownTess(ID3D11DeviceContext* context);
private:
	bool InitializeShader(ID3D11Device* device, WCHAR * vsFilename, WCHAR * psFilename);
	bool InitializeShader_Tess(ID3D11Device* device, WCHAR * vsFilename, WCHAR * psFilename, WCHAR* hsFilename, WCHAR* dsFilename);

	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext* context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix,
							 ID3D11ShaderResourceView* warpMaps);
	bool SetShaderParameters_Tess(ID3D11DeviceContext* context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix,
							 ID3D11ShaderResourceView* warpMaps,
								  FLOAT tessAmount);

	void RenderShader(ID3D11DeviceContext* context, int indexCount);
	void RenderShader_Tess(ID3D11DeviceContext* context, int indexCount);


private:
	ID3D11VertexShader* m_vertexShader0;
	ID3D11PixelShader*	m_pixelShader0;
	ID3D11InputLayout*	m_layout0;

	ID3D11VertexShader*	m_vertexShader;
	ID3D11HullShader*	m_hullShader;
	ID3D11DomainShader*	m_domainShader;
	ID3D11PixelShader*	m_pixelShader;
	ID3D11InputLayout*	m_layout;
	ID3D11Buffer*		m_CBuffer, * m_tessCBuffer;

	ID3D11SamplerState*	m_SamplerBorder;
};

