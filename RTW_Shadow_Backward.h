#pragma once
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include "Log.h"
#include "global.h"
#include "baseShader.h"

class RTW_Shadow_Backward {
private:
	struct DeferredPSBuffer {
		D3DXMATRIX	cv2lvMatr;
		D3DXVECTOR4	ambientColor;
		D3DXVECTOR2	shadow_texel_size;
		D3DXVECTOR2	padding;
	};

public:
	RTW_Shadow_Backward();
	~RTW_Shadow_Backward();
	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, D3DXMATRIX cv2lvMatr, D3DXVECTOR4 ambientColor,
				ID3D11ShaderResourceView * warpMaps, ID3D11ShaderResourceView* colorTex, ID3D11ShaderResourceView* diffuseTex,
				ID3D11ShaderResourceView* lightDepTex, ID3D11ShaderResourceView* viewDepTex);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd,
								   WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext* context, D3DXMATRIX cv2lvMatr, D3DXVECTOR4 ambientColor,
							 ID3D11ShaderResourceView * warpMaps, ID3D11ShaderResourceView* colorTex, ID3D11ShaderResourceView* diffuseTex,
							 ID3D11ShaderResourceView * lightDepTex, ID3D11ShaderResourceView* viewDepTex);
	void RenderShader(ID3D11DeviceContext* context);
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*	m_pixelShader;
	ID3D11Buffer*		m_deferPSBuf;

	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
};

