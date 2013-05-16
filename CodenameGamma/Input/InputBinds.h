#pragma once
#ifndef INPUTBINDS_H
#define INPUTBINDS_H

#include "InputEnums.h"


enum XboxBinds
{
	XShoot	=	RIGHT_TRIGGER,
	XAim	=	LEFT_TRIGGER,
	XDrop	=	B,
	XReload	=	X
};

enum KeyboardBinds
{
	KWalkUp		=	'W',
	KWalkLeft	=	'A',
	KWalkDown	=	'S',
	KWalkRight	=	'D',

	KReload	=	'R',
	KDrop	=	'G',
	KRun	=	VK_SHIFT

};

enum MouseBinds
{
	MShoot	=	M_LEFT,
	MLook	=	M_RIGHT
};

#endif