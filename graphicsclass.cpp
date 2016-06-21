////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D			= 0;
	m_Camera		= 0;
	m_CubeModel		= 0;
	m_Light			= 0;
	m_Shader = 0;
	m_keysPressed.Reset();

	m_bwParser		= 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	
	// Create the Direct3D object.
	m_D3D = new D3DClass;
	CHECK_RESULT_MSG(m_D3D && m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR) , "Could not initialize Direct3D.");

	ID3D11Device* device = m_D3D->GetDevice();

	// Create the camera object.
	m_Camera = new Camera;
	if(!m_Camera) {
		return false;
	}

	m_Camera->SetPosition(0.0f, 3.0f, 3.0f);
	m_Camera->SetLookAt(0.0f, -1.5f, -2.0f);
	
	// Create the cube model object.
	m_CubeModel = new ModelClass;
	CHECK_RESULT_MSG(m_CubeModel && m_CubeModel->Initialize(device, "./data/cube.txt", L"./data/wall01.dds"), "Could not initialize the cube model object.");

	m_CubeModel->SetPosition(0.0f, 1.0f, 0.0f);
	

	m_sence.push_back(m_CubeModel);

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light) {
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(LIGHTVIEW_FAR, LIGHTVIEW_NEAR);

	// Create the shadow shader object.
	m_Shader = new ShaderClass;
	CHECK_RESULT_MSG(m_Shader && m_Shader->Initialize(device, hwnd), "Could not initialize the shadow shader object.");

	m_bwParser = new BW_ModelParser;
	CHECK_RESULT_MSG(m_bwParser && m_bwParser->Initialize(device),"Could not initialize BW_ModelParser.");
	CHECK_RESULT_MSG(m_bwParser->LoadModelFiles("./res/10006_50097_00_body.primitives","./res/10006_50097_00_body.visual"), "Could not load files in bw_parser");

	m_modelsList = m_bwParser->GetModels();
	m_boneTree	 = m_bwParser->GetBoneTree();
	auto root = m_boneTree->GetRoot();
	return true;
}


void GraphicsClass::Shutdown()
{

	SHUTDOWN_SHADER(m_Shader);

	for each (auto model in m_sence) {
		SHUTDOWN_MODEL(model);
	}
	for each (auto  model in m_modelsList) {
		SHUTDOWN_MODEL(model);
	}
	SAFE_DELETE(m_Camera);
	SAFE_DELETE(m_Light);
	SHUTDOWN_DELETE(m_D3D);
}


bool GraphicsClass::Frame() {




	static D3DXVECTOR3	lightPos(0.0f, 8.0f, 5.0f);

	m_Camera->Render(&m_keysPressed);
	m_Light->SetPosition(lightPos);
	m_Light->GenerateViewMatrix();

	return Render();
	
	return true;
}


void GraphicsClass::SetKeysPressed(KeysPressed keysPressed) {
	for (size_t i = 0;i < KEYS_NUM; ++i) {
		m_switch._key[i] = m_keysPressed._key[i] & !keysPressed._key[i] ^ m_switch._key[i];
	}

	m_keysPressed = keysPressed;
}

void GraphicsClass::SetFrameTime(FLOAT frametime) {
	m_frameTime = frametime;
	if (m_keysPressed.l_shifit) m_frameTime = frametime * 0.04;
	m_Camera->SetFrameTime(m_frameTime);
}

KeysPressed GraphicsClass::GetKeysSwitch() {
	return m_switch;
}

//bool GraphicsClass::CreateRenderTexture(RenderTextureClass ** renderTexPtr, TextureType texType) {
//	bool result = true;
//	RenderTextureClass* renderTex = new RenderTextureClass;
//	if (!renderTex) return false;
//	
//	renderTex->SetTextureType(texType);
//	switch (texType) {
//		case ScreenTexture:
//			result = renderTex->Initialize(m_D3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT);
//			break;
//		case ShadowTexture:
//			result = renderTex->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_WIDTH);
//			break;
//		case WarpMapTexture:
//			result = renderTex->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, 2);
//			break;
//		case ScreenColorTexture:
//			result = renderTex->Initialize(m_D3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);
//			break;
//		default:
//			result = renderTex->Initialize(m_D3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT);
//			break;
//	}
//	if (!result) {
//		Log::GetInstance()->LogMsg("Could not initialize the render to texture object.");
//		delete renderTex;
//	}
//	else {
//		*renderTexPtr = renderTex;
//	}
//	return result;
//}


bool GraphicsClass::Render() {
	
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;
	float posX, posY, posZ;


	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->GetViewMatrix(viewMatrix);
	m_Camera->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// ---- 对模型进行渲染 -----

	for each (auto model in m_modelsList) {
		m_D3D->GetWorldMatrix(worldMatrix);
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
		
		// 将 vertex和index 加载到 pipeline中， 渲染 模型
		model->Render(m_D3D->GetDeviceContext());

		m_Shader->SetLightParameters(m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
		result = m_Shader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(),
										worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
										NULL/*model->GetTexture()*/);

		if (!result) {
			return false;
		}
	}

	m_D3D->EndScene();

	return true;
}

