#pragma once
#include<d3d11.h>
#include<D3DX10math.h>
#include <d3dx11async.h>
#include "Log.h"
#include "global.h"


class InputLayout {
public:
	static D3D11_INPUT_ELEMENT_DESC layout[];
	static UINT numElements;
	static D3D11_INPUT_ELEMENT_DESC XYZNUV[];
	static D3D11_INPUT_ELEMENT_DESC	XYZNUVTB[];
};

class BaseShader {
public:
	static bool CreateCBuffer(ID3D11Device* device, UINT byteWidth, ID3D11Buffer ** bufferPPtr);
	static bool CreateSampler(ID3D11Device* device, ID3D11SamplerState** sampleState, D3D11_TEXTURE_ADDRESS_MODE texMode);
	static bool CompileVSFromFile(WCHAR* vsFilename, CHAR* funcname, ID3D10Blob** vsBufferPtr, HWND hwnd = 0);
	static bool CompilePSFromFile(WCHAR* psFilename, CHAR* funcname, ID3D10Blob** psBufferPtr, HWND hwnd = 0);
	static bool CompileHSFromFile(WCHAR* hsFilename, CHAR* funcname, ID3D10Blob** hsBufferPtr, HWND hwnd = 0);
	static bool CompileDSFromFile(WCHAR* dsFilename, CHAR* funcname, ID3D10Blob** dsBufferPtr, HWND hwnd = 0);
	
	static bool CreateVerteShader(ID3D11Device* device, WCHAR* vsFilename, CHAR* funcname, ID3D10Blob** vsBufferPtr, ID3D11VertexShader** vertexShader, HWND hwnd = NULL);
};