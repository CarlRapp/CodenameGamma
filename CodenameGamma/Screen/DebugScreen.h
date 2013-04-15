#pragma once
#ifndef DEBUGSCREEN_H
#define DEBUGSCREEN_H

#include "Screen.h"

struct DebugData
{
public:
	DebugData(){}

	string	Title;
	string	Value;

	TextColor	TitleColor;
	TextColor	ValueColor;
};

class DebugScreen : public Screen
{
protected:
	vector<DebugData*>*	gDebugData;

	int					gTextSize;

	bool	Load();
	bool	Unload();

public:
	DebugScreen(ScreenData* Setup);
	DebugData*	AddDebugData(string Title);

	void	Update(float DeltaTime);
	void	Render();
};

#endif