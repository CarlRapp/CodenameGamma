#include "TextureManager.h"

TextureManager::TextureManager() : md3dDevice(0)
{
}

TextureManager::~TextureManager()
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
    {
		ReleaseCOM(it->second);
    }

	mTextureSRV.clear();
}

void TextureManager::Init(ID3D11Device* device)
{
	md3dDevice = device;
}

ID3D11ShaderResourceView* TextureManager::CreateTexture(std::string filename)
{
	ID3D11ShaderResourceView* srv = 0;

	// Does it already exist?
	if( mTextureSRV.find(filename) != mTextureSRV.end() )
	{
		srv = mTextureSRV[filename];
	}
	else
	{
		std::wstring widestr = std::wstring(filename.begin(), filename.end());
		D3DX11CreateShaderResourceViewFromFile(md3dDevice, widestr.c_str(), 0, 0, &srv, 0 );

		mTextureSRV[filename] = srv;
	}

	return srv;
}
 
