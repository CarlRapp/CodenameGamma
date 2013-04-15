#include "InputManager.h"

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


void InputManager::Update()
{
	//	Only update input if the
	//	game is the active window
	if(GetActiveWindow() != *gWindow)
		return;

	//	Update controllers
	for each(Controller* tController in *gControllers)
		tController->Update();

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


/*
InputManager::InputManager(HWND* Window)
{
	inputWindow			=	Window;

	totalKeys			=	255;
	keyStateLastUpdate	=	new bool[totalKeys];
	keyStateThisUpdate	=	new bool[totalKeys];
}

InputManager::~InputManager()
{
	keyStateLastUpdate	=	0;
	keyStateThisUpdate	=	0;
}

void InputManager::Update()
{
	if(GetActiveWindow() != *inputWindow)
	{
		mouseMovement	=	D3DXVECTOR2(0, 0);
		return;
	}

	for(int i = 0; i < totalKeys; ++i)
	{
		keyStateLastUpdate[i]	=	keyStateThisUpdate[i];

		keyStateThisUpdate[i]	=	GetAsyncKeyState(i) ? true : false;
	}

	UpdateMouse();
}

void InputManager::UpdateMouse()
{
	LPRECT Rect = new RECT();
	GetWindowRect(*inputWindow, Rect);

	LPPOINT mousePos = new POINT();
	GetCursorPos(mousePos);

	int centerX		=	(int)((Rect->left + Rect->right) * 0.5f);
	int centerY		=	(int)((Rect->top + Rect->bottom) * 0.5f);
	mouseMovement	=	D3DXVECTOR2((float)(mousePos->x - centerX), (float)(centerY - mousePos->y));
	SetCursorPos(centerX, centerY);
}

D3DXVECTOR2 InputManager::GetMouseMovement()
{
	return mouseMovement;
}

bool InputManager::IsKeyDown(char Key)
{
	return keyStateThisUpdate[Key];
}

bool InputManager::IsKeyPressed(char Key)
{
	return (keyStateThisUpdate[Key] && !keyStateLastUpdate[Key]);
}

bool InputManager::IsKeyReleased(char Key)
{
	return (!keyStateThisUpdate[Key] && keyStateLastUpdate[Key]);
}
*/