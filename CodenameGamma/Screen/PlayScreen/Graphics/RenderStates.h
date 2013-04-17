#pragma once
#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "..\..\..\stdafx.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;

	static ID3D11DepthStencilState* EqualsDSS;
	static ID3D11DepthStencilState* LessDDS;
	 
	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;
};

#endif

