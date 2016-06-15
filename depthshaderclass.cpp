////////////////////////////////////////////////////////////////////////////////
// Filename: depthshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "depthshaderclass.h"


DepthShaderClass::DepthShaderClass()
{
	m_vertexShader	= NULL;
	m_pixelShader	= NULL;
	m_nvPShader		= NULL;

	m_layout		= NULL;
	m_layout2		= NULL;

	m_matrixBuffer	= NULL;
	m_pixelShader	= NULL;
	m_samplerClamp	= NULL;
	m_shadowDepVS	= NULL;
	m_shadowDepPS	= NULL;
}


DepthShaderClass::DepthShaderClass(const DepthShaderClass& other)
{
}


DepthShaderClass::~DepthShaderClass()
{
}


bool DepthShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	BaseShader::CreateSampler(device, &m_samplerClamp, D3D11_TEXTURE_ADDRESS_CLAMP);
	// ---- constant buffer ----
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	BaseShader::CreateCBuffer(device, sizeof(MatrixBufferType), &m_matrixBuffer);
	BaseShader::CreateCBuffer(device, sizeof(PosBufferType), &m_posBuffer);
	CHECK_RESULT(
		InitializeShader(device, hwnd, L"./Shaders/depthVS.hlsl", L"./Shaders/depthPS.hlsl")	&&
		InitialiseShadowDepShader(device, hwnd, L"./Shaders/depthVS.hlsl", L"./Shaders/depthPS.hlsl")
	);
	return true;
}


void DepthShaderClass::Shutdown()
{
	ShutdownShader();

}

bool DepthShaderClass::Render(ID3D11DeviceContext * context, int indexCount, 
				D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, 
							  D3DXVECTOR3 cameraPos, ShaderType texType) {
	m_flag = FALSE;
	SetShaderParameters(context, worldMatrix, viewMatrix, projMatrix,
						D3DXMATRIX(), D3DXMATRIX(), cameraPos, D3DXVECTOR3(0, 0, 0), 
						NULL
						);
	RenderShader(context, indexCount, texType);
	return true;
}


bool DepthShaderClass::Render_ShadowDepth(ID3D11DeviceContext* context, int indexCount,
							  D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,  D3DXMATRIX projMatrix, 
							  D3DXMATRIX lightViewMatrix, D3DXMATRIX ligthProjMatrtix, D3DXVECTOR3 lightPos,
							  ID3D11ShaderResourceView* texture,
							  ShaderType texType
							  )
{
	m_flag = TRUE;
	// Set the shader parameters that it will use for rendering.
	CHECK_RESULT(
		SetShaderParameters(context, worldMatrix, viewMatrix, projMatrix,
							lightViewMatrix, ligthProjMatrtix,
							D3DXVECTOR3(0, 0, 0), lightPos, texture)
	);

	RenderShader(context, indexCount, ShaderType::ShadowDepth);

	return true;
}


bool DepthShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;
	ID3D10Blob* psNVBuffer = 0;

    // Compile the vertex shader code.
	BaseShader::CompileVSFromFile(vsFilename, "DepthVertexShader", &vsBuffer, hwnd);
	BaseShader::CompilePSFromFile(psFilename, "DepthPixelShader", &psBuffer, hwnd);
	BaseShader::CompilePSFromFile(psFilename, "NVPixelShader", &psNVBuffer, hwnd);
	
	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vertexShader)
		);
	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pixelShader)
		);
	CHECK_HRESULT(
		device->CreatePixelShader(psNVBuffer->GetBufferPointer(), psNVBuffer->GetBufferSize(), NULL, &m_nvPShader)
		);

	CHECK_HRESULT(
		device->CreateInputLayout(InputLayout::layout, InputLayout::numElements, 
								  vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_layout)
	);
	

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);
	SAFE_RELEASE(psNVBuffer);

	return true;
}

bool DepthShaderClass::InitialiseShadowDepShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;
	
	BaseShader::CompileVSFromFile(vsFilename, "ShadowDepVS", &vsBuffer, hwnd);
	BaseShader::CompilePSFromFile(psFilename, "ShadowDepPS", &psBuffer, hwnd);

	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_shadowDepVS)
		);
	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_shadowDepPS)
		);


	CHECK_HRESULT(
		device->CreateInputLayout(InputLayout::layout, InputLayout::numElements, 
								  vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_layout2)
		);


	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);
	
	return true;
}


void DepthShaderClass::ShutdownShader() {
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_posBuffer);
	SAFE_RELEASE(m_layout);
	SAFE_RELEASE(m_layout2);
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_nvPShader);
	SAFE_RELEASE(m_vertexShader);
	SAFE_RELEASE(m_samplerClamp);
	SAFE_RELEASE(m_shadowDepVS);
	SAFE_RELEASE(m_shadowDepPS);
}


bool DepthShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
										   D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjMatrix, 
										   D3DXVECTOR3 cameraPos, D3DXVECTOR3 lightPos, 
										   ID3D11ShaderResourceView* texture)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	

	// 将矩阵转置
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	D3DXMatrixTranspose(&lightViewMatrix, &lightViewMatrix);
	D3DXMatrixTranspose(&lightProjMatrix, &lightProjMatrix);

	// 锁定 const buffer 以写入数据
	CHECK_HRESULT(
		deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
	);
	{
		// Get a pointer to the data in the constant buffer.
		MatrixBufferType* matrixBufferPtr = (MatrixBufferType*)mappedResource.pData;

		matrixBufferPtr->worldMat	= worldMatrix;
		matrixBufferPtr->viewMat	= viewMatrix;
		matrixBufferPtr->projMat	= projectionMatrix;

		matrixBufferPtr->lightViewMat = lightViewMatrix;
		matrixBufferPtr->lightProjMat = lightProjMatrix;

	}
	deviceContext->Unmap(m_matrixBuffer, 0);

	CHECK_HRESULT(deviceContext->Map(m_posBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	{
		PosBufferType* ptr = (PosBufferType*) mappedResource.pData;
		ptr->cameraPos	= cameraPos;
		ptr->lightPos	= lightPos;
		ptr->padding	= 0.0f;
	}
	deviceContext->Unmap(m_posBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &m_posBuffer);
	deviceContext->PSSetSamplers(0, 1, &m_samplerClamp);
	deviceContext->PSSetShaderResources(0, 1, &texture);
	return true;
}


void DepthShaderClass::RenderShader(ID3D11DeviceContext* context, int indexCount, ShaderType texType)
{
	// Set the vertex input layout.

	switch (texType) {
		case DepthShader:
			context->IASetInputLayout(m_layout);
			context->VSSetShader(m_vertexShader, NULL, 0);
			context->PSSetShader(m_pixelShader, NULL, 0);
			break;
		case NVShader:
			context->IASetInputLayout(m_layout);
			context->VSSetShader(m_vertexShader, NULL, 0);
			context->PSSetShader(m_nvPShader, NULL, 0);
			break;
		case ShadowDepth:
			context->IASetInputLayout(m_layout2);
			context->VSSetShader(m_shadowDepVS, NULL, 0);
			context->PSSetShader(m_shadowDepPS, NULL, 0);
			break;
		default:
			context->IASetInputLayout(m_layout);
			context->VSSetShader(m_vertexShader, NULL, 0);
			context->PSSetShader(m_pixelShader, NULL, 0);
			break;
	}
    
	// Render the triangle.
	context->DrawIndexed(indexCount, 0, 0);
	

}
