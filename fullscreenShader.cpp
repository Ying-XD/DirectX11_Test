#include "fullscreenShader.h"



FullscreenShader::FullscreenShader() {
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_sampleStateClamp = 0;
	m_pixelShader2 = 0;
	m_psRGBA = 0;
	m_constBuffer = 0;
}


FullscreenShader::~FullscreenShader() {
}

bool FullscreenShader::Initialize(ID3D11Device * device) {
	return InitializeShader(device, L"./Shaders/FullScreenProcess.hlsl", L"./Shaders/FullScreenProcess.hlsl");
}

void FullscreenShader::Shutdown() {
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_pixelShader2);
	SAFE_RELEASE(m_psRGBA);
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_sampleStateClamp);
	SAFE_RELEASE(m_constBuffer);
}

bool FullscreenShader::Render(ID3D11DeviceContext * context, ID3D11ShaderResourceView * renderTex, TextureType type) {
	CHECK_RESULT(
		SetShaderParameters(context, renderTex)
		);
	RenderShader(context, type);
	return true;
}

bool FullscreenShader::InitializeShader(ID3D11Device * device, WCHAR * vsFilename, WCHAR * psFilename) {
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer, *psBuffer, *psBuffer2, *psBuf_RGBA;
	HRESULT hr;
	hr = D3DX11CompileFromFile(vsFilename, NULL, NULL, "VS_FullScreenProcess", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							   0, NULL, &vsBuffer, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
		}
		return false;
	}
	hr = D3DX11CompileFromFile(psFilename, NULL, NULL, "PS_FullScreenProcess", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							   0, NULL, &psBuffer, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
		}
		return false;
	}
	hr = D3DX11CompileFromFile(psFilename, NULL, NULL, "PS_WarpMaps", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							   0, NULL, &psBuffer2, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
		}
		return false;
	}


	hr = D3DX11CompileFromFile(psFilename, NULL, NULL, "PS_FullScreenRGBA", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							   0, NULL, &psBuf_RGBA, &errorMsg, NULL);

	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vertexShader)
		);

	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pixelShader)
		);
	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer2->GetBufferPointer(), psBuffer2->GetBufferSize(), NULL, &m_pixelShader2)
		);
	CHECK_HRESULT(
		device->CreatePixelShader(psBuf_RGBA->GetBufferPointer(), psBuf_RGBA->GetBufferSize(), NULL, &m_psRGBA)
		);

	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);
	SAFE_RELEASE(psBuffer2);
	SAFE_RELEASE(psBuf_RGBA);

	BaseShader::CreateCBuffer(device, sizeof(ConstBufferType), &m_constBuffer);
	BaseShader::CreateSampler(device, &m_sampleStateClamp, D3D11_TEXTURE_ADDRESS_CLAMP);
	return true;
}

bool FullscreenShader::SetShaderParameters(ID3D11DeviceContext * context, ID3D11ShaderResourceView * renderTex) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(m_constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ConstBufferType* ptr = (ConstBufferType*)mappedResource.pData;
	ptr->sm_width = static_cast<FLOAT>(SHADOWMAP_WIDTH);
	context->Unmap(m_constBuffer, 0);

	context->PSSetConstantBuffers(0, 1, &m_constBuffer);
	context->PSSetShaderResources(0, 1, &renderTex);
	context->PSSetSamplers(0, 1, &m_sampleStateClamp);

	return true;	
}

void FullscreenShader::RenderShader(ID3D11DeviceContext * context, TextureType type) {
	context->VSSetShader(m_vertexShader, NULL, 0);
	switch (type) {
		case ScreenTexture:
		case ShadowTexture:
			context->PSSetShader(m_pixelShader, NULL, 0);
			break;
		case WarpMapTexture:
			context->PSSetShader(m_pixelShader2, NULL, 0);
			break;
		case ScreenColorTexture:
			context->PSSetShader(m_psRGBA, NULL, 0);
			break;
		default:
			context->PSSetShader(m_pixelShader, NULL, 0);
			break;
	}
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3, 0);
}
