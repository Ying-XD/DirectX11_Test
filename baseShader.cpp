#include "baseShader.h"
 
// ********** Layout *******************
D3D11_INPUT_ELEMENT_DESC InputLayout::layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,D3D11_INPUT_PER_VERTEX_DATA,0 }
};
UINT InputLayout::numElements = sizeof(InputLayout::layout)/sizeof(InputLayout::layout[0]);

D3D11_INPUT_ELEMENT_DESC InputLayout::XYZNUV[3] = {
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
UINT InputLayout::XYZNUV_EleNum	= sizeof(InputLayout::XYZNUV)/sizeof(InputLayout::XYZNUV[0]);
D3D11_INPUT_ELEMENT_DESC InputLayout::XYZNUVTB[5] = {
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 44,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
};





// **************** BaseShader	**********************
bool BaseShader::CreateCBuffer(ID3D11Device * device, UINT byteWidth, ID3D11Buffer ** bufferPPtr) {
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

bool BaseShader::CreateSampler(ID3D11Device * device, ID3D11SamplerState ** sampleState, D3D11_TEXTURE_ADDRESS_MODE texMode) {
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = texMode;
	samplerDesc.AddressV = texMode;
	samplerDesc.AddressW = texMode;
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
		device->CreateSamplerState(&samplerDesc, sampleState)
		);
	return true;
}

bool BaseShader::CompileVSFromFile(WCHAR * vsFilename, CHAR * funcname, ID3D10Blob ** vsBufferPtr, HWND hwnd) {
	ID3D10Blob* errorMsg = 0;
	HRESULT hr;
	hr = D3DX11CompileFromFile(vsFilename, NULL, NULL, funcname, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
							   vsBufferPtr, &errorMsg, NULL);
	if (FAILED(hr)) {
		if (errorMsg)
			Log::GetInstance()->LogShaderErr(errorMsg);
		if (hwnd != 0)
			MessageBox(hwnd, vsFilename, L"Shader File ERROR", MB_OK);
		return false;
	}
	return true;
}

bool BaseShader::CompilePSFromFile(WCHAR * psFilename, CHAR * funcname, ID3D10Blob ** psBufferPtr, HWND hwnd) {
	ID3D10Blob* errorMsg = 0;
	HRESULT hr;
	hr = D3DX11CompileFromFile(psFilename, NULL, NULL, funcname, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
							   psBufferPtr, &errorMsg, NULL
							   );
	if (FAILED(hr)) {
		if (errorMsg)
			Log::GetInstance()->LogShaderErr(errorMsg);
		if (hwnd != 0)
			MessageBox(hwnd, psFilename, L"Shader File ERROR", MB_OK);
		return false;
	}
	return true;
}

bool BaseShader::CompileHSFromFile(WCHAR * hsFilename, CHAR * funcname, ID3D10Blob ** hsBufferPtr, HWND hwnd) {
	ID3D10Blob* errorMsg = 0;
	HRESULT hr;
	S_OK;
	hr = D3DX11CompileFromFile(hsFilename, NULL, NULL, funcname, "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
							   hsBufferPtr, &errorMsg, NULL
							   );
	if (FAILED(hr)) {
		if (errorMsg) 
			Log::GetInstance()->LogShaderErr(errorMsg);
		if (hwnd)
			MessageBox(hwnd, hsFilename, L"Hull Shader Error", MB_OK);
		return false;
	}
	return true;
}

bool BaseShader::CompileDSFromFile(WCHAR * dsFilename, CHAR * funcname, ID3D10Blob ** dsBufferPtr, HWND hwnd) {
	ID3D10Blob* errorMsg = 0;
	HRESULT hr;
	hr = D3DX11CompileFromFile(dsFilename, NULL, NULL, funcname, "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
							   dsBufferPtr, &errorMsg, NULL
							   );
	if (FAILED(hr)) {
		if (errorMsg)
			Log::GetInstance()->LogShaderErr(errorMsg);
		if (hwnd)
			MessageBox(hwnd, dsFilename, L"Hull Shader Error", MB_OK);
		return false;
	}
	return true;
}

bool BaseShader::CreateVerteShader(ID3D11Device * device, WCHAR * vsFilename, CHAR * funcname, ID3D10Blob ** vsBufferPtr, ID3D11VertexShader ** vertexShader, HWND hwnd) {
	ID3D10Blob * &vsBuffer = *vsBufferPtr;
	CHECK_RESULT(CompileVSFromFile(vsFilename, funcname, vsBufferPtr, hwnd));
	CHECK_HRESULT(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, vertexShader));
	return true;
}

