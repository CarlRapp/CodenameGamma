#pragma once
#ifndef SCREENENUMS
#define SCREENENUMS

#include "../stdafx.h"
#include "../Input/InputManager.h"
#include "../Sound/SoundManager.h"

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

struct ScreenData
{
	ScreenData(){}

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

struct DebugData
{
public:
	DebugData(){}

	string	Title;
	string	Value;

	TextColor	TitleColor;
	TextColor	ValueColor;
};

class DebugScreen
{
private:
	struct LogData
	{
		string		MESSAGE;
		TextColor	COLOR;

		LogData(string Message, TextColor Color)
		{
			MESSAGE	=	Message;
			COLOR	=	Color;
		}
	};

protected:
	static	DebugScreen*	gInstance;
	DebugScreen(ScreenData* Setup);

	IFW1FontWrapper*		gTextInstance;
	ID3D11DeviceContext*	gDeviceContext;

	vector<DebugData*>*	gDebugData;
	vector<LogData*>*	gDebugLog;

	int		gTextSize;
	int		gScreenHeight, gScreenWidth;
	int		gMarginX, gMarginY;
	int		gLogIndex, gLogMaxRows;

	float	GetTextWidth(string Text, float TextSize);

	void	DrawString(string Text, int x, int y, float Size, TextColor Color, UINT Flags);
	void	DrawString(string Text, int x, int y, float Size, TextColor Color, TextColor BorderColor, int BorderSize, UINT Flags);

public:
	~DebugScreen(void);

	static	DebugScreen*	GetInstance(void);
	static	void			Initialize(ScreenData* Setup);


	DebugData*	AddDebugData(string Title);
	void		RemoveDebugData(DebugData* Instance, bool DeletePointer);

	void	AddLogMessage(string Text, TextColor Color);

	void	Update(float DeltaTime);
	void	Render(void);
};

#endif