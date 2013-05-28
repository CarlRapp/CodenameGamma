#include "ScreenManager.h"
#include "PrePlayScreen.h"
#include "PostPlayScreen.h"
#include "LevelSelectScreen.h"
#include "OptionScreen.h"

ScreenManager::ScreenManager()
{
	gLoadedScreens	=	new vector<Screen*>();
	gCurrentScreen	=	NULL;
}
ScreenManager::ScreenManager(ScreenData* Setup)
{
	gScreenData	=	Setup;

	gLoadedScreens	=	new vector<Screen*>();
	gCurrentScreen	=	NULL;

	

	gExit	=	false;
	gShowDebug	=	false;

	gFpsCounter	=	DebugScreen::GetInstance()->AddDebugData("");
	gFpsCounter->Value	=	"FPS";
	gFpsCounter->ValueColor	=	Green;
	gFpsCounter->TitleColor	=	White;

	gViewPort	=	D3D11_VIEWPORT();
	gViewPort.TopLeftX	=	0;
	gViewPort.TopLeftY	=	0;
	gViewPort.MinDepth	=	0.0f;
	gViewPort.MaxDepth	=	1.0f;
	gViewPort.Width		=	Setup->WIDTH;
	gViewPort.Height	=	Setup->HEIGHT;
	

	DebugScreen::GetInstance()->AddLogMessage("Screen Manager: Initialized!", Green);
}

void ScreenManager::Update(float DeltaTime)
{
	DeltaTime	=	min(1.0f, DeltaTime);

	gFpsCounter->Title	=	to_string((long double)((int)(1/DeltaTime)));
	SoundManager::GetInstance()->Update(DeltaTime);
	InputManager::GetInstance()->Update();

	if ( InputManager::GetInstance()->GetKeyboard()->GetKeyState(VK_F1) == PRESSED )
		ToggleDebug();

	if ( gShowDebug )
		DebugScreen::GetInstance()->Update(DeltaTime);


	if( !gCurrentScreen )
		return;


	gCurrentScreen->Update(DeltaTime);

	ChangeScreen(gCurrentScreen->SwapToThisNextFrame());
}

void ScreenManager::Render()
{
	if(	!gCurrentScreen )
		return;
	float ClearColor[4] = {0.1f,  0.1f, 0.1f, 0.0f};
	gScreenData->DEVICE_CONTEXT->ClearRenderTargetView( gScreenData->RENDER_TARGET_VIEW, ClearColor );

	gScreenData->DEVICE_CONTEXT->RSSetViewports( 1, &gViewPort );
	gCurrentScreen->Render();

	if( gShowDebug )
	{
		gScreenData->DEVICE_CONTEXT->RSSetViewports( 1, &gViewPort );
		DebugScreen::GetInstance()->Render();
	}
}

bool ScreenManager::CloseGame()
{
	return gExit;
}


void ScreenManager::ChangeScreen(ScreenType Type)
{
	//	Abort invalid screens
	if(Type == UNDEFINED_SCREEN)
		return;

	if(Type == EXIT_SCREEN)
	{
		gExit	=	true;
		return;
	}

	bool	isLoaded	=	false;

	//	Check if the selected screen is loaded
	for each(Screen* tScreen in *gLoadedScreens)
	{
		//Type;
		//tScreen->GetScreenType();

		if(tScreen->GetScreenType() == Type)
		{
			isLoaded		=	true;
			gCurrentScreen	=	tScreen;
		}
	}

	//	If the screen is not
	//	loaded, load it
	if(!isLoaded)
	{
		//	Call the creation 
		//	of the screen
		Screen*	tScreen	=	LoadScreen(Type);

		//	Abort if the screen
		//	is a non-valid screen
		if( tScreen == NULL)
			return;

		tScreen->Initialize();
		gLoadedScreens->push_back(tScreen);

		gCurrentScreen	=	tScreen;
	}

	//	Call Reset() to
	//	let the screen know
	//	it's been selected
	else
		gCurrentScreen->Reset();
}

Screen* ScreenManager::LoadScreen(ScreenType Type)
{
	if(Type == UNDEFINED_SCREEN)
		return NULL;

	//	Main Menu
	else if(Type == MAIN_MENU_SCREEN)
		return	new MainMenuScreen(gScreenData);

	//	Options
	else if(Type == OPTIONS_SCREEN)
		return	new OptionScreen(gScreenData);

	//	Play Screen
	else if(Type == PLAY_SCREEN)
		return	new PlayScreen(gScreenData);

	//	Credits
	else if(Type == CREDITS_SCREEN)
		return	NULL;

	else if(Type == PRE_PLAY_SCREEN)
		return	new PrePlayScreen(gScreenData);
	else if(Type == POST_PLAY_SCREEN)
		return	new PostPlayScreen(gScreenData);
	else if( Type == LEVEL_SELECT_SCREEN )
		return	new LevelSelectScreen( gScreenData );

	//	When this is called it means
	//	there is a screen type that not
	//	yet have been added
	return NULL;
}

ScreenManager::~ScreenManager()
{
	//	Unload all screens
	for(int i = 0; i < (int)gLoadedScreens->size(); ++i)
	{
		gLoadedScreens->at(i)->Destroy();
		gLoadedScreens->at(i)	=	0;
	}
	gCurrentScreen	=	0;
	gLoadedScreens->clear();
}

void ScreenManager::ToggleDebug()
{
	gShowDebug	=	!gShowDebug;

	if ( !DebugScreen::GetInstance() )
		gShowDebug	=	false;
}