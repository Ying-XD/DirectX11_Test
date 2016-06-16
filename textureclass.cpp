////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"


TextureClass::TextureClass()
{
	m_texture = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass() {
}


bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	// Load the texture in.
	CHECK_HRESULT(
		D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL)
	);

	return true;
}


void TextureClass::Shutdown()
{
	// Release the texture resource.
	SAFE_RELEASE(m_texture);

}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}