#pragma once
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#define UNICODE


#include "..\..\..\stdafx.h"
#include <map>


class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	void Init(ID3D11Device* device);

	ID3D11ShaderResourceView* CreateTexture(std::string filename);

private:
	TextureManager(const TextureManager& rhs);
	TextureManager& operator=(const TextureManager& rhs);
	
private:
	ID3D11Device* md3dDevice;
	std::map<std::string, ID3D11ShaderResourceView*> mTextureSRV;
};

#endif