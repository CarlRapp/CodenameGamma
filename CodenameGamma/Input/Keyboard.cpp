#include "Keyboard.h"

Keyboard::Keyboard()
{
	gKeyboardTotalKeys	=	256;
	gOldFrameKeyboard	=	new bool[gKeyboardTotalKeys];
	gNewFrameKeyboard	=	new bool[gKeyboardTotalKeys];

}

Keyboard::~Keyboard()
{
	delete	gOldFrameKeyboard;
	gOldFrameKeyboard	=	0;

	delete	gNewFrameKeyboard;
	gNewFrameKeyboard	=	0;
}

void Keyboard::Update()
{
	for(int i = 0; i < gKeyboardTotalKeys; ++i)
	{
		gOldFrameKeyboard[i]	=	gNewFrameKeyboard[i];
		gNewFrameKeyboard[i]	=	GetAsyncKeyState(i) ? true : false;
	}
}


InputState Keyboard::GetKeyState(char Key)
{
	bool	tOld	=	gOldFrameKeyboard[Key];
	bool	tNew	=	gNewFrameKeyboard[Key];

	//	If the button was pressed this frame
	//	but NOT the last frame
	if(!tOld && tNew)
		return	PRESSED;

	//	If the button was pressed last frame
	//	AND this frame
	else if(tOld && tNew)
		return	DOWN;

	//	If the button was pressed last frame
	//	but NOT this frame
	else if(tOld && !tNew)
		return	RELEASED;

	//	If no other state was found the
	//	button must be up.
	return	UP;
}