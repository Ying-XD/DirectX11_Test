#include "shadowshaderclass.h"
#include "baseShader.h"

ShadowShaderClass::ShadowShaderClass() {
	m_vertexShader	= NULL;
	m_pixelShader	= NULL;
	m_layout		= NULL;

	m_matrixBuffer	= NULL;
	m_lightBuffer	= NULL;
	m_lightBuffer2	= NULL;

	//	----	tessellation	----
	m_tessVS	= NULL;
	m_tessHS	= NULL;
	m_tessDS	= NULL;
	m_tessPS	= NULL;
	m_tessLayout = NULL;

	m_hsBuffer		= NULL;
	m_dsBuffer		= NULL;
	m_psBuffer		= NULL;

	//	----	Sampler		----
	m_sampleStateWrap	= NULL;
	m_sampleStateClamp	= NULL;
	m_sampleStateBorder	= NULL;

}


ShadowShaderClass::ShadowShaderClass(const ShadowShaderClass& other) {
}


ShadowShaderClass::~ShadowShaderClass() {
}

bool ShadowShaderClass::Initialize(ID3D11Device* device, HWND hwnd) {
	BaseShader::CreateSampler(device, &m_sampleStateWrap, D3D11_TEXTURE_ADDRESS_WRAP);
	BaseShader::CreateSampler(device, &m_sampleStateClamp, D3D11_TEXTURE_ADDRESS_CLAMP);
	BaseShader::CreateSampler(device, &m_sampleStateBorder, D3D11_TEXTURE_ADDRESS_BORDER);
	CHECK_RESULT(
		InitializeShader(device, hwnd, L"./Shaders/shadowVS.hlsl", L"./Shaders/shadowPS.hlsl")	&&
		InitializeShader_Tess(device, hwnd, L"./Shaders/Shadow_Tess.hlsl", L"./Shaders/Shadow_Tess.hlsl", L"./Shaders/Shadow_Tess.hlsl", L"./Shaders/Shadow_Tess.hlsl")
	);
	return true;
}


void ShadowShaderClass::Shutdown() {
	ShutdownShader();
}

void ShadowShaderClass::SetLightParameters(D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor) {
	m_lightPos = lightPosition;
	m_ambientColor = ambientColor;
	m_diffuseColor = diffuseColor;
}


bool ShadowShaderClass::Render(ID3D11DeviceContext* context, int indexCount, 
							   D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
							   D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, 
							   ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* warpMaps)
{
	// 传递参数
	CHECK_RESULT(
		SetShaderParameters(context, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, 
							texture, depthMapTexture, warpMaps, m_lightPos, m_ambientColor, m_diffuseColor)
	);
	// 渲染
	RenderShader(context, indexCount);
	

	return true;
}

bool ShadowShaderClass::Render_Tess(ID3D11DeviceContext * context, int indexCount, 
									D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
									D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, 
									ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, ID3D11ShaderResourceView * warpMaps, 
									FLOAT tessAmount) 
{
		SetShaderParameters_Tess(context, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,lightProjectionMatrix, 
								 texture, depthMapTexture, warpMaps, m_lightPos, m_ambientColor, m_diffuseColor,
								 tessAmount);
		RenderShader_Tess(context, indexCount);
		return true;
}


bool ShadowShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) {
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;

	

	// Compile the vertex shader code.
	bool result;
	CHECK_RESULT(
		BaseShader::CompileVSFromFile(vsFilename, "ShadowVertexShader", &vsBuffer, hwnd)	&&
		BaseShader::CompilePSFromFile(psFilename, "ShadowPixelShader", &psBuffer, hwnd)
	);
	

    // Create the vertex shader from the buffer.
	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vertexShader)
	);

    // Create the pixel shader from the buffer.
    CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pixelShader)
	);
		
	// Create the vertex input layout.
	CHECK_HRESULT(
		device->CreateInputLayout(InputLayout::layout, InputLayout::numElements,
								  vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_layout)
	);
	
	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);
	

	// Create a wrap texture sampler state description.

	
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.

	BaseShader::CreateCBuffer(device, sizeof(MatrixBufferType), &m_matrixBuffer);
	BaseShader::CreateCBuffer(device, sizeof(PS_LightBufferType), &m_lightBuffer);
	BaseShader::CreateCBuffer(device, sizeof(VS_LightBufferType), &m_lightBuffer2);

	return true;
}

bool ShadowShaderClass::InitializeShader_Tess(ID3D11Device * device, HWND hwnd, WCHAR * vsFilename, WCHAR * hsFilename, WCHAR * dsFilename, WCHAR * psFilename) {
	HRESULT hr;
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer = 0, *hsBuffer = 0, * dsBuffer = 0, * psBuffer = 0;
	CHECK_RESULT(
		BaseShader::CompileVSFromFile(vsFilename, "ShadowVS", &vsBuffer, hwnd)	&&
		BaseShader::CompileHSFromFile(hsFilename, "ShadowHS", &hsBuffer, hwnd)	&&
		BaseShader::CompileDSFromFile(dsFilename, "ShadowDS", &dsBuffer, hwnd)	&&
		BaseShader::CompilePSFromFile(psFilename, "ShadowPS", &psBuffer, hwnd)
	);
	
	CHECK_HRESULT(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_tessVS));
	CHECK_HRESULT(device->CreateHullShader(hsBuffer->GetBufferPointer(), hsBuffer->GetBufferSize(), NULL, &m_tessHS));
	CHECK_HRESULT(device->CreateDomainShader(dsBuffer->GetBufferPointer(), dsBuffer->GetBufferSize(), NULL, &m_tessDS));
	CHECK_HRESULT(device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_tessPS));

	CHECK_HRESULT(device->CreateInputLayout(InputLayout::layout, InputLayout::numElements, 
							vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_tessLayout));

	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(hsBuffer);
	SAFE_RELEASE(dsBuffer);
	SAFE_RELEASE(psBuffer);

	CHECK_RESULT(
		BaseShader::CreateCBuffer(device, sizeof(TessMatrixBufferType), &m_dsBuffer)		&&
		BaseShader::CreateCBuffer(device, sizeof(TessBufferType), &m_hsBuffer)				&&
		BaseShader::CreateCBuffer(device, sizeof(TessLightBufferType), &m_psBuffer)
	);
	return true;
}


void ShadowShaderClass::ShutdownShader(){

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

	//	----	tessellation	----
	SAFE_RELEASE(m_tessVS);
	SAFE_RELEASE(m_tessHS);
	SAFE_RELEASE(m_tessDS);
	SAFE_RELEASE(m_tessPS);
	SAFE_RELEASE(m_tessLayout);
	SAFE_RELEASE(m_hsBuffer);
	SAFE_RELEASE(m_dsBuffer);
	SAFE_RELEASE(m_psBuffer);

}



bool ShadowShaderClass::SetShaderParameters(ID3D11DeviceContext* context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix,
											ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, ID3D11ShaderResourceView* warpMaps,
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

		context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	}

	{
		context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		PS_LightBufferType* ptr = (PS_LightBufferType*)mappedResource.pData;
		ptr->ambientColor = ambientColor;
		ptr->rtwFlag = (warpMaps == NULL) ? 0 : G_Flag;
		ptr->diffuseColor = diffuseColor;
		ptr->padding = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
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

	context->VSSetShaderResources(0, 1, &warpMaps);
	context->VSSetSamplers(0, 1, &m_sampleStateBorder);

	// 在ps中设置 纹理数据
	context->PSSetShaderResources(0, 1, &texture);
	context->PSSetShaderResources(1, 1, &depthMapTexture);
	context->PSSetShaderResources(2, 1, &warpMaps);

	// 在PS中设置 采样
	context->PSSetSamplers(0, 1, &m_sampleStateWrap);
	context->PSSetSamplers(1, 1, &m_sampleStateClamp);
	context->PSSetSamplers(2, 1, &m_sampleStateBorder);

	return true;
}


bool ShadowShaderClass::SetShaderParameters_Tess(ID3D11DeviceContext * context, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, 
												 D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjMatrix, 
												 ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, 
												 ID3D11ShaderResourceView * warpMap, D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, FLOAT tessAmount) {
	D3D11_MAPPED_SUBRESOURCE mapped;
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projMatrix, &projMatrix);
	D3DXMatrixTranspose(&lightViewMatrix, &lightViewMatrix);
	D3DXMatrixTranspose(&lightProjMatrix, &lightProjMatrix);

	context->Map(m_dsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,&mapped);
	{
		TessMatrixBufferType*  ptr = (TessMatrixBufferType*)mapped.pData;
		ptr->worldMatr		= worldMatrix;
		ptr->viewMatr		= viewMatrix;
		ptr->projMatr		= projMatrix;
		ptr->lightViewMatr	= lightViewMatrix;
		ptr->lightProjMatr	= lightProjMatrix;
		ptr->lightPos		= lightPosition;
		ptr->smWidth		= SHADOWMAP_WIDTH;
	}
	context->Unmap(m_dsBuffer, 0);

	context->Map(m_psBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	{
		TessLightBufferType* ptr = (TessLightBufferType*) mapped.pData;
		ptr->ambientColor =	ambientColor;
		ptr->diffuseColor = diffuseColor;
	}
	context->Unmap(m_psBuffer, 0);

	context->Map(m_hsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	{
		TessBufferType* ptr = (TessBufferType*) mapped.pData;
		ptr->tessAmount = tessAmount;
		ptr->padding	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	context->Unmap(m_hsBuffer, 0);

	context->HSSetConstantBuffers(0, 1, &m_hsBuffer);
	
	context->DSSetShaderResources(0, 1, &warpMap);
	context->DSSetConstantBuffers(1, 1, &m_dsBuffer);
	context->DSSetSamplers(2, 1, &m_sampleStateBorder);

	context->PSSetShaderResources(0, 1, &warpMap);
	context->PSSetShaderResources(1, 1, &texture);
	context->PSSetShaderResources(2, 1, &depthMapTexture);
	context->PSSetConstantBuffers(2, 1, &m_psBuffer);


	context->PSSetSamplers(0, 1, &m_sampleStateWrap);
	context->PSSetSamplers(1, 1, &m_sampleStateClamp);
	context->PSSetSamplers(2, 1, &m_sampleStateBorder);
	return true;
}

void ShadowShaderClass::RenderShader(ID3D11DeviceContext* context, int indexCount)
{
	// Set the vertex input layout.
	context->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);
	// Render the triangle.
	context->DrawIndexed(indexCount, 0, 0);

}

void ShadowShaderClass::RenderShader_Tess(ID3D11DeviceContext * context, int indexCount) {
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	context->IASetInputLayout(m_tessLayout);
	context->VSSetShader(m_tessVS, NULL, 0);
	context->HSSetShader(m_tessHS, NULL, 0);
	context->DSSetShader(m_tessDS, NULL, 0);
	context->PSSetShader(m_tessPS, NULL, 0);
	context->DrawIndexed(indexCount, 0, 0);

	context->HSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
}
