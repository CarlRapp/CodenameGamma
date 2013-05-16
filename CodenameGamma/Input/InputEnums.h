#pragma once
#ifndef INPUTENUMS
#define INPUTENUMS

#include "../stdafx.h"

enum Xbox_Button
{
	A	=	0, 
	X	=	1, 
	Y	=	2, 
	B	=	3,

	D_UP	=	4, 
	D_DOWN	=	5, 
	D_LEFT	=	6, 
	D_RIGHT	=	7,

	LEFT_STICK	=	8, 
	RIGHT_STICK	=	9,

	START	=	10, 
	BACK	=	11,

	LEFT_BUMPER		=	12, 
	RIGHT_BUMPER	=	13,

	LEFT_TRIGGER	=	14,
	RIGHT_TRIGGER	=	15,

	NUMBER_OF_BUTTONS
};
enum Xbox_Direction
{
	LEFT, RIGHT
};

enum InputState
{
	UP, PRESSED, DOWN, RELEASED
};

enum MouseButton
{
	M_LEFT,M_RIGHT, M_MIDDLE, M_MOUSE4
};

#endif