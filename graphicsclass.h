////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include "global.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"
#include "rendertextureclass.h"
#include "shadowshaderclass.h"


class GraphicsClass {
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	void SetKeysPressed(KeysPressed keysPressed);
	void SetFrameTime(FLOAT frametime);
	
	KeysPressed GetKeysSwitch();
private:

	bool CreateRenderTexture(RenderTextureClass** renderTexPtr, TextureType texType);

	bool conventionalShadowmap();

private:
	D3DClass* m_D3D;
	Camera* m_Camera;
	ModelClass *m_CubeModel;
	LightClass* m_Light;

	ShadowShaderClass*	m_ShadowShader;
	
	KeysPressed m_keysPressed;
	KeysPressed m_switch;

	FLOAT		m_frameTime;
	std::vector<ModelClass*> m_models;
	std::vector<ModelClass*> m_sence;
};
