#pragma once
#ifndef BLENDMAP_H
#define BLENDMAP_H
#include "..\..\..\stdafx.h"
#include <iostream>
#include <vector>

class BlendMap
{
	XMFLOAT4**	m_Map;
	int			m_Width;
	int			m_Height;

public:
	BlendMap(void);
	~BlendMap(void);

	void Load(std::string, int Width, int Height);
	XMFLOAT4 GetBlendData(float x, float z);
};
#endif
