#include "Screen.h"

Screen::Screen(ScreenData*	Setup)
{
	gInitialized	=	false;
	gDestroyed		=	false;

	LoadScreenData(Setup);
	gViewPort	=	D3D11_VIEWPORT();
	gViewPort.TopLeftX	=	0;
	gViewPort.TopLeftY	=	0;
	gViewPort.MinDepth	=	0.0f;
	gViewPort.MaxDepth	=	1.0f;
	gViewPort.Width		=	Setup->WIDTH;
	gViewPort.Height	=	Setup->HEIGHT;
}
Screen::Screen(void)
{
	gInitialized	=	false;
	gDestroyed		=	false;
}

void Screen::LoadScreenData(ScreenData* Setup)
{
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
}

void Screen::Initialize()
{
	if(gInitialized)
		return;

	if(Load())
		gInitialized	=	true;
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

void Screen::PreRenderSettings()
{
	gDeviceContext->RSSetViewports( 1, &gViewPort );
}