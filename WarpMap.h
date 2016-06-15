#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include <d3dx11async.h>
#include "Log.h"
#include "global.h"


class WarpMapShader {
	struct ConstBufferType {
		FLOAT	d_texSize;
		INT32	sm_width;
		FLOAT	blur_factor;
		INT32   blur_width;
	};
public:
	WarpMapShader();
	~WarpMapShader();

	bool Initialize(ID3D11Device* device);

	void Shutdown();
	bool Comapct(ID3D11DeviceContext *context, ID3D11ShaderResourceView* tex);
	bool Blur(ID3D11DeviceContext *context, ID3D11ShaderResourceView* tex);
	bool BuildWarpMap(ID3D11DeviceContext *context, ID3D11ShaderResourceView* tex);

private:
	bool CompileVS(ID3D11Device * device, ID3D11VertexShader *& vs, WCHAR * vsFilename, LPCSTR pFunctionName);
	bool CompilePS(ID3D11Device* device, ID3D11PixelShader*& ps, WCHAR* psFilename, LPCSTR pFunctionName);
	
	bool InitializeShader(ID3D11Device* deivce,WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext* context, ID3D11ShaderResourceView* tex, UINT slot);
	bool CreateCBuffer(ID3D11Device * device, UINT byteWidth, ID3D11Buffer ** bufPPtr);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_psCompact;
	ID3D11PixelShader* m_psBlur;
	ID3D11PixelShader* m_psWarpMap;


	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;

	ID3D11Buffer* m_ConstBuffer;

};

