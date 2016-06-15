#include "RTW_Shadow_Backward.h"



RTW_Shadow_Backward::RTW_Shadow_Backward() {
	m_sampleStateWrap = NULL;
	m_sampleStateClamp = NULL;

	m_vertexShader = NULL;
	m_pixelShader = NULL;
	m_deferPSBuf = NULL;
}


RTW_Shadow_Backward::~RTW_Shadow_Backward() {
}

bool RTW_Shadow_Backward::Initialize(ID3D11Device * device, HWND hwnd) {
	CHECK_RESULT(InitializeShader(device, hwnd, L"./Shaders/Shadow_DeferredBackward.hlsl", L"./Shaders/Shadow_DeferredBackward.hlsl"));
	return true;
}

void RTW_Shadow_Backward::Shutdown() {
	ShutdownShader();
}

bool RTW_Shadow_Backward::Render(ID3D11DeviceContext * context, D3DXMATRIX cv2lvMatr, D3DXVECTOR4 ambientColor,
				ID3D11ShaderResourceView * warpMaps, ID3D11ShaderResourceView * colorTex, ID3D11ShaderResourceView* diffuseTex,
				ID3D11ShaderResourceView * lightDepTex, ID3D11ShaderResourceView * viewDepTex) {
	CHECK_RESULT(SetShaderParameters(context, cv2lvMatr, ambientColor,warpMaps, colorTex, diffuseTex, lightDepTex, viewDepTex));
	RenderShader(context);
	return true;
}

bool RTW_Shadow_Backward::InitializeShader(ID3D11Device * device, HWND hwnd, 
										   WCHAR * vsFilename, WCHAR * psFilename) {
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;
	CHECK_RESULT(
		BaseShader::CompileVSFromFile(vsFilename, "Backward_VS", &vsBuffer, hwnd) &&
		BaseShader::CompilePSFromFile(psFilename, "Backward_PS", &psBuffer, hwnd)
		);
	CHECK_HRESULT(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vertexShader));
	CHECK_HRESULT(device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pixelShader));

	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);
	CHECK_RESULT(BaseShader::CreateCBuffer(device, sizeof(DeferredPSBuffer), &m_deferPSBuf));
	BaseShader::CreateSampler(device, &m_sampleStateWrap, D3D11_TEXTURE_ADDRESS_WRAP);
	BaseShader::CreateSampler(device, &m_sampleStateClamp, D3D11_TEXTURE_ADDRESS_CLAMP);
	return true;
}

void RTW_Shadow_Backward::ShutdownShader() {
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_pixelShader);

	SAFE_RELEASE(m_deferPSBuf);

	SAFE_RELEASE(m_sampleStateWrap);
	SAFE_RELEASE(m_sampleStateClamp);

}

bool RTW_Shadow_Backward::SetShaderParameters(ID3D11DeviceContext * context, D3DXMATRIX cv2lvMatr, D3DXVECTOR4 ambientColor,
				ID3D11ShaderResourceView * warpMaps, ID3D11ShaderResourceView * colorTex, ID3D11ShaderResourceView* diffuseTex,
				ID3D11ShaderResourceView * lightDepTex, ID3D11ShaderResourceView * viewDepTex) {
	D3D11_MAPPED_SUBRESOURCE mapped;

	D3DXMatrixTranspose(&cv2lvMatr, &cv2lvMatr);
	context->Map(m_deferPSBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	{
		DeferredPSBuffer* ptr = (DeferredPSBuffer*) mapped.pData;
		ptr->cv2lvMatr = cv2lvMatr;
		ptr->ambientColor = ambientColor;
		ptr->shadow_texel_size = D3DXVECTOR2(Shadow_Texel_Size, Shadow_Texel_Size);
	}
	context->Unmap(m_deferPSBuf, 0);

	context->PSSetShaderResources(0, 1, &warpMaps);
	context->PSSetShaderResources(1, 1, &colorTex);
	context->PSSetShaderResources(2, 1, &lightDepTex);
	context->PSSetShaderResources(3, 1, &viewDepTex);
	context->PSSetShaderResources(4, 1, &diffuseTex);

	context->PSSetConstantBuffers(0, 1, &m_deferPSBuf);

	context->PSSetSamplers(0, 1, &m_sampleStateWrap);
	context->PSSetSamplers(1, 1, &m_sampleStateClamp);
	return true;
}

void RTW_Shadow_Backward::RenderShader(ID3D11DeviceContext * context) {
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3, 0);
}
