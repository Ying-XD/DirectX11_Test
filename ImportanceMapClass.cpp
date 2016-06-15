#include "ImportanceMapClass.h"

ImportanceMapClass::ImportanceMapClass() {
	m_forwardVS = 0;
	m_forwardPS = 0;
	m_backwardVS = 0;
	m_backwardPS = 0;

	m_CBuffer_Forward = 0;
	m_CBuffer_Backward = 0;

	m_sampleStateWrap = 0;
	//D3DXMatrixIdentity(&m_lv2cv);
	//D3DXMatrixIdentity(&m_cv2lv);

}


ImportanceMapClass::~ImportanceMapClass() {
}

bool ImportanceMapClass::Initialize(ID3D11Device * device, HWND hwnd) {
	return InitializeShader(device,hwnd,L"./Shaders/FullScreenProcess.hlsl",L"./Shaders/ImpForwardPS.hlsl",
							L"./Shaders/ImpBackwardVS.hlsl",L"./Shaders/ImpBackwardPS.hlsl"
							);
}

void ImportanceMapClass::Shutdown() {
	ShutdownShader();
}

//void ImportanceMapClass::SetLv2CvMatrix(D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr, D3DXMATRIX vLightMatr, D3DXMATRIX pLightMatr) {
//
//	D3DXMatrixInverse(&vLightMatr, NULL, &vLightMatr);
//	D3DXMatrixInverse(&pLightMatr, NULL, &pLightMatr);
//
//	D3DXMatrixMultiply(&m_lv2cv, &pLightMatr, &vLightMatr);
//	D3DXMatrixMultiply(&m_lv2cv, &m_lv2cv, &vCameraMatr);
//	D3DXMatrixMultiply(&m_lv2cv, &m_lv2cv, &pCameraMatr);
//}
//
//void ImportanceMapClass::SetCv2LvMatrix(D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr, D3DXMATRIX vLightMatr, D3DXMATRIX pLightMatr) {
//	D3DXMatrixInverse(&vCameraMatr, NULL, &vCameraMatr);
//	D3DXMatrixInverse(&pCameraMatr, NULL, &pCameraMatr);
//
//	D3DXMatrixMultiply(&m_cv2lv, &pCameraMatr, &vCameraMatr);
//	D3DXMatrixMultiply(&m_cv2lv, &m_cv2lv, &vLightMatr);
//	D3DXMatrixMultiply(&m_cv2lv, &m_cv2lv, &pLightMatr);
//}

bool ImportanceMapClass::Render_Forward(ID3D11DeviceContext * context, ID3D11ShaderResourceView * depthLightTex, ID3D11ShaderResourceView * nvTex, D3DXMATRIX lv2cv) {
	CHECK_RESULT(
		SetShaderParameters_Forward(context, depthLightTex, nvTex, lv2cv)
	);
	RenderShader_Forward(context);
	return true;
}

bool ImportanceMapClass::Render_Backward(ID3D11DeviceContext * context, ID3D11ShaderResourceView * depthViewTex, ID3D11ShaderResourceView * nvTex, D3DXMATRIX cv2lv) {
	CHECK_RESULT(
		SetShaderParameters_Backward(context, depthViewTex, nvTex, cv2lv)
		);
	RenderShader_Backward(context);

	return true;
}

//bool ImportanceMapClass::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* depthLightTex, ID3D11ShaderResourceView* nvTex){
//	CHECK_RESULT(
//		SetShaderParameters_Forward(context, depthLightTex, nvTex, m_lv2cv)
//		);
//	RenderShader_Forward(context);
//	return true;
//}

bool ImportanceMapClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFilenameF, WCHAR * psFilenameF, WCHAR* vsFilenameB, WCHAR* psFilenameB) {
	ID3D10Blob* errorMsg = 0;
	ID3D10Blob* vsBuffer,* psBuffer;
	HRESULT hr;

	// ========== Forwardward	==================
	hr = D3DX11CompileFromFile(vsFilenameF,NULL,NULL,"VS_FullScreenProcess","vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
						  0,NULL,&vsBuffer,&errorMsg,NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
			MessageBox(hwnd, L"Error compiling shader.  Please check Log.txt.", vsFilenameF, MB_OK);
		}
		return false;
	}

	hr = D3DX11CompileFromFile(psFilenameF, NULL, NULL, "ImportancePS_Forward", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							   0, NULL, &psBuffer, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
			MessageBox(hwnd, L"Error compiling shader. Please check Log.txt.", psFilenameF, MB_OK);	
		}
		return false;
	}

	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(),vsBuffer->GetBufferSize(),NULL,&m_forwardVS)
	);

	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_forwardPS)
	);

	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);


	// ========== Backward	==================
	hr = D3DX11CompileFromFile(vsFilenameB, NULL, NULL, "ImportanceVS_Backward", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							   0, NULL, &vsBuffer, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
			MessageBox(hwnd, L"Error compiling shader.  Please check Log.txt.", vsFilenameF, MB_OK);
		}
		return false;
	}

	hr = D3DX11CompileFromFile(psFilenameB, NULL, NULL, "ImportancePS_Backward", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							   0, NULL, &psBuffer, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg) {
			Log::GetInstance()->LogShaderErr(errorMsg);
			MessageBox(hwnd, L"Error compiling shader. Please check Log.txt.", psFilenameF, MB_OK);
		}
		return false;
	}

	CHECK_HRESULT(
		device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_backwardVS)
		);

	CHECK_HRESULT(
		device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_backwardPS)
		);

	SAFE_RELEASE(vsBuffer);
	SAFE_RELEASE(psBuffer);

	CreateCBuffer(device, sizeof(CBufferType_Forward), &m_CBuffer_Forward);
	CreateCBuffer(device, sizeof(CBufferType_Backward), &m_CBuffer_Backward);


	// sampler

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


	CHECK_HRESULT(
		device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap)
		);



	return true;

}

void ImportanceMapClass::ShutdownShader() {
	SAFE_RELEASE(m_forwardPS);
	SAFE_RELEASE(m_forwardVS);
	SAFE_RELEASE(m_backwardVS);
	SAFE_RELEASE(m_backwardPS);

	SAFE_RELEASE(m_CBuffer_Forward);
	SAFE_RELEASE(m_CBuffer_Backward);

	SAFE_RELEASE(m_sampleStateWrap);
}

bool ImportanceMapClass::SetShaderParameters_Forward(ID3D11DeviceContext * context, ID3D11ShaderResourceView* depthLightTex, ID3D11ShaderResourceView* nvTex, D3DXMATRIX lv2cv) {

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//D3DXMatrixIdentity(&m_lv2cv);
	D3DXMatrixTranspose(&lv2cv, &lv2cv);

	CHECK_HRESULT(
		context->Map(m_CBuffer_Forward, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
		);
	CBufferType_Forward* ptr = (CBufferType_Forward*)mappedResource.pData;
	ptr->lv2cv = lv2cv;
	ptr->sm_wdith = SHADOWMAP_WIDTH;
	ptr->sn_bonus = SN_BONUS;
	ptr->shadow_bias = SHADOW_BIAS;
	ptr->padding = 0.0f;

	context->Unmap(m_CBuffer_Forward, 0);
	context->PSSetConstantBuffers(0, 1, &m_CBuffer_Forward);

	context->PSSetShaderResources(0, 1, &depthLightTex);
	context->PSSetShaderResources(1, 1, &nvTex);

	return true;
}

void ImportanceMapClass::RenderShader_Forward(ID3D11DeviceContext * context) {
	context->VSSetShader(m_forwardVS,NULL,0);
	context->PSSetShader(m_forwardPS,NULL,0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(3,0);
}

bool ImportanceMapClass::SetShaderParameters_Backward(ID3D11DeviceContext * context, ID3D11ShaderResourceView * depthViewTex, ID3D11ShaderResourceView * nvTex, D3DXMATRIX cv2lv) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3DXMatrixTranspose(&cv2lv, &cv2lv);
	CHECK_HRESULT(
		context->Map(m_CBuffer_Backward, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
		);
	CBufferType_Backward* ptr = (CBufferType_Backward*)mappedResource.pData;
	ptr->cv2lv = cv2lv;
	ptr->sm_wdith = SHADOWMAP_WIDTH;
	ptr->sn_bonus = SN_BONUS;
	ptr->padding = D3DXVECTOR2(0.0f, 0.0f);
	context->Unmap(m_CBuffer_Backward, 0);
	context->VSSetConstantBuffers(0, 1, &m_CBuffer_Backward);

	context->VSSetSamplers(0, 1, &m_sampleStateWrap);
	context->VSSetShaderResources(0, 1, &depthViewTex);
	context->VSSetShaderResources(1, 1, &nvTex);


	//context->PSSetShaderResources(0, 1, &depthViewTex);
	//context->PSSetShaderResources(1, 1, &nvTex);
	return true;
}

void ImportanceMapClass::RenderShader_Backward(ID3D11DeviceContext * context) {
	context->VSSetShader(m_backwardVS, NULL, 0);
	context->PSSetShader(m_backwardPS, NULL, 0);
	
	//context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//context->Draw(3, 0);

	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw((SHADOWMAP_WIDTH)* (SHADOWMAP_HEIGHT), 0);
}

bool ImportanceMapClass::CreateCBuffer(ID3D11Device * device, UINT byteWidth, ID3D11Buffer ** bufferPPtr) {
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	CHECK_HRESULT(
		device->CreateBuffer(&bufferDesc, NULL, bufferPPtr)
		);
	return true;
}
