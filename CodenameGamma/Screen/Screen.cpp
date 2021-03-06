#include "Screen.h"

GraphicsManager*	Screen::gGraphicsManager	=	0;
Screen::Screen(ScreenData*	Setup)
{
	gInitialized	=	false;
	gDestroyed		=	false;

	LoadScreenData(Setup);
	gScreenData	=	Setup;
}
Screen::Screen(void)
{
	gInitialized	=	false;
	gDestroyed		=	false;
}

void Screen::LoadScreenData(ScreenData* Setup)
{
	gScreenData		=	Setup;
	gScreenWidth	=	Setup->WIDTH;
	gScreenHeight	=	Setup->HEIGHT;

	gDevice				=	Setup->DEVICE;
	gDeviceContext		=	Setup->DEVICE_CONTEXT;

	gSwapChain			=	Setup->SWAP_CHAIN;
	gRenderTargetView	=	Setup->RENDER_TARGET_VIEW;

	gDepthStencil		=	Setup->DEPTH_STENCIL;
	gDepthStencilView	=	Setup->DEPTH_STENCIL_VIEW;

	gTextInstance		=	Setup->TEXT_INSTANCE;

	gGotoNextFrame		=	UNDEFINED_SCREEN;

	gFullscreenVP			=	D3D11_VIEWPORT();
	gFullscreenVP.TopLeftX	=	0.0f;
	gFullscreenVP.TopLeftY	=	0.0f;
	gFullscreenVP.MinDepth	=	0.0f;
	gFullscreenVP.MaxDepth	=	1.0f;
	gFullscreenVP.Width		=	gScreenWidth;
	gFullscreenVP.Height	=	gScreenHeight;
}

void Screen::Initialize()
{
	if( gInitialized )
		return;

	gInitialized	=	Load();
}

void Screen::Destroy()
{
	if(gDestroyed)
		return;

	if(Unload())
		gDestroyed	=	true;
}

ScreenType Screen::GetScreenType()
{
	return UNDEFINED_SCREEN;
}

bool Screen::Load()
{
	return true;
}

bool Screen::Unload()
{
	return true;
}

ScreenType Screen::SwapToThisNextFrame()
{
	//	Save the selected screen
	ScreenType	tScreen	=	gGotoNextFrame;

	//	Null the current screen so it won't
	//	keep flashing between the screens.
	gGotoNextFrame	=	UNDEFINED_SCREEN;

	return tScreen;
}

void Screen::Reset(){}
void Screen::Render(){}
void Screen::Update(float){}



float Screen::GetTextWidth(string Text, float TextSize)
{
	float	tWidth	=	0.0f;

	for(unsigned int i = 0; i < Text.size(); ++i)
	{
		char	tC	=	Text[i];

		if( tC == ' ' )
			tWidth	+=	TextSize * 0.25f;
		else if( tC == '(' || tC == ')' )
			tWidth	+=	TextSize * 0.369f;
		else if( tC == '?' )
			tWidth	+=	TextSize * 0.369f;
		else
			tWidth	+=	isupper(tC) ? TextSize * 0.755f : TextSize * 0.4625f;
	}

	return tWidth;
}

void Screen::DrawString(IFW1FontWrapper& Instance, string Text, float x, float y, float Size, TextColor Color, UINT Flags)
{
	Instance.DrawString(
		gDeviceContext, 
		wstring(Text.begin(), Text.end()).c_str(), 
		floorf(Size * 100 + 0.5f) * 0.01f, 
		floorf(x * 100 + 0.5f) * 0.01f, 
		floorf(y * 100 + 0.5f) * 0.01f, 
		Color, 
		Flags
	);
}

void Screen::DrawString(IFW1FontWrapper& Instance, string Text, float x, float y, float Size, TextColor Color, TextColor BorderColor, float BorderSize, UINT Flags)
{
	DrawString(Instance, Text, x-BorderSize, y, Size, BorderColor, Flags);
	DrawString(Instance, Text, x+BorderSize, y, Size, BorderColor, Flags);
	DrawString(Instance, Text, x, y-BorderSize, Size, BorderColor, Flags);
	DrawString(Instance, Text, x, y+BorderSize, Size, BorderColor, Flags);

	DrawString(Instance, Text, x, y, Size, Color, Flags);
}