////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <vector>
#include "d3dclass.h"
#include "global.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	//bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT);
	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT, UINT viewSize = 1);
	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, DXGI_FORMAT* pFormats, UINT viewSize);

	void Shutdown();
	void SetTextureType(TextureType type);
	TextureType GetTextureType();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f);
	ID3D11ShaderResourceView* GetShaderResourceView(UINT index = 0);

	void BeginRender(D3DClass* d3d, float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f);
	void EndRender(D3DClass* d3d);
private:
	TextureType	m_textureType;
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;


	UINT	m_viewSize;
	//ID3D11Texture2D**			m_renderTargetTextureArr;
	//ID3D11RenderTargetView**	m_renderTargetViewArr;
	//ID3D11ShaderResourceView**	m_shaderResourceViewArr;

	std::vector<ID3D11Texture2D*>			m_renderTargetTextureArr;
	std::vector<ID3D11RenderTargetView*>	m_renderTargetViewArr;
	std::vector<ID3D11ShaderResourceView*>	m_shaderResourceViewArr;
};
