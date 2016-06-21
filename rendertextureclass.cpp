////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "rendertextureclass.h"


RenderTextureClass::RenderTextureClass()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;

	m_textureType = TextureType::ShadowTexture;

	m_viewSize = 0;
}


RenderTextureClass::RenderTextureClass(const RenderTextureClass& other)
{
}


RenderTextureClass::~RenderTextureClass()
{
}

/*
bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, DXGI_FORMAT format) {

	// Initialize the render target texture description.
	D3D11_TEXTURE2D_DESC textureDesc = {};

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;//DXGI_FORMAT_R32_FLOAT;//DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

	// Create the render target texture.
	CHECK_HRESULT(
		device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture)
	);
	
	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	CHECK_HRESULT(
		device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView)
	);
	

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	CHECK_HRESULT(
		device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView)
	);
	

	// --- 深度 Buffer 初始化 ---
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	
	// 构建description.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 创造depth buffer纹理。
	CHECK_HRESULT(
		device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)
	);

	// 初始化 depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	CHECK_HRESULT(
		device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)
	);
	


	// Setup the viewport for rendering.
    m_viewport.Width = (float)textureWidth;
    m_viewport.Height = (float)textureHeight;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

	return true;
}
*/
bool RenderTextureClass::Initialize(ID3D11Device * device, int textureWidth, int textureHeight, DXGI_FORMAT format, UINT viewSize) {
	// Initialize the render target texture description.
	D3D11_TEXTURE2D_DESC textureDesc = {};

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;//DXGI_FORMAT_R32_FLOAT;//DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	m_viewSize = viewSize;
	m_renderTargetTextureArr.resize(viewSize);
	m_renderTargetViewArr.resize(viewSize);
	m_shaderResourceViewArr.resize(viewSize);
	
	for (size_t i = 0;i < viewSize; ++i) {
		CHECK_HRESULT(device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTextureArr[i]));
		CHECK_HRESULT(device->CreateRenderTargetView(m_renderTargetTextureArr[i], &renderTargetViewDesc, &m_renderTargetViewArr[i]));
		CHECK_HRESULT(device->CreateShaderResourceView(m_renderTargetTextureArr[i], &shaderResourceViewDesc, &m_shaderResourceViewArr[i]));
	}


	// --- 深度 Buffer 初始化 ---
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 创造depth buffer纹理。
	CHECK_HRESULT(device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer));

	// 初始化 depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	CHECK_HRESULT(device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView));



	// Setup the viewport for rendering.
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return true;
}

bool RenderTextureClass::Initialize(ID3D11Device * device, int textureWidth, int textureHeight, DXGI_FORMAT * pFormats, UINT viewSize) {
	D3D11_TEXTURE2D_DESC textureDesc = {};

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	//textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	//renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	//shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	m_viewSize = viewSize;
	m_renderTargetTextureArr.resize(viewSize);
	m_renderTargetViewArr.resize(viewSize);
	m_shaderResourceViewArr.resize(viewSize);


	for (size_t i = 0;i < viewSize; ++i) {
		textureDesc.Format = pFormats[i];
		renderTargetViewDesc.Format = pFormats[i];
		shaderResourceViewDesc.Format = pFormats[i];
		CHECK_HRESULT(device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTextureArr[i]));
		CHECK_HRESULT(device->CreateRenderTargetView(m_renderTargetTextureArr[i], &renderTargetViewDesc, &m_renderTargetViewArr[i]));
		CHECK_HRESULT(device->CreateShaderResourceView(m_renderTargetTextureArr[i], &shaderResourceViewDesc, &m_shaderResourceViewArr[i]));
	}


	// --- 深度 Buffer 初始化 ---
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 创造depth buffer纹理。
	CHECK_HRESULT(device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer));

	// 初始化 depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	CHECK_HRESULT(device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView));



	// Setup the viewport for rendering.
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return true;
}


void RenderTextureClass::Shutdown()
{
	SAFE_RELEASE(m_depthStencilView);
	SAFE_RELEASE(m_depthStencilBuffer);
	SAFE_RELEASE(m_shaderResourceView);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_renderTargetTexture);

	for (size_t i = 0; i < m_viewSize; i++) {
		SAFE_RELEASE(m_renderTargetTextureArr[i]);
		SAFE_RELEASE(m_renderTargetViewArr[i]);
		SAFE_RELEASE(m_shaderResourceViewArr[i]);
	}

	
}

void RenderTextureClass::SetTextureType(TextureType type) {
	m_textureType = type;
}

TextureType RenderTextureClass::GetTextureType() {
	return m_textureType;
}


void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	//deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	deviceContext->OMSetRenderTargets(m_renderTargetViewArr.size(), m_renderTargetViewArr.data(), m_depthStencilView);
	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);
	
}


void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };


	// Clear the back buffer.
	//deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	for each (auto & view in m_renderTargetViewArr) {
		deviceContext->ClearRenderTargetView(view, color);
	}
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView(UINT index )
{
	if (index < m_shaderResourceViewArr.size())
		return m_shaderResourceViewArr[index];
	else
		return NULL;
	//return m_shaderResourceView;
}


void RenderTextureClass::BeginRender(D3DClass* d3d, float r, float g, float b, float a) {
	SetRenderTarget(d3d->GetDeviceContext());
	ClearRenderTarget(d3d->GetDeviceContext(),r,g,b,a);
}

void RenderTextureClass::EndRender(D3DClass * d3d) {
	d3d->SetBackBufferRenderTarget();
	d3d->ResetViewport();
}

bool RenderTextureClass::CreateRenderTexture(ID3D11Device * device, RenderTextureClass ** ppRenderTex, TextureType texType) {
	bool result = true;
	RenderTextureClass* renderTex = new RenderTextureClass;
	if (!renderTex) return false;

	renderTex->SetTextureType(texType);
	switch (texType) {
		case ScreenTexture:
			result = renderTex->Initialize(device, SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
		case ShadowTexture:
			result = renderTex->Initialize(device, SHADOWMAP_WIDTH, SHADOWMAP_WIDTH);
			break;
		case WarpMapTexture:
			result = renderTex->Initialize(device, SHADOWMAP_WIDTH, 2);
			break;
		case ScreenColorTexture:
			result = renderTex->Initialize(device, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_R32G32B32A32_FLOAT);
			break;
		default:
			result = renderTex->Initialize(device, SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
	}
	if (!result) {
		Log::GetInstance()->LogMsg("Could not initialize the render to texture object.");
		delete renderTex;
	}
	else {
		*ppRenderTex = renderTex;
	}
	return result;
}
