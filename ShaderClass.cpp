#include "ShaderClass.h"
#include "baseShader.h"

ShaderClass::ShaderClass() {
	m_vertexShader	= NULL;
	m_pixelShader	= NULL;
	m_layout		= NULL;

	m_matrixBuffer	= NULL;
	m_lightBuffer	= NULL;
	m_lightBuffer2	= NULL;

	m_sampleStateWrap	= NULL;
	m_sampleStateClamp	= NULL;
	m_sampleStateBorder	= NULL;

}


ShaderClass::ShaderClass(const ShaderClass& other) {
}


ShaderClass::~ShaderClass() {
}

bool ShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {
	BaseShader::CreateSampler(device, &m_sampleStateWrap, D3D11_TEXTURE_ADDRESS_WRAP);
	BaseShader::CreateSampler(device, &m_sampleStateClamp, D3D11_TEXTURE_ADDRESS_CLAMP);
	BaseShader::CreateSampler(device, &m_sampleStateBorder, D3D11_TEXTURE_ADDRESS_BORDER);
	CHECK_RESULT(
		InitializeShader(device, hwnd, L"./Shaders/shadowVS.hlsl", L"./Shaders/shadowPS.hlsl")
	);
	return true;
}


void ShaderClass::Shutdown() {
	ShutdownShader();
}

void ShaderClass::SetLightParameters(D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor) {
	m_lightPos = lightPosition;
	m_ambientColor = ambientColor;
	m_diffuseColor = diffuseColor;
}


bool ShaderClass::Render(ID3D11DeviceContext* context, int indexCount, 
							   D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
							   D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, 
							   ID3D11ShaderResourceView* texture)
{
	// 传递参数
	CHECK_RESULT(
		SetShaderParameters(context, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, 
							texture, m_lightPos, m_ambientColor, m_diffuseColor)
	);
	// 渲染
	RenderShader(context, indexCount);
	

	return true;
}

bool ShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;

	CHECK_RESULT(
		BaseShader::CompileVSFromFile(vsFilename, "ShadowVertexShader", &vsBuffer, hwnd)	&&
		BaseShader::CompilePSFromFile(psFilename, "ShadowPixelShader", &psBuffer, hwnd)
	);
	

	CHECK_HRESULT(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vertexShader));
    CHECK_HRESULT(device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pixelShader));
	CHECK_HRESULT(device->CreateInputLayout(InputLayout::layout, InputLayout::numElements,
								  vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_layout));
	
	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);
	
	CHECK_RESULT(
		BaseShader::CreateCBuffer(device, sizeof(MatrixBufferType), &m_matrixBuffer)	&&
		BaseShader::CreateCBuffer(device, sizeof(PS_LightBufferType), &m_lightBuffer)	&&
		BaseShader::CreateCBuffer(device, sizeof(VS_LightBufferType), &m_lightBuffer2)
	);

	return true;
}

void ShaderClass::ShutdownShader(){

	SAFE_RELEASE(m_lightBuffer);
	SAFE_RELEASE(m_lightBuffer2);
	SAFE_RELEASE(m_matrixBuffer);
	SAFE_RELEASE(m_layout);
	
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);

	//	----	Sampler		----
	SAFE_RELEASE(m_sampleStateWrap);
	SAFE_RELEASE(m_sampleStateClamp);
	SAFE_RELEASE(m_sampleStateBorder);

}



bool ShaderClass::SetShaderParameters(ID3D11DeviceContext* context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
											ID3D11ShaderResourceView* texture, 
											D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	D3DXMatrixTranspose(&lightViewMatrix, &lightViewMatrix);
	D3DXMatrixTranspose(&lightProjectionMatrix, &lightProjectionMatrix);

	{
		CHECK_HRESULT(context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

		MatrixBufferType* matrixBufferPtr = (MatrixBufferType*)mappedResource.pData;
		matrixBufferPtr->world = worldMatrix;
		matrixBufferPtr->view = viewMatrix;
		matrixBufferPtr->projection = projectionMatrix;
		matrixBufferPtr->lightView = lightViewMatrix;
		matrixBufferPtr->lightProjection = lightProjectionMatrix;
		context->Unmap(m_matrixBuffer, 0);
	}
	context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	{
		context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		PS_LightBufferType* ptr = (PS_LightBufferType*)mappedResource.pData;
		ptr->ambientColor = ambientColor;
		ptr->diffuseColor = diffuseColor;
		context->Unmap(m_lightBuffer, 0);
	}
	context->PSSetConstantBuffers(0, 1, &m_lightBuffer);
	
	{
		CHECK_HRESULT(context->Map(m_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		VS_LightBufferType* ptr= (VS_LightBufferType*)mappedResource.pData;
		ptr->lightPosition = lightPosition;
		ptr->smWidth = SHADOWMAP_WIDTH;
		context->Unmap(m_lightBuffer2, 0);
	}
	context->VSSetConstantBuffers(1, 1, &m_lightBuffer2);

	// 在ps中设置 纹理数据
	context->PSSetShaderResources(0, 1, &texture);

	// 在PS中设置 采样
	context->PSSetSamplers(0, 1, &m_sampleStateWrap);
	context->PSSetSamplers(1, 1, &m_sampleStateClamp);
	context->PSSetSamplers(2, 1, &m_sampleStateBorder);

	return true;
}


void ShaderClass::RenderShader(ID3D11DeviceContext* context, int indexCount)
{
	context->IASetInputLayout(m_layout);

	context->VSSetShader(m_vertexShader, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);

	context->DrawIndexed(indexCount, 0, 0);
}

