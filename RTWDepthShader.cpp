#include "RTWDepthShader.h"
#include "baseShader.h"

RTWDepthShader::RTWDepthShader() {
	m_vertexShader	= NULL;
	m_hullShader	= NULL;
	m_domainShader	= NULL;
	m_pixelShader	= NULL;
	m_tessCBuffer	= NULL;
	m_layout		= NULL;

	m_vertexShader0	= NULL;
	m_pixelShader0	= NULL;
	m_layout0		= NULL;
	m_CBuffer		= NULL;

	m_SamplerBorder	= NULL;
}


RTWDepthShader::~RTWDepthShader() {
}

bool RTWDepthShader::Initialize(ID3D11Device * device) {
//#if TESSELLATION
//	CHECK_RESULT(
//		InitializeShader_Tess(device, L"./Shaders/RTWDepth_Tess.hlsl", L"./Shaders/RTWDepth_Tess.hlsl", L"RTWDepth_Tess.hlsl", L"RTWDepth_Tess.hlsl")
//		);
//#else
//	CHECK_RESULT(
//		InitializeShader(device, L"./RTW_Depth.hlsl", L"./RTW_Depth.hlsl")
//		);
//#endif
	CHECK_RESULT(
		InitializeShader_Tess(device, L"./Shaders/RTWDepth_Tess.hlsl", L"./Shaders/RTWDepth_Tess.hlsl", L"./Shaders/RTWDepth_Tess.hlsl", L"./Shaders/RTWDepth_Tess.hlsl")
	);
	CHECK_RESULT(
		InitializeShader(device, L"./Shaders/RTW_Depth.hlsl", L"./Shaders/RTW_Depth.hlsl")
	);
	return true;
}

void RTWDepthShader::Shutdown() {
	ShutdownShader();
}

bool RTWDepthShader::Render(ID3D11DeviceContext * context, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, ID3D11ShaderResourceView * warpMaps) {
	CHECK_RESULT(
		SetShaderParameters(context,worldMatrix,viewMatrix,projMatrix,warpMaps)
		);
	RenderShader(context,indexCount);
	return true;
}

bool RTWDepthShader::Render_Tessellation(ID3D11DeviceContext * context, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, ID3D11ShaderResourceView * warpMaps, float tessAmount) {
	CHECK_RESULT(SetShaderParameters_Tess(context, worldMatrix, viewMatrix, projMatrix, warpMaps, tessAmount));
	RenderShader_Tess(context, indexCount);
	return true;
}

void RTWDepthShader::ShutdownTess(ID3D11DeviceContext* context) {
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
}

bool RTWDepthShader::InitializeShader(ID3D11Device * device, WCHAR * vsFilename, WCHAR * psFilename) {
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;

	CHECK_RESULT(BaseShader::CompileVSFromFile(vsFilename, "RTW_DepthVertexShader", &vsBuffer, NULL));
	CHECK_RESULT(BaseShader::CompilePSFromFile(psFilename, "RTW_DepthPixelShader", &psBuffer, NULL));
	
	CHECK_HRESULT(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(),NULL,&m_vertexShader0));
	CHECK_HRESULT(device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(),NULL,&m_pixelShader0));
	
	CHECK_HRESULT(device->CreateInputLayout(InputLayout::layout, InputLayout::numElements, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_layout0));

	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);

	BaseShader::CreateCBuffer(device, sizeof(CBufferType), &m_CBuffer);

	return true;
}

bool RTWDepthShader::InitializeShader_Tess(ID3D11Device * device, WCHAR * vsFilename, WCHAR * psFilename, WCHAR * hsFilename, WCHAR * dsFilename) {
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;
	ID3D10Blob* hsBuffer = 0;
	ID3D10Blob* dsBuffer = 0;

	CHECK_RESULT(BaseShader::CompileVSFromFile(vsFilename, "TessellationVS", &vsBuffer, NULL));
	CHECK_RESULT(BaseShader::CompileHSFromFile(hsFilename, "TessellationHS", &hsBuffer, NULL));
	CHECK_RESULT(BaseShader::CompileDSFromFile(dsFilename, "TessellationDS", &dsBuffer, NULL));
	CHECK_RESULT(BaseShader::CompilePSFromFile(psFilename, "TessellationPS", &psBuffer, NULL));

	CHECK_HRESULT(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vertexShader));
	CHECK_HRESULT(device->CreateHullShader(hsBuffer->GetBufferPointer(), hsBuffer->GetBufferSize(), NULL, &m_hullShader));
	CHECK_HRESULT(device->CreateDomainShader(dsBuffer->GetBufferPointer(), dsBuffer->GetBufferSize(), NULL, &m_domainShader));
	CHECK_HRESULT(device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pixelShader));
	CHECK_HRESULT(device->CreateInputLayout(InputLayout::layout, InputLayout::numElements, 
											vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_layout));

	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(dsBuffer);
	SAFE_RELEASE(hsBuffer);
	SAFE_RELEASE(psBuffer);
	BaseShader::CreateCBuffer(device, sizeof(Tess_CBufferType), &m_tessCBuffer);
	BaseShader::CreateSampler(device, &m_SamplerBorder, D3D11_TEXTURE_ADDRESS_CLAMP);
	return true;
}

void RTWDepthShader::ShutdownShader() {
	SAFE_RELEASE(m_CBuffer);
	SAFE_RELEASE(m_tessCBuffer);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_hullShader);
	SAFE_RELEASE(m_domainShader);
	SAFE_RELEASE(m_SamplerBorder);
}

bool RTWDepthShader::SetShaderParameters(ID3D11DeviceContext * context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, ID3D11ShaderResourceView * warpMaps) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;


	// ½«¾ØÕó×ªÖÃ
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projMatrix, &projMatrix);

	CHECK_HRESULT(context->Map(m_CBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	CBufferType* CBufferPtr = (CBufferType*)mappedResource.pData;
	CBufferPtr->worldMatrix = worldMatrix;
	CBufferPtr->viewMatrix	= viewMatrix;
	CBufferPtr->projMatrix	= projMatrix;

	context->Unmap(m_CBuffer, 0);
	
	context->VSSetConstantBuffers(0, 1, &m_CBuffer);
	context->VSSetShaderResources(0, 1, &warpMaps);
	context->VSSetSamplers(0, 1, &m_SamplerBorder);

	return true;

}

bool RTWDepthShader::SetShaderParameters_Tess(ID3D11DeviceContext * context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, 
											  ID3D11ShaderResourceView * warpMaps, FLOAT tessAmount) {
	D3D11_MAPPED_SUBRESOURCE mapped;
	
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projMatrix, &projMatrix);
	
	CHECK_HRESULT(context->Map(m_tessCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
	Tess_CBufferType* ptr = (Tess_CBufferType*) mapped.pData;
	ptr->worldMatrix = worldMatrix;
	ptr->viewMatrix = viewMatrix;
	ptr->projMatrix = projMatrix;
	ptr->tessAmount = tessAmount;
	ptr->padding = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	context->Unmap(m_tessCBuffer, 0);

	context->HSSetConstantBuffers(0, 1, &m_tessCBuffer);
	context->DSSetConstantBuffers(0, 1, &m_tessCBuffer);
	context->DSSetShaderResources(0, 1, &warpMaps);
	context->DSSetSamplers(0, 1, &m_SamplerBorder);

	return true;
}

void RTWDepthShader::RenderShader(ID3D11DeviceContext * context, int indexCount) {
	context->IASetInputLayout(m_layout0);
	context->VSSetShader(m_vertexShader0, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->PSSetShader(m_pixelShader0, NULL, 0);
	context->DrawIndexed(indexCount, 0, 0);
}

void RTWDepthShader::RenderShader_Tess(ID3D11DeviceContext * context, int indexCount) {
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	context->IASetInputLayout(m_layout);
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->HSSetShader(m_hullShader, NULL, 0);
	context->DSSetShader(m_domainShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);
	context->DrawIndexed(indexCount, 0, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
}

