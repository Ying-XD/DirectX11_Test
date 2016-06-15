////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"
#include <D3DX11tex.h>
bool SaveTextureToFile(ID3D11DeviceContext * context, ID3D11ShaderResourceView * rv, WCHAR * filename) {
#if defined(DEBUG) || defined(_DEBUG)

	ID3D11Texture2D * tex = NULL;
	ID3D11Resource * res = NULL;

	rv->GetResource(&res);
	res->QueryInterface(&tex);

	HRESULT hr = D3DX11SaveTextureToFile(context, tex, D3DX11_IFF_DDS, filename);

	CHECK_HRESULT(hr);
#endif // DEBUG
	return true;
}


GraphicsClass::GraphicsClass()
{
	m_D3D			= 0;
	m_Camera		= 0;
	m_CubeModel		= 0;
	m_GroundModel	= 0;
	m_GroundModel2	= 0;
	m_SphereModel	= 0;
	m_Light			= 0;

	m_DepthShader	= 0;	
	m_ShadowShader	= 0;
	m_ImpMapShader	= 0;
	m_ImpMapTex		= 0;
	m_FullscreenShader = 0;

	m_DepLightTex	= 0;
	m_DepViewTex	= 0;
	m_NVTex			= 0;
	m_NormalTex		= 0;
	m_compactTex	= 0;
	m_blurTex		= 0;
	m_warpMapTex	= 0;
	m_rtwDepthTex	= 0;
	m_colorTex		= 0;
	m_DeferredTex	= 0;

	m_rtwShadow_Backward	= 0;

	m_deferredShader	= 0;
	m_warpMapShader		= 0;
	m_rtwDepthShader	= 0;

	m_keysPressed.Reset();
	
	m_switch.num_8 = true;
	m_rtwType	= RTW_Type::Backward;
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
	if(!m_D3D) {
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if(!result)	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}
	ID3D11Device* device = m_D3D->GetDevice();

	// Create the camera object.
	m_Camera = new Camera;
	if(!m_Camera) {
		return false;
	}

	m_Camera->SetPosition(0.0f, 5.0f, -10.0f);
	
	
	// Create the cube model object.
	m_CubeModel = new ModelClass;
	if (!m_CubeModel || !m_CubeModel->Initialize(device, "./data/cube.txt", L"./data/wall01.dds")) {
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	m_CubeModel->SetPosition(-10.0f, 1.0f, 0.0f);
	
	// Create the sphere model object.
	m_SphereModel = new ModelClass;
	if(!m_SphereModel || !m_SphereModel->Initialize(device, "./data/sphere.txt", L"./data/ice.dds")) {
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}
	m_SphereModel->SetPosition(10.0f, 1.0f, .0f);
	
	// Create the ground model object.
	m_GroundModel = new ModelClass;
	if(!m_GroundModel || !m_GroundModel->Initialize(device, "./data/plane01.txt", L"./data/metal001.dds")) {
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	m_GroundModel2 = new ModelClass;
	if (!m_GroundModel2 || !m_GroundModel2->Initialize(device, "./data/plane01.txt", L"./data/metal001.dds")) {
		return false;
	}
	// Set the position for the ground model.
	m_GroundModel->SetPosition(10.0f, 0.0f, 0.0f);
	m_GroundModel2->SetPosition(-10.0f, 0.0f, 0.0f);

	m_models.push_back(m_CubeModel);
	m_models.push_back(m_SphereModel);

	m_sence.push_back(m_CubeModel);
	m_sence.push_back(m_SphereModel);
	m_sence.push_back(m_GroundModel);
	m_sence.push_back(m_GroundModel2);

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

	
	// --- Create Render Texture ---
	CHECK_RESULT(
		//CreateRenderTexture(&m_colorTex, TextureType::ScreenColorTexture) &&
		CreateRenderTexture(&m_ImpMapTex, TextureType::ShadowTexture)	&&
		//CreateRenderTexture(&m_DepViewTex, TextureType::ScreenTexture)	&
		CreateRenderTexture(&m_DepViewTex, TextureType::ShadowTexture)	&&
		CreateRenderTexture(&m_DepLightTex, TextureType::ShadowTexture)	&&
		CreateRenderTexture(&m_DepShadowTex, TextureType::ShadowTexture)&&
		CreateRenderTexture(&m_NVTex, TextureType::ShadowTexture)		&&
		CreateRenderTexture(&m_NormalTex, TextureType::ShadowTexture)	&&

		CreateRenderTexture(&m_compactTex,TextureType::WarpMapTexture)	&&
		CreateRenderTexture(&m_blurTex, TextureType::WarpMapTexture)	&&
		CreateRenderTexture(&m_warpMapTex, TextureType::WarpMapTexture) &&

		CreateRenderTexture(&m_rtwDepthTex, TextureType::ShadowTexture)
	);
	DXGI_FORMAT formats[] = {DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT };

	m_colorTex = new RenderTextureClass;
	if (!m_colorTex || !m_colorTex->Initialize(device, SCREEN_WIDTH, SCREEN_HEIGHT, formats, 2))
		return false;


	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if(!m_DepthShader) {
		return false;
	}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(device, hwnd);
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the shadow shader object.
	m_ShadowShader = new ShadowShaderClass;
	if(!m_ShadowShader) {
		return false;
	}

	// Initialize the shadow shader object.
	result = m_ShadowShader->Initialize(device, hwnd);
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	m_ImpMapShader = new ImportanceMapClass;
	if (!m_ImpMapShader || !m_ImpMapShader->Initialize(device, hwnd)) {
		return false;
	}
	
	m_FullscreenShader = new FullscreenShader;
	if (!m_FullscreenShader || !m_FullscreenShader->Initialize(device))
		return false;

	m_warpMapShader = new WarpMapShader;
	if (!m_warpMapShader || !m_warpMapShader->Initialize(device)) {
		return false;
	}
	
	m_rtwDepthShader = new RTWDepthShader;
	if (!m_rtwDepthShader || !m_rtwDepthShader->Initialize(device))
		return false;

	m_deferredShader = new DeferredShader;
	if (!m_deferredShader || !m_deferredShader->Initialize(device, hwnd)) {
		return false;
	}

	m_rtwShadow_Backward = new RTW_Shadow_Backward;
	CHECK_RESULT(m_rtwShadow_Backward && m_rtwShadow_Backward->Initialize(device, hwnd));

	return true;
}


void GraphicsClass::Shutdown()
{

	//RELEASE_RTEXTURE(m_RenderTexture);
	SHUTDOWN_RTEXTURE(m_colorTex);
	SHUTDOWN_RTEXTURE(m_ImpMapTex);
	SHUTDOWN_RTEXTURE(m_DepViewTex);
	SHUTDOWN_RTEXTURE(m_DepLightTex);
	SHUTDOWN_RTEXTURE(m_DepShadowTex);
	SHUTDOWN_RTEXTURE(m_DeferredTex);
	SHUTDOWN_RTEXTURE(m_NVTex);
	SHUTDOWN_RTEXTURE(m_NormalTex);

	SHUTDOWN_RTEXTURE(m_compactTex);
	SHUTDOWN_RTEXTURE(m_blurTex);
	SHUTDOWN_RTEXTURE(m_warpMapTex);
	SHUTDOWN_RTEXTURE(m_rtwDepthTex);


	SHUTDOWN_SHADER(m_ShadowShader);
	SHUTDOWN_SHADER(m_DepthShader);
	SHUTDOWN_SHADER(m_FullscreenShader);
	SHUTDOWN_SHADER(m_warpMapShader);
	SHUTDOWN_SHADER(m_rtwDepthShader);
	SHUTDOWN_SHADER(m_deferredShader);
	SHUTDOWN_SHADER(m_rtwShadow_Backward);

	// Release model object
	for each (auto model in m_sence) {
		SHUTDOWN_MODEL(model);
	}

	
	// Release the camera object.
	SAFE_DELETE(m_Camera);

	// Release the light object.
	SAFE_DELETE(m_Light);


	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

}


bool GraphicsClass::Frame() {
	/*
	Camera Pos:	1.73823, 2.1998, -4.0246
	Camera LookAt:	0.072943, -0.0864469, 0.993583
	Light Pos:	1.59372, 8, -5

	Camera Pos:	1.38433, 18.0546, -18.3194
	Camera LookAt:	-1.32609e-07, -0.580938, 0.813948
	Light Pos:	0.397182, 16, -5


	Camera Pos:	-1.18223, 5, -4.75942
	Camera LookAt:	-0.0644936, -0.718185, 0.692857
	Light Pos:	1.45612, 16, -5
	*/
	m_switch.num_8 = true;


	static D3DXVECTOR3	lightPos(10.0f, 16.0f, -5.0f);
	static float df = 0.05f * m_frameTime;

	if (m_switch.o) {
		m_Camera->SetPosition(-1.18223, 5, -4.75942);
		m_Camera->SetLookAt(-0.0644936, -0.718185, 0.692857);
		lightPos = D3DXVECTOR3(1.45612, 16, -5);
		m_Camera->Render();
	}
	else {

		if (lightPos.x > 20.0f) {
			df = -1.0 * df;
			lightPos.x = 20.0f;
		}
		else
			if (lightPos.x < -20.0f) {
				df = -1.0 * df;
				lightPos.x = -20.0f;
			}

		if (!m_switch.space)
			lightPos.x += df;
		else {
			if (m_keysPressed.home)
				lightPos.x += df;
			if (m_keysPressed.end)
				lightPos.x -= df;
		}
		m_Camera->Render(&m_keysPressed);
	}
	m_Light->SetPosition(lightPos);
	m_Light->GenerateViewMatrix();


	if (m_keysPressed.tab) {
		auto cPos = m_Camera->GetPosition();
		auto cLookAt = m_Camera->GetLookAt();
		Log::GetInstance()->LogVec3(cPos, "Camera Pos");
		Log::GetInstance()->LogVec3(cLookAt, "Camera LookAt");
		Log::GetInstance()->LogVec3(lightPos, "Light Pos");
	}

	CHECK_RESULT(
		RenderLightDepthTexture(m_DepLightTex)
		);

	

	if (m_rtwType == RTW_Type::Conventional)
		return conventionalShadowmap();
	
	if (m_rtwType == RTW_Type::Forward)
		RenderToImpMap_Forward();
	if (m_rtwType == RTW_Type::Backward)
		RenderToImpMap_Backward();

	BuildWarpingMaps();

	if (DrawTempTexture()) return true;

	if (m_rtwType == Forward)
		return ShadowMapping();
	if (m_rtwType == Backward)
		return ShadowMapping();
		/*if (!m_switch.num_7)
			return ShadowMapping();
		else
			return ShadowMapping_Backward();*/



	return true;
}


void GraphicsClass::SetLightPostion(float x, float y, float z) {
	m_Light->SetPosition(x,y,z);
}

void GraphicsClass::SetKeysPressed(KeysPressed keysPressed) {
	for (size_t i = 0;i < KEYS_NUM; ++i) {
		m_switch._key[i] = m_keysPressed._key[i] & !keysPressed._key[i] ^ m_switch._key[i];
	}
	if (m_keysPressed.num_0 & !keysPressed.num_0)
		m_rtwType = RTW_Type::Conventional;
	if (m_keysPressed.num_9 & !keysPressed.num_9)
		m_rtwType = RTW_Type::Backward;
	if (m_keysPressed.num_8 & !keysPressed.num_8)
		m_rtwType = RTW_Type::Forward;

	if (m_keysPressed.F1 & !keysPressed.F1)	G_Flag = 1;
	if (m_keysPressed.F2 & !keysPressed.F2) G_Flag = 2;
	if (m_keysPressed.F3 & !keysPressed.F3) G_Flag = 3;
	if (m_keysPressed.F4 & !keysPressed.F4)	G_Flag = 4;
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


bool GraphicsClass::CreateRenderTexture(RenderTextureClass ** renderTexPtr, TextureType texType) {
	bool result = true;
	RenderTextureClass* renderTex = new RenderTextureClass;
	if (!renderTex) return false;
	
	renderTex->SetTextureType(texType);
	switch (texType) {
		case ScreenTexture:
			result = renderTex->Initialize(m_D3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
		case ShadowTexture:
			result = renderTex->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_WIDTH);
			break;
		case WarpMapTexture:
			result = renderTex->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, 2);
			break;
		case ScreenColorTexture:
			result = renderTex->Initialize(m_D3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);
			break;
		default:
			result = renderTex->Initialize(m_D3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
	}
	if (!result) {
		Log::GetInstance()->LogMsg("Could not initialize the render to texture object.");
		delete renderTex;
	}
	else {
		*renderTexPtr = renderTex;
	}
	return result;
}

bool GraphicsClass::RenderToImpMap_Forward() {
	
	CHECK_RESULT(
		//RenderLightDepthTexture(m_DepLightTex) &
		RenderNVTexture(m_NVTex)
	);

	D3DXMATRIX vCameraMatr, pCameraMatr;
	D3DXMATRIX vLightMatr, pLightMatr;
	D3DXMATRIX lv2cvMatr;

	m_Camera->GetViewMatrix(vCameraMatr);
	m_Camera->GetProjectionMatrix(pCameraMatr);
	
	m_Light->GetViewMatrix(vLightMatr);
	m_Light->GetProjectionMatrix(pLightMatr);

	GetLv2Cv(&lv2cvMatr, vCameraMatr, pCameraMatr, vLightMatr, pLightMatr);

	m_ImpMapTex->BeginRender(m_D3D,1.5f);

	CHECK_RESULT(
		m_ImpMapShader->Render_Forward(m_D3D->GetDeviceContext(), m_DepLightTex->GetShaderResourceView(),m_NVTex->GetShaderResourceView(), lv2cvMatr)
	);

	m_ImpMapTex->EndRender(m_D3D);

	return true;
}

bool GraphicsClass::RenderToImpMap_Backward() {
	CHECK_RESULT(
		RenderShadowDepthTexture(m_DepShadowTex, m_DepLightTex->GetShaderResourceView())
		//RenderNVTexture(m_NVTex)
		);

	D3DXMATRIX vCameraMatr, pCameraMatr;
	D3DXMATRIX vLightMatr, pLightMatr;
	D3DXMATRIX cv2lvMatr;
	D3DXMATRIX lv2cvMatr;

	m_Camera->GetViewMatrix(vCameraMatr);
	m_Camera->GetProjectionMatrix(pCameraMatr);

	m_Light->GetViewMatrix(vLightMatr);
	m_Light->GetProjectionMatrix(pLightMatr);

	GetCv2Lv(&cv2lvMatr, vCameraMatr, pCameraMatr, vLightMatr, pLightMatr);

	m_ImpMapTex->BeginRender(m_D3D, 0.0f);

	CHECK_RESULT(
		m_ImpMapShader->Render_Backward(m_D3D->GetDeviceContext(), m_DepShadowTex->GetShaderResourceView(),
										m_NVTex->GetShaderResourceView(), cv2lvMatr)
		);
		
	
	m_ImpMapTex->EndRender(m_D3D);

}

bool GraphicsClass::BuildWarpingMaps() {

	m_compactTex->BeginRender(m_D3D);
	m_warpMapShader->Comapct(m_D3D->GetDeviceContext(), m_ImpMapTex->GetShaderResourceView());
	m_compactTex->EndRender(m_D3D);

	m_blurTex->BeginRender(m_D3D);
	m_warpMapShader->Blur(m_D3D->GetDeviceContext(), m_compactTex->GetShaderResourceView());
	m_blurTex->EndRender(m_D3D);

	m_warpMapTex->BeginRender(m_D3D);
	m_warpMapShader->BuildWarpMap(m_D3D->GetDeviceContext(), m_blurTex->GetShaderResourceView());
	m_warpMapTex->EndRender(m_D3D);

	RTW_RenderLightDepthTexture(m_rtwDepthTex,m_warpMapTex->GetShaderResourceView());
	
	return true;
}





bool GraphicsClass::conventionalShadowmap() {
	
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

	ID3D11ShaderResourceView* depthMapTexture = m_DepLightTex->GetShaderResourceView();
	for each (auto model in m_sence) {
		m_D3D->GetWorldMatrix(worldMatrix);
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

		// 将 vertex和index 加载到 pipeline中， 渲染 模型
		model->Render(m_D3D->GetDeviceContext());

		m_ShadowShader->SetLightParameters(m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
		result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(),
										worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
										model->GetTexture(), depthMapTexture, NULL);

		if (!result) {
			return false;
		}
	}


	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::ShadowMapping()
{
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

	ID3D11ShaderResourceView* depthMapTexture = m_rtwDepthTex->GetShaderResourceView();
	ID3D11ShaderResourceView* warpMaps = m_warpMapTex->GetShaderResourceView();
	size_t i =0;
	for each (auto model in m_sence) {

		m_D3D->GetWorldMatrix(worldMatrix);
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

		// 将 vertex和index 加载到 pipeline中， 渲染 模型
		model->Render(m_D3D->GetDeviceContext());


		m_ShadowShader->SetLightParameters(m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
		result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), 
										worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,	lightProjectionMatrix, 
										model->GetTexture(), depthMapTexture, warpMaps);

		CHECK_RESULT(result);
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::ShadowMapping_Backward() {
	bool result;
	float posX, posY, posZ;
	D3DXMATRIX vCameraMatr, pCameraMatr;
	D3DXMATRIX vLightMatr, pLightMatr;
	D3DXMATRIX cv2lvMatr;
	RenderViewDepthTexture(m_DepViewTex);

	m_Camera->GetViewMatrix(vCameraMatr);
	m_Camera->GetProjectionMatrix(pCameraMatr);

	m_Light->GetViewMatrix(vLightMatr);
	m_Light->GetProjectionMatrix(pLightMatr);
	D3DXVECTOR4 ambientColor = m_Light->GetAmbientColor();
	GetCv2Lv(&cv2lvMatr, vCameraMatr, pCameraMatr, vLightMatr, pLightMatr);

	RenderColorTexture(m_colorTex);
	auto rtwDepTexture	= m_rtwDepthTex->GetShaderResourceView();
	auto colorTexture	= m_colorTex->GetShaderResourceView(0);
	auto diffuseTexture	= m_colorTex->GetShaderResourceView(1);

	auto warpMaps		= m_warpMapTex->GetShaderResourceView();
	auto viewDepTexture	= m_DepViewTex->GetShaderResourceView();

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	result = m_rtwShadow_Backward->Render(m_D3D->GetDeviceContext(), cv2lvMatr, ambientColor,
								 warpMaps, colorTexture, diffuseTexture,
										  rtwDepTexture, viewDepTexture);
	//result = m_FullscreenShader->Render(m_D3D->GetDeviceContext(), colorTexture, m_colorTex->GetTextureType());
	CHECK_RESULT(result);
	m_D3D->EndScene();


	return true;
}

bool GraphicsClass::GetDepthTexture(RenderTextureClass * renderTex, D3DXMATRIX viewMatrix, D3DXMATRIX projMatrix, ShaderType texType) {
	D3DXMATRIX worldMatrix;
	D3DXVECTOR3 cameraPos;
	float posX, posY, posZ;
	m_Camera->GetPosition(&cameraPos);

	renderTex->BeginRender(m_D3D, 1.5f);
	int k = 0;
	
	for each(auto & model in m_models) {
		m_D3D->GetWorldMatrix(worldMatrix);
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

		model->Render(m_D3D->GetDeviceContext());
		CHECK_RESULT(
			m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), 
								  worldMatrix, viewMatrix, projMatrix, cameraPos,texType)
			);
	}
	renderTex->EndRender(m_D3D);
	//m_D3D->ResetBackuBufferRTandViewport();

	return true;
}

bool GraphicsClass::RenderNVTexture(RenderTextureClass * renderTex) {
	D3DXMATRIX viewMatr, projMatr;
	if (m_rtwType == Backward) {
		m_Camera->GetViewMatrix(viewMatr);
		m_Camera->GetProjectionMatrix(projMatr);
	}
	if (m_rtwType == Forward) {
		m_Light->GetViewMatrix(viewMatr);
		m_Light->GetProjectionMatrix(projMatr);
	}
	return GetDepthTexture(renderTex, viewMatr, projMatr, ShaderType::NVShader);
}

bool GraphicsClass::RenderNormalTexture(RenderTextureClass * renderTex) {
	return false;
}

bool GraphicsClass::RenderLightDepthTexture(RenderTextureClass * renderTex) {
	D3DXMATRIX viewMatr, projMatr;
	// 获得 light的深度缓存
	m_Light->GetViewMatrix(viewMatr);
	m_Light->GetProjectionMatrix(projMatr);
	return GetDepthTexture(renderTex, viewMatr, projMatr,ShaderType::DepthShader);
}

bool GraphicsClass::RenderViewDepthTexture(RenderTextureClass * renderTex) {
	D3DXMATRIX viewMatr, projMatr;
	m_Camera->GetViewMatrix(viewMatr);
	m_Camera->GetProjectionMatrix(projMatr);
	//return GetDepthTexture(renderTex, viewMatr, projMatr, ShaderType::DepthShader);

	D3DXMATRIX worldMatrix;
	D3DXVECTOR3 cameraPos;
	float posX, posY, posZ;
	m_Camera->GetPosition(&cameraPos);

	renderTex->BeginRender(m_D3D, 1.5f);
	int k = 0;

	for each(auto & model in m_models) {
		m_D3D->GetWorldMatrix(worldMatrix);
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

		model->Render(m_D3D->GetDeviceContext());
		CHECK_RESULT(
			m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(),
								  worldMatrix, viewMatr, projMatr, cameraPos, ShaderType::DepthShader)
			);
	}
	renderTex->EndRender(m_D3D);
}

bool GraphicsClass::RTW_RenderLightDepthTexture(RenderTextureClass * renderTex, ID3D11ShaderResourceView* warpMap) {
	D3DXMATRIX viewMatr, projMatr, worldMatr;
	m_Light->GetViewMatrix(viewMatr);
	m_Light->GetProjectionMatrix(projMatr);
	float tessAmount = 0.0f;
	float posX, posY, posZ;
	bool result;

	renderTex->BeginRender(m_D3D,10.05f);
	for (size_t i = 0;i < m_models.size(); i++) {
		tessAmount = (i == 0) ? 30.0f : 3.0f;
		auto & model = m_models[i];
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatr, posX, posY, posZ);
		model->Render(m_D3D->GetDeviceContext());
//#if TESSELLATION
//		result = m_rtwDepthShader->Render_Tessellation(m_D3D->GetDeviceContext(), model->GetIndexCount(),
//												  worldMatr, viewMatr, projMatr, warpMap, tessAmount);
//#else
//		result = m_rtwDepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(),
//									 worldMatr, viewMatr, projMatr,warpMap);
//#endif
		result = m_rtwDepthShader->Render_Tessellation(m_D3D->GetDeviceContext(), model->GetIndexCount(),
															  worldMatr, viewMatr, projMatr, warpMap, tessAmount);
		//result = m_rtwDepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(),
		//								  worldMatr, viewMatr, projMatr,warpMap);
		CHECK_RESULT(result);
	}

	renderTex->EndRender(m_D3D);
	return true;
}

bool GraphicsClass::RenderShadowDepthTexture(RenderTextureClass * renderTex, ID3D11ShaderResourceView * lightDepTex) {

	D3DXMATRIX viewMatr, projMatr, worldMatr;
	D3DXMATRIX lightViewMatr, lightPorjMatr;
	D3DXVECTOR3 cameraPos, lightPos;


	m_Camera->GetViewMatrix(viewMatr);
	m_Camera->GetProjectionMatrix(projMatr);
	cameraPos = m_Camera->GetPosition();

	m_Light->GetViewMatrix(lightViewMatr);
	m_Light->GetProjectionMatrix(lightPorjMatr);
	lightPos = m_Light->GetPosition();

	float posX, posY, posZ;
	renderTex->BeginRender(m_D3D, 1.5f);
	for each(auto & model in m_sence) {
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatr, posX, posY, posZ);
		model->Render(m_D3D->GetDeviceContext());
		CHECK_RESULT(
		m_DepthShader->Render_ShadowDepth(m_D3D->GetDeviceContext(),model->GetIndexCount(),
										  worldMatr, viewMatr, projMatr,
										  lightViewMatr, lightPorjMatr, lightPos,
										  lightDepTex)
			);

	}

	renderTex->EndRender(m_D3D);
	return true;
}

bool GraphicsClass::RenderColorTexture(RenderTextureClass * renderTex) {
	D3DXMATRIX worldMatrix, viewMatrix, projMatrix;
	float posX, posY, posZ;
	D3DXVECTOR3 lightPos;
	D3DXVECTOR4 ambient, diffuse;

	lightPos = m_Light->GetPosition();
	ambient = m_Light->GetAmbientColor();
	diffuse = m_Light->GetDiffuseColor();

	m_Camera->GetViewMatrix(viewMatrix);
	m_Camera->GetProjectionMatrix(projMatrix);

	renderTex->BeginRender(m_D3D);
	size_t i = 0;
	for each (auto model in m_sence) {
		m_D3D->GetWorldMatrix(worldMatrix);
		model->GetPosition(posX, posY, posZ);
		D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
		model->Render(m_D3D->GetDeviceContext());
		CHECK_RESULT(
			m_deferredShader->Render_Color(m_D3D->GetDeviceContext(), model->GetIndexCount(),
									   worldMatrix, viewMatrix, projMatrix, 
									   lightPos, ambient, diffuse,
									   model->GetTexture())
		);
	}

	renderTex->EndRender(m_D3D);

	return true;
}



bool GraphicsClass::DrawTempTexture() {
	RenderTextureClass * renderTex = NULL;
	UINT	index = 0;	
	if (m_switch.num_1)		renderTex = m_ImpMapTex;
	else
	if (m_switch.num_2)		renderTex = m_DepLightTex;
	else
	if (m_switch.num_3)		renderTex = m_rtwDepthTex;
	else
	if (m_switch.num_4)		renderTex = m_DepShadowTex;
	else
	if (m_switch.num_5)		renderTex =	m_compactTex;
	else
	if (m_switch.num_6)		renderTex = m_blurTex;
	
	if (!renderTex) return false;

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

		m_FullscreenShader->Render(m_D3D->GetDeviceContext(), renderTex->GetShaderResourceView(), renderTex->GetTextureType());
	m_D3D->EndScene();



	return true;
}

void GetLv2Cv(D3DXMATRIX* lv2cvPtr, D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr,
			  D3DXMATRIX vLightMatr, D3DXMATRIX pLightMatr) {

	D3DXMatrixInverse(&vLightMatr, NULL, &vLightMatr);
	D3DXMatrixInverse(&pLightMatr, NULL, &pLightMatr);

	D3DXMatrixMultiply(lv2cvPtr, &pLightMatr, &vLightMatr);
	D3DXMatrixMultiply(lv2cvPtr, lv2cvPtr, &vCameraMatr);
	D3DXMatrixMultiply(lv2cvPtr, lv2cvPtr, &pCameraMatr);

}

void GetCv2Lv(D3DXMATRIX* cv2lvPtr, D3DXMATRIX vCameraMatr, D3DXMATRIX pCameraMatr,
			  D3DXMATRIX vLightMatr, D3DXMATRIX pLightMatr) {
	D3DXMatrixInverse(&vCameraMatr, NULL, &vCameraMatr);
	D3DXMatrixInverse(&pCameraMatr, NULL, &pCameraMatr);

	D3DXMatrixMultiply(cv2lvPtr, &pCameraMatr, &vCameraMatr);
	D3DXMatrixMultiply(cv2lvPtr, cv2lvPtr, &vLightMatr);
	D3DXMatrixMultiply(cv2lvPtr, cv2lvPtr, &pLightMatr);

}
