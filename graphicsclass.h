////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "depthshaderclass.h"
#include "shadowshaderclass.h"
#include "ImportanceMapClass.h"
#include "global.h"
#include "fullscreenShader.h"
#include "WarpMap.h"
#include "RTWDepthShader.h"
#include "DeferredShader.h"
#include "RTW_Shadow_Backward.h"
enum RTW_Type {
	Forward, Backward, Conventional
};
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	void SetLightPostion(float x,float y,float z);
	void SetKeysPressed(KeysPressed keysPressed);
	void SetFrameTime(FLOAT frametime);
	RTW_Type GetRTWType() {return m_rtwType;}
	KeysPressed GetKeysSwitch();
private:

	RTW_Type m_rtwType;
	
	bool CreateRenderTexture(RenderTextureClass** renderTexPtr, TextureType texType);

	bool RenderToImpMap_Forward();
	bool RenderToImpMap_Backward();

	bool BuildWarpingMaps();

	bool conventionalShadowmap();
	bool ShadowMapping();
	bool ShadowMapping_Backward();

	bool GetDepthTexture(RenderTextureClass* renderTex,D3DXMATRIX viewMatr,D3DXMATRIX projMatr,ShaderType texType);
	bool RenderNVTexture(RenderTextureClass* renderTex);
	bool RenderNormalTexture(RenderTextureClass* renderTex);

	bool RenderLightDepthTexture(RenderTextureClass* renderTex);
	bool RenderViewDepthTexture(RenderTextureClass* renderTex);
	bool RTW_RenderLightDepthTexture(RenderTextureClass* renderTex, ID3D11ShaderResourceView* warpMap);
	bool RenderShadowDepthTexture(RenderTextureClass* renderTex, ID3D11ShaderResourceView * lightDepTex);

	bool RenderColorTexture(RenderTextureClass * renderTex);

	bool DrawTempTexture();
private:
	D3DClass* m_D3D;
	Camera* m_Camera;
	ModelClass *m_CubeModel, *m_GroundModel, *m_SphereModel;
	ModelClass *m_GroundModel2;
	LightClass* m_Light;

	//	----	render texture	----
	RenderTextureClass* m_ImpMapTex;
	RenderTextureClass* m_DepViewTex;
	RenderTextureClass* m_DepLightTex;
	RenderTextureClass* m_DepShadowTex;

	RenderTextureClass* m_NVTex, *m_NormalTex;

	RenderTextureClass* m_compactTex, * m_blurTex, * m_warpMapTex;
	RenderTextureClass* m_rtwDepthTex;
	RenderTextureClass* m_colorTex;
	RenderTextureClass* m_DeferredTex;

	//	----	Shader	----
	DepthShaderClass*	m_DepthShader;
	ShadowShaderClass*	m_ShadowShader;
	ImportanceMapClass* m_ImpMapShader;
	FullscreenShader*	m_FullscreenShader;
	WarpMapShader*		m_warpMapShader;
	RTWDepthShader*		m_rtwDepthShader;
	DeferredShader*		m_deferredShader;

	RTW_Shadow_Backward*	m_rtwShadow_Backward;

	
	KeysPressed m_keysPressed;
	KeysPressed m_switch;

	FLOAT		m_frameTime;
	std::vector<ModelClass*> m_models;
	std::vector<ModelClass*> m_sence;
};
void GetLv2Cv(D3DXMATRIX* lv2cvPtr, D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr,
			  D3DXMATRIX vLightMatr, D3DXMATRIX pLightMatr);
void GetCv2Lv(D3DXMATRIX* cv2lvPtr, D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr,
			  D3DXMATRIX vLightMatr, D3DXMATRIX pLightMatr);
