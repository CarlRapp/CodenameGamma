#include "SoundManager.h"
#include "../Screen/ScreenEnums.h"

SoundManager* SoundManager::gInstance	=	0;
SoundManager* SoundManager::GetInstance()
{
	if( !gInstance )
		gInstance	=	new SoundManager();

	return gInstance;
}

SoundManager::SoundManager()
{
	gTimeToUpdate	=	0.0f;
	gMasterVolume	=	1.0f;
	gEffectVolume	=	1.0f;
	gMusicVolume	=	1.0f;

	gChannel	=	0;

	unsigned int		tVersion;
	int					nDrivers;
	FMOD_SPEAKERMODE	tSpeakerMode;
	FMOD_CAPS			tCaps;

	gListenerPosition.x	=	0;
	gListenerPosition.y	=	0;
	gListenerPosition.z	=	0;
	gUniqueIndex		=	0;

	/*
	Create a System object and initialize.
	*/
	gResult = FMOD::System_Create(&gSystem);
	ErrorCheck(gResult);
    
	gResult = gSystem->getVersion(&tVersion);
	ErrorCheck(gResult);

	if (tVersion < FMOD_VERSION)
	{
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", tVersion, FMOD_VERSION);
	}
    
	gResult = gSystem->getNumDrivers(&nDrivers);
	ErrorCheck(gResult);

	if (nDrivers == 0)
	{
		gResult = gSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ErrorCheck(gResult);
	}
	else
	{
		gResult = gSystem->getDriverCaps(0, &tCaps, 0, &tSpeakerMode);
		ErrorCheck(gResult);

		gResult = gSystem->setSpeakerMode(tSpeakerMode);       /* Set the user selected speaker mode. */
		ErrorCheck(gResult);

		if (tCaps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
		{                                                   /* You might want to warn the user about this. */
			gResult = gSystem->setDSPBufferSize(1024, 10);
			ErrorCheck(gResult);
		}

		char tName[256];
		gResult = gSystem->getDriverInfo(0, tName, 256, 0);
		ErrorCheck(gResult);

		if (strstr(tName, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
		{
			gResult = gSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			ErrorCheck(gResult);
		}
	}

	gResult = gSystem->init(100, FMOD_INIT_NORMAL, 0);
	if (gResult == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
	{
		gResult = gSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ErrorCheck(gResult);
            
		gResult = gSystem->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
		ErrorCheck(gResult);
	}
	/*
		Set the distance units. (meters/feet etc).
	*/
	gResult = gSystem->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
	ErrorCheck(gResult);

	gPlayingSounds	=	new vector<PlayingSound*>();

	DebugScreen::GetInstance()->AddLogMessage("Sound Manager: Initialized!", Green);
}

SoundManager::~SoundManager()
{
}

bool SoundManager::Exists(string Name)
{
	//	Check if the Sound is
	//	already loaded
	for ( gSoundIterator = gLoadedSounds.begin(); gSoundIterator != gLoadedSounds.end(); ++gSoundIterator)
		if(Name == gSoundIterator->second.first)
			return true;

	return false;
}


void SoundManager::Load(string Name, string Path, FMOD_MODE Flags)
{
	if(Exists(Name))
	{
		DebugScreen::GetInstance()->AddLogMessage("Sound: \"" + Name + "\" is already loaded.", Red);
		return;
	}

	FMOD::Sound*	tSound;

	gResult = gSystem->createSound(Path.c_str(), Flags, 0, &tSound);
	ErrorCheck(gResult);

	gLoadedSounds[gLoadedSounds.size()]	=	SoundEntry(Name, tSound);
}

int SoundManager::Play(string Name, SoundType Type)
{
	return Play(Name, Type, false);
}

int SoundManager::Play(string Name, SoundType Type, bool Loop)
{
	if(!Exists(Name))
	{
		DebugScreen::GetInstance()->AddLogMessage("Sound: \"" + Name + "\" does not exist.", Red);
		return -1;
	}

	if(Loop)
		gSoundIterator->second.second->setMode(FMOD_LOOP_NORMAL);

	gResult = gSystem->playSound(FMOD_CHANNEL_FREE, gSoundIterator->second.second, true, &gChannel);
	ErrorCheck(gResult);
	gResult = gChannel->setPaused(false);
	ErrorCheck(gResult);

	switch(Type)
	{
	case Song:
		gChannel->setVolume( gMusicVolume * gMasterVolume );
		break;

	case SFX:
		gChannel->setVolume( gEffectVolume * gMasterVolume );
		break;
	}

	
	gPlayingSounds->push_back(new PlayingSound(PSIndex( gUniqueIndex, Name ), PSEntry( Type, gChannel )));
	++gUniqueIndex;

	return gUniqueIndex - 1;
}

int SoundManager::Play3D(string Name, SoundType Type, XMFLOAT3 Position)
{
	return Play3D(Name, Type, Position, false);
}


int SoundManager::Play3D(string Name, SoundType Type, XMFLOAT3 Position, bool Loop)
{
	if(!Exists(Name))
	{
		DebugScreen::GetInstance()->AddLogMessage("Sound: \"" + Name + "\" does not exist.", Red);
		return -1;
	}

	if(Loop)
		gSoundIterator->second.second->setMode(FMOD_LOOP_NORMAL);

	ErrorCheck(gSystem->playSound(FMOD_CHANNEL_FREE, gSoundIterator->second.second, true, &gChannel));

	ErrorCheck(gChannel->setPaused(false));

	switch(Type)
	{
	case Song:
		gChannel->setVolume( gMusicVolume * gMasterVolume );
		break;

	case SFX:
		gChannel->setVolume( gEffectVolume * gMasterVolume );
		break;
	}

	FMOD_VECTOR	tVelocity	=	{0, 0, 0};
	gResult	=	gChannel->set3DAttributes(&gListenerPosition, &tVelocity);
	ErrorCheck(gResult);

	gPlayingSounds->push_back(new PlayingSound(PSIndex( gUniqueIndex, Name ), PSEntry( Type, gChannel )));
	++gUniqueIndex;

	return gUniqueIndex - 1;
}

bool SoundManager::IsPlaying(string Name)
{
	for(int i = gPlayingSounds->size() - 1; i >= 0; --i)
	{
		PlayingSound*	tSound	=	gPlayingSounds->at(i);
		bool	tPlaying	=	false;
		if ( tSound->first.second == Name)
		{
			PSEntry	tEntry	=	tSound->second;
			ErrorCheck( tEntry.second->isPlaying( &tPlaying ) );

			if(tPlaying)
			{
				return true;
			}
		}
	}
	return false;
}

void SoundManager::Stop(string Name)
{
	if(!Exists(Name))
	{
		DebugScreen::GetInstance()->AddLogMessage("Sound: \"" + Name + "\" does not exist.", Red);
		return;
	}
	
	for(int i = gPlayingSounds->size() - 1; i >= 0; --i)
	{
		PlayingSound*	tSound	=	gPlayingSounds->at(i);
		bool	tPlaying	=	false;
		if ( tSound->first.second == Name)
		{
			PSEntry	tEntry	=	tSound->second;
			ErrorCheck( tEntry.second->isPlaying( &tPlaying ) );

			if(tPlaying)
			{
				gPlayingSounds->erase(gPlayingSounds->begin() + i);
				tEntry.second->stop();
				delete tSound;
				return;
			}
			else
			{
				gPlayingSounds->erase(gPlayingSounds->begin() + i);
			}
		}
	}

	DebugScreen::GetInstance()->AddLogMessage("Sound: Trying to stop \"" + Name + "\", but it is not playing.", Red);
}

void SoundManager::Stop(int Index)
{
	for(int i = gPlayingSounds->size() - 1; i >= 0; --i)
	{
		PlayingSound*	tSound	=	gPlayingSounds->at(i);
		bool	tPlaying	=	false;
		if ( tSound->first.first == Index)
		{
			PSEntry	tEntry	=	tSound->second;
			ErrorCheck( tEntry.second->isPlaying( &tPlaying ) );

			if(tPlaying)
			{
				gPlayingSounds->erase(gPlayingSounds->begin() + i);
				tEntry.second->stop();
				delete tSound;
				return;
			}
			else
			{
				gPlayingSounds->erase(gPlayingSounds->begin() + i);
			}
		}
	}

	DebugScreen::GetInstance()->AddLogMessage("Sound: Trying to stop index " + to_string((long double)Index) + ", but it is not playing.", Red);
}




void SoundManager::ErrorCheck(FMOD_RESULT Result)
{
	//	Implement write to debug screen here.
	if ( Result != FMOD_OK && Result != FMOD_ERR_INVALID_HANDLE && Result != FMOD_ERR_CHANNEL_STOLEN )
	{
		DebugScreen::GetInstance()->AddLogMessage("Sound Error: " + (string)FMOD_ErrorString(Result), Red);
	}
}





void SoundManager::Update(float DeltaTime)
{
	gTimeToUpdate	+=	DeltaTime;


	if(gTimeToUpdate > 0.1f)
	{
		gSystem->update();
		unsigned int ms = 0;
		unsigned int lenms = 0;
		bool         playing = false;
		bool         paused = false;
		int          channelsplaying = 0;

		if (gChannel)
		{
			FMOD::Sound *currentsound = 0;

			ErrorCheck(gChannel->isPlaying(&playing));
			ErrorCheck(gChannel->getPaused(&paused));
			ErrorCheck(gChannel->getPosition(&ms, FMOD_TIMEUNIT_MS));
               
			gChannel->getCurrentSound(&currentsound);

			if (currentsound)
			{
				ErrorCheck(currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS));
			}
		}
		
		for(int i = gPlayingSounds->size() - 1; i >= 0; --i)
		{
			PlayingSound*	tSound	=	gPlayingSounds->at(i);
			ErrorCheck( tSound->second.second->isPlaying( &playing ) );

			if(!playing)
				gPlayingSounds->erase(gPlayingSounds->begin() + i);
		}
		
		gTimeToUpdate	=	0;
	}
}

void SoundManager::SetListenerPosition(float X, float Y, float Z)
{
	gListenerPosition.x	=	X;
	gListenerPosition.y	=	Y;
	gListenerPosition.z	=	Z;
}

void SoundManager::SetVolume(SoundType VolumeType, float Value)
{
	Value	=	MathHelper::Clamp(Value, 0.0f, 1.0f);

	switch(VolumeType)
	{
	case SFX:
		gEffectVolume	=	Value;
		break;

	case Song:
		gMusicVolume	=	Value;
		break;

	case Master:
		gMasterVolume	=	Value;
		break;
	}

	for( int i = 0; i < gPlayingSounds->size(); ++i )
	{
		PlayingSound*	tSound	=	gPlayingSounds->at( i );
		PSEntry			tEntry	=	tSound->second;

		if( tEntry.first == VolumeType || VolumeType == Master )
		{
			float	newVolume	=	gMasterVolume;
			if( VolumeType == SFX )
				newVolume	*=	gEffectVolume;
			else if( VolumeType == Song )
				newVolume	*=	gMusicVolume;

			tEntry.second->setVolume( newVolume );
		}
	}
}

float SoundManager::GetVolume(SoundType VolumeType)
{
	switch(VolumeType)
	{
	case SFX:
		return gEffectVolume;
		break;

	case Song:
		return gMusicVolume;
		break;

	case Master:
		return gMasterVolume;
		break;
	}

	return -1.0f;
}