#pragma once
#ifndef SCREEN_H
#define SCREEN_H

#include "ScreenEnums.h"

using namespace std;

class Screen
{
protected:
	bool	gInitialized, gDestroyed;
	int		gScreenWidth, gScreenHeight;

	ID3D11Device*			gDevice;
	ID3D11DeviceContext*	gDeviceContext;

	IDXGISwapChain*         gSwapChain;
	ID3D11RenderTargetView* gRenderTargetView;

	ID3D11Texture2D*        gDepthStencil;
	ID3D11DepthStencilView* gDepthStencilView;

	IFW1FontWrapper*		gTextInstance;

	ScreenType	gGotoNextFrame;

	virtual	bool	Load();
	virtual	bool	Unload();

	void	LoadScreenData(ScreenData* Setup);
	float	GetTextWidth(string Text, float TextSize);
public:
	Screen(void);
	Screen(ScreenData* Setup);

	void	Initialize();
	void	Destroy();

	virtual	void	Reset();
	virtual	void	Update(float DeltaTime);
	virtual	void	Render();

	virtual	ScreenType	GetScreenType();

	ScreenType	SwapToThisNextFrame();

	void	DrawString(IFW1FontWrapper& Instance, string Text, float x, float y, float Size, TextColor Color, UINT Flags);
	void	DrawString(IFW1FontWrapper& Instance, string Text, float x, float y, float Size, TextColor Color, TextColor BorderColor, float BorderSize, UINT Flags);
};
#endif