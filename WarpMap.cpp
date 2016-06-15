#include "WarpMap.h"



WarpMapShader::WarpMapShader() {
	m_vertexShader = 0;
	m_psBlur = 0;
	m_psCompact = 0;
	m_psWarpMap = 0;

	m_sampleStateClamp = 0;
	m_sampleStateWrap = 0;

	m_ConstBuffer = 0;
}


WarpMapShader::~WarpMapShader() {
	Shutdown();
}

bool WarpMapShader::Initialize(ID3D11Device * device) {
	CHECK_RESULT(
		InitializeShader(device,L"./Shaders/FullScreenProcess.hlsl",L"./Shaders/WarpMapPS.hlsl")
		);
	CreateCBuffer(device,sizeof(ConstBufferType),&m_ConstBuffer);
	return true;
}

void WarpMapShader::Shutdown() {
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_psBlur);
	SAFE_RELEASE(m_sampleStateClamp);
	SAFE_RELEASE(m_sampleStateWrap);
	SAFE_RELEASE(m_ConstBuffer);
}

bool WarpMapShader::Comapct(ID3D11DeviceContext * context, ID3D11ShaderResourceView * tex) {

	SetShaderParameters(context,tex,0);

	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_psCompact, NULL, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3, 0);
	return true;
}

bool WarpMapShader::Blur(ID3D11DeviceContext * context, ID3D11ShaderResourceView * tex) {
	SetShaderParameters(context, tex, 1);

	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_psBlur, NULL, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3, 0);
	return true;
}

bool WarpMapShader::BuildWarpMap(ID3D11DeviceContext * context, ID3D11ShaderResourceView * tex) {
	SetShaderParameters(context, tex, 2);

	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_psWarpMap, NULL, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3, 0);
	return true;
}

bool WarpMapShader::CompileVS(ID3D11Device * device, ID3D11VertexShader *& vs, WCHAR * vsFilename, LPCSTR pFunctionName) {
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0;

	hr = D3DX11CompileFromFile(vsFilename, NULL, NULL, pFunctionName, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
							   &vsBuffer, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
		}
		return false;
	}
	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &vs)
		);
	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(errorMsg);
	return true;
}

bool WarpMapShader::CompilePS(ID3D11Device * device, ID3D11PixelShader*& ps, WCHAR * psFilename, LPCSTR pFunctionName) {
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* psBuffer = 0;

	hr = D3DX11CompileFromFile(psFilename, NULL, NULL, pFunctionName, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
							   &psBuffer, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
		}
		return false;
	}
	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &ps)
		);

	SAFE_RELEASE(psBuffer);
	SAFE_RELEASE(errorMsg);
	return true;
}

bool WarpMapShader::InitializeShader(ID3D11Device * device, WCHAR * vsFilename, WCHAR * psFilename) {
	
	CompileVS(device, m_vertexShader, vsFilename, "VS_FullScreenProcess");
	
	CompilePS(device, m_psCompact, psFilename, "rtw_compact");
	CompilePS(device, m_psBlur, psFilename, "rtw_blur");
	CompilePS(device, m_psWarpMap, psFilename, "rtw_build");


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	CHECK_HRESULT(
		device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap)
		);

	// Create a clamp texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// Create the texture sampler state.
	CHECK_HRESULT(
		device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp)
		);




	return true;
}

bool WarpMapShader::SetShaderParameters(ID3D11DeviceContext * context, ID3D11ShaderResourceView * tex, UINT slot) {

	D3D11_MAPPED_SUBRESOURCE mappedRes;

	CHECK_HRESULT(
		context->Map(m_ConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes)
		);

	ConstBufferType* pBuf = (ConstBufferType*)mappedRes.pData;

	
	pBuf->d_texSize = 1.0f / SHADOWMAP_WIDTH;
	pBuf->sm_width = SHADOWMAP_WIDTH;
	pBuf->blur_factor = BLUR_FACTOR;
	pBuf->blur_width = BLUR_WIDTH;


	context->Unmap(m_ConstBuffer,0);
	context->PSSetConstantBuffers(0,1,&m_ConstBuffer);

	context->PSSetShaderResources(slot, 1, &tex);

	context->PSSetSamplers(0, 1, &m_sampleStateClamp);
	context->PSSetSamplers(1, 1, &m_sampleStateWrap);
	return true;
}

bool WarpMapShader::CreateCBuffer(ID3D11Device * device, UINT byteWidth, ID3D11Buffer ** bufPPtr) {
	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	CHECK_HRESULT(
		hr = device->CreateBuffer(&bufferDesc, NULL, bufPPtr)
		);
	return true;
}
