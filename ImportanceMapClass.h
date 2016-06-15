#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include <d3dx11async.h>
#include "global.h"
#include "Log.h"
using namespace std;

class ImportanceMapClass {
	struct CBufferType_Forward {
		D3DXMATRIX	lv2cv;
		UINT32		sm_wdith;
		FLOAT		sn_bonus;
		FLOAT		shadow_bias;
		FLOAT		padding;
	};
	struct CBufferType_Backward {
		D3DXMATRIX	cv2lv;
		UINT32		sm_wdith;
		FLOAT		sn_bonus;
		D3DXVECTOR2 padding;
	};
public:
	ImportanceMapClass();
	~ImportanceMapClass();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	//void SetLv2CvMatrix(D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr,
	//			   D3DXMATRIX vLightMatr,  D3DXMATRIX pLightMatr);
	//void SetCv2LvMatrix(D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr,
	//					D3DXMATRIX vLightMatr, D3DXMATRIX pLightMatr);

	bool Render_Forward(ID3D11DeviceContext* context,
						ID3D11ShaderResourceView* depthLightTex, ID3D11ShaderResourceView* nvTex, D3DXMATRIX lv2cv);

	bool Render_Backward(ID3D11DeviceContext* context,
						 ID3D11ShaderResourceView* depthViewTex, ID3D11ShaderResourceView* nvTex, D3DXMATRIX cv2lv);

	//bool Render(ID3D11DeviceContext* deviceContext, 
	//			ID3D11ShaderResourceView* depthLightTex, ID3D11ShaderResourceView* nvTex);
private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilenameF, WCHAR* psFilenameF,
						  WCHAR* vsFilenameB, WCHAR* psFilenameB);
	void ShutdownShader();

	bool SetShaderParameters_Forward(ID3D11DeviceContext* context, ID3D11ShaderResourceView* depthLightTex, ID3D11ShaderResourceView* nvTex, D3DXMATRIX lv2cv);
	void RenderShader_Forward(ID3D11DeviceContext* context);

	bool SetShaderParameters_Backward(ID3D11DeviceContext* context, ID3D11ShaderResourceView* depthViewTex, ID3D11ShaderResourceView* nvTex, D3DXMATRIX cv2lv);
	void RenderShader_Backward(ID3D11DeviceContext* context);

	bool CreateCBuffer(ID3D11Device* device, UINT byteWidth, ID3D11Buffer ** bufferPPtr);
private:
	ID3D11VertexShader* m_forwardVS;
	ID3D11PixelShader* m_forwardPS;

	ID3D11VertexShader* m_backwardVS;
	ID3D11PixelShader* m_backwardPS;

	//D3DXMATRIX	m_lv2cv, m_cv2lv;
	ID3D11Buffer* m_CBuffer_Forward, *m_CBuffer_Backward;
	ID3D11SamplerState*	m_sampleStateWrap;
};


