#pragma once
#ifndef SCREEN_H
#define SCREEN_H

#include "../stdafx.h"
#include "../Input/InputManager.h"
#include "../Sound/SoundManager.h"


#include "../FW1FontWrapper.h"
#pragma comment (lib, "FW1FontWrapper.lib")

using namespace std;

struct ScreenData
{
	ScreenData()
	{

	}

	int	WIDTH, HEIGHT;

	ID3D11Device*			DEVICE;
	ID3D11DeviceContext*	DEVICE_CONTEXT;

	IDXGISwapChain*         SWAP_CHAIN;
	ID3D11RenderTargetView* RENDER_TARGET_VIEW;

	ID3D11Texture2D*        DEPTH_STENCIL;
	ID3D11DepthStencilView* DEPTH_STENCIL_VIEW;

	IFW1FontWrapper*	TEXT_INSTANCE;
};

enum ScreenType
{
	MAIN_MENU_SCREEN,
	OPTIONS_SCREEN,
	PLAY_SCREEN,
	CREDITS_SCREEN,




	EXIT_SCREEN,
	UNDEFINED_SCREEN
};

//	Add colors by writing
//	Name	=	0xAaBbGgRr
//	Aa	=	Alpha
//	Bb	=	Blue
//	Gg	=	Green
//	Rr	=	Red
enum	TextColor
{
	White	=	(UINT32)0xFFFFFFFF,
	Black	=	(UINT32)0xFF000000,
	Red		=	(UINT32)0xFF0000FF,
	Green	=	(UINT32)0xFF00FF00,
	Blue	=	(UINT32)0xFFFF0000,
	Yellow	=	(UINT32)0xFF00FFFF,
	Orange	=	(UINT32)0xFF00CCFF
};

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