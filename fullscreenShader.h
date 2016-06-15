#pragma once
#include <d3dx11async.h>
#include "global.h"
#include "rendertextureclass.h"
#include "Log.h"
#include "baseShader.h"

class FullscreenShader{
private:
	struct ConstBufferType {
		FLOAT		sm_width;
		D3DXVECTOR3	padding;
	};
public:
	FullscreenShader();
	~FullscreenShader();

	bool Initialize(ID3D11Device* device);

	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* renderTex, TextureType type);

private:
	bool InitializeShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* context, ID3D11ShaderResourceView* renderTex);
	void RenderShader(ID3D11DeviceContext* context, TextureType type);
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader, *m_pixelShader2, *m_psRGBA;
	ID3D11SamplerState* m_sampleStateClamp;

	ID3D11Buffer*	m_constBuffer;
};

