#pragma once
#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "../stdafx.h"
#include <fmod.hpp>
#include <fmod_errors.h>
using namespace std;

const	float	DISTANCEFACTOR			=	1.0f;	// Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

enum SoundType
{
	SFX,
	Song,
	Master
};

class SoundManager
{
private:
	static	SoundManager*	gInstance;
	SoundManager(void);

	//	System fields
	FMOD::System	*gSystem;
	FMOD::Channel	*gChannel;
	int				gUniqueIndex;


	//	Fields for loaded sounds
	typedef pair<string, FMOD::Sound*>		SoundEntry;

	typedef	pair<SoundType, FMOD::Channel*>	PSEntry;
	typedef	pair<int, string>				PSIndex;
	typedef	pair<PSIndex, PSEntry>			PlayingSound;

	map <const int, SoundEntry>				gLoadedSounds;
	map <const int, SoundEntry>::iterator	gSoundIterator;
	vector<PlayingSound*>*					gPlayingSounds;

	//	Settings
	float		gMasterVolume;
	float		gEffectVolume;
	float		gMusicVolume;
	float		gTimeToUpdate;
	FMOD_VECTOR	gListenerPosition;


	//	Fields for error handling
	FMOD_RESULT	gResult;
	void	ErrorCheck(FMOD_RESULT Result);
	bool	Exists(string Name);
public:
	static SoundManager* GetInstance();
	~SoundManager(void);

	void	Update(float DeltaTime);
	void	Load(string Name, string Path, FMOD_MODE Flags);

	int	Play(string Name, SoundType Type);
	int	Play(string Name, SoundType Type, bool Loop);
	int	Play3D(string Name, SoundType Type, XMFLOAT3 Position);
	int	Play3D(string Name, SoundType Type, XMFLOAT3 Position, bool Loop);

	bool	IsPlaying(string Name);

	void	Stop(string Name);
	void	Stop(int Index);

	void	SetListenerPosition(float X, float Y, float Z);

	void	SetVolume(SoundType VolumeType, float Value);
	float	GetVolume(SoundType VolumeType);
};

#endif;