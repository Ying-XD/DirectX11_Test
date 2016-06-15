#include "DeferredShader.h"

DeferredShader::DeferredShader() {
	
	m_vertexShader = 0;
	m_pixelShader = 0;

	m_vsCBuffer = 0;
	m_psCBuffer = 0;

	m_samplerWrap = 0;
	m_layout = 0;

}


DeferredShader::~DeferredShader() {
}

bool DeferredShader::Initialize(ID3D11Device * device, HWND hwnd) {
	CHECK_RESULT(InitializeShader(device, hwnd, L"./Shaders/DeferredVS.hlsl", L"./Shaders/DeferredPS.hlsl"));
	return true;
}


void DeferredShader::Shutdown() {
	ShutdownShader();
}

bool DeferredShader::Render_Color(ID3D11DeviceContext * context, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
								  D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
								  ID3D11ShaderResourceView * texture) {
	SetShaderParameters_Color(context, worldMatrix, viewMatrix, projectionMatrix, 
							  lightPosition, ambientColor, diffuseColor,
							  texture);
	RenderShader_Color(context, indexCount);
	return true;
}

bool DeferredShader::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFilename, WCHAR * psFilename) {
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0, *psBuffer = 0;
	
	CHECK_RESULT(
		BaseShader::CompileVSFromFile(vsFilename, "DefferedVS", &vsBuffer, hwnd)	&&
		BaseShader::CompilePSFromFile(psFilename, "DefferedPS", &psBuffer, hwnd)
		);
	
	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vertexShader)
		);
	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pixelShader)
		);

	CHECK_HRESULT(
		device->CreateInputLayout(InputLayout::layout, InputLayout::numElements,
								   vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_layout)
	);
	
	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);
	
	BaseShader::CreateSampler(device, &m_samplerWrap, D3D11_TEXTURE_ADDRESS_WRAP);
	BaseShader::CreateCBuffer(device, sizeof(VS_CBufferType), &m_vsCBuffer);
	BaseShader::CreateCBuffer(device, sizeof(PS_CBufferType), &m_psCBuffer);

	return true;
}


void DeferredShader::ShutdownShader() {
	SAFE_RELEASE(m_psCBuffer);
	SAFE_RELEASE(m_vsCBuffer);
	SAFE_RELEASE(m_samplerWrap);
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_layout);
}


bool DeferredShader::SetShaderParameters_Color(ID3D11DeviceContext * context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
											   D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
											   ID3D11ShaderResourceView * texture) {
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	
	hr = context->Map(m_vsCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	CHECK_HRESULT(hr);

	VS_CBufferType * matrixBufPtr = (VS_CBufferType*) mapped.pData;
	matrixBufPtr->worldMat = worldMatrix;
	matrixBufPtr->viewMat = viewMatrix;
	matrixBufPtr->projMat = projectionMatrix;
	matrixBufPtr->lightPos = lightPosition;
	context->Unmap(m_vsCBuffer, 0);
	
	hr = context->Map(m_psCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	CHECK_HRESULT(hr);
	PS_CBufferType* psCBufPtr = (PS_CBufferType*) mapped.pData;
	psCBufPtr->ambientColor = ambientColor;
	psCBufPtr->diffuseColor = diffuseColor;
	context->Unmap(m_psCBuffer, 0);

	context->VSSetConstantBuffers(0, 1, &m_vsCBuffer);
	context->PSSetConstantBuffers(0, 1, &m_psCBuffer);

	context->PSSetShaderResources(0, 1, &texture);
	context->PSSetSamplers(0, 1, &m_samplerWrap);

	return true;
}

void DeferredShader::RenderShader_Color(ID3D11DeviceContext* context , int indexCount) {
	context->IASetInputLayout(m_layout);

	context->VSSetShader(m_vertexShader, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);

	context->DrawIndexed(indexCount, 0, 0);

}

