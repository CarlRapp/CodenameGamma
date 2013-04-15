#include "ScreenManager.h"

ScreenManager::ScreenManager()
{
	gLoadedScreens	=	new vector<Screen*>();
	gCurrentScreen	=	NULL;
}
DebugData*	TEST, *TEST2, *TEST3;
int	scroll = 0;
ScreenManager::ScreenManager(ScreenData* Setup)
{
	gScreenData	=	Setup;

	gLoadedScreens	=	new vector<Screen*>();
	gCurrentScreen	=	NULL;

	gDebugScreen	=	new DebugScreen(Setup);
	gDebugScreen->Initialize();
	TEST	=	gDebugScreen->AddDebugData("");
	TEST->Value	=	"    FPS (SUCKS ASS IN DEBUG SO PLZ NO HATE)";
	TEST->ValueColor	=	Red;

	TEST2	=	gDebugScreen->AddDebugData("Mouse Position");

	TEST3	=	gDebugScreen->AddDebugData("Left M3 Right M4 (Scroll) -> (Total Value)");

	gExit	=	false;
	gShowDebug	=	false;
}

void ScreenManager::Update(float DeltaTime)
{
	SoundManager::GetInstance()->Update(DeltaTime);
	InputManager::GetInstance()->Update();

	if( InputManager::GetInstance()->GetKeyboard()->GetKeyState(VK_F1) == PRESSED)
		ToggleDebug();

	if( !gCurrentScreen )
		return;

	gCurrentScreen->Update(DeltaTime);

	TEST->Title			=	to_string((long double)((int)(1/DeltaTime)));


	XMFLOAT2	tVec	=	InputManager::GetInstance()->GetMouse()->GetPosition(true);
	TEST2->Value	=	to_string((long double)tVec.x) + ", " + to_string((long double)tVec.y);


	TEST3->Value	=	to_string((long double)InputManager::GetInstance()->GetMouse()->GetButtonState(M_LEFT)) + " | ";
	TEST3->Value	+=	to_string((long double)InputManager::GetInstance()->GetMouse()->GetButtonState(M_MIDDLE)) + " | ";
	TEST3->Value	+=	to_string((long double)InputManager::GetInstance()->GetMouse()->GetButtonState(M_RIGHT)) + " | ";
	TEST3->Value	+=	to_string((long double)InputManager::GetInstance()->GetMouse()->GetButtonState(M_MOUSE4)) + " (";
	TEST3->Value	+=	to_string((long double)InputManager::GetInstance()->GetMouse()->GetScrollValue()) + ") -> ";
	scroll	+=	InputManager::GetInstance()->GetMouse()->GetScrollValue();

	TEST3->Value	+=	to_string((long double)scroll);

	ChangeScreen(gCurrentScreen->SwapToThisNextFrame());
}

void ScreenManager::Render()
{
	if(	!gCurrentScreen )
		return;

	float ClearColor[4] = {0.1f,  0.1f, 0.1f, 0.0f};
	gScreenData->DEVICE_CONTEXT->ClearRenderTargetView( gScreenData->RENDER_TARGET_VIEW, ClearColor );

	gCurrentScreen->Render();

	if( gShowDebug )
		gDebugScreen->Render();
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
		Type;
		tScreen->GetScreenType();

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
		return	NULL;

	//	Play Screen
	else if(Type == PLAY_SCREEN)
		return	NULL;

	//	Credits
	else if(Type == CREDITS_SCREEN)
		return	NULL;

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
}