#include "InputManager.h"
#include "../Screen/ScreenEnums.h"

InputManager* InputManager::gInstance	=	0;
InputManager* InputManager::GetInstance()
{
	return gInstance;
}

void InputManager::Initialize(HINSTANCE* HInstance, HWND* Hwnd, int ScreenWidth, int ScreenHeight)
{
	//	If the manager is already
	//	initialized, abort the request
	if( gInstance )
		return;

	//	Create the instance
	gInstance	=	new InputManager(HInstance, Hwnd, ScreenWidth, ScreenHeight);
}
InputManager::InputManager(){}
InputManager::InputManager(HINSTANCE* HInstance, HWND* Hwnd, int ScreenWidth, int ScreenHeight)
{
	gControllers	=	new vector<Controller*>();

	gControllers->push_back(new Controller(0));
	gControllers->push_back(new Controller(1));
	gControllers->push_back(new Controller(2));
	gControllers->push_back(new Controller(3));

	gKeyboard	=	new Keyboard();
	gMouse		=	new Mouse(HInstance, Hwnd, ScreenWidth, ScreenHeight);

	gWindow	=	Hwnd;

	DebugScreen::GetInstance()->AddLogMessage("Input Manager: Initialized!", Green);
}

InputManager::~InputManager()
{
	for each(Controller* tController in *gControllers)
		delete	tController;
	gControllers->clear();
	delete	gControllers;

	if( gKeyboard )
		delete gKeyboard;
	if( gMouse )
		delete gMouse;
}


void InputManager::Update(float DeltaTime)
{
	//	Only update input if the
	//	game is the active window
	if(GetActiveWindow() != *gWindow)
		return;

	//	Update controllers
	for each(Controller* tController in *gControllers)
		tController->Update( DeltaTime );

	//	Update keyboard input
	gKeyboard->Update();

	//	Update the mouse
	gMouse->Update();
}

Controller* InputManager::GetController(int Index)
{
	if(Index < 0 || Index >= gControllers->size())
		return NULL;

	return gControllers->at(Index);
}

Keyboard* InputManager::GetKeyboard()
{
	return gKeyboard;
}
Mouse* InputManager::GetMouse()
{
	return gMouse;
}