#pragma once
#ifndef OPTIONSCREEN_H
#define OPTIONSCREEN_H

#include "Screen.h"
using namespace std;

class OptionScreen : public Screen
{
private:
	enum	OptionType
	{
		Resolution,
		Volume,
		Checkbox
	};
	typedef pair<OptionType, int>	EntryData;
	typedef pair<string, EntryData>	OptionEntry;
	vector<OptionEntry>			gMenu;
	int	gMenuIndex;

	typedef pair<int, int>	ResolutionEntry;
	ResolutionEntry			gResolutions[2];

	ID3D11ShaderResourceView*	gBackground;

	void	Confirm(void);
	void	UpdateVolume( SoundType VolumeType, int Value );

	bool	Load();
	bool	Unload();
public:
	OptionScreen(ScreenData* Setup);

	void	Update(float DeltaTime);
	void	Render();

	ScreenType	GetScreenType();
};
#endif