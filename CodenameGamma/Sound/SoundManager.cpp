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
		return;

	FMOD::Sound*	tSound;

	gResult = gSystem->createSound(Path.c_str(), Flags, 0, &tSound);
	ErrorCheck(gResult);

	gLoadedSounds[gLoadedSounds.size()]	=	SoundEntry(Name, tSound);
}

void SoundManager::Play(string Name)
{
	Play(Name, false);
}


void SoundManager::Play(string Name, bool Loop)
{
	if(!Exists(Name))
	{
		DebugScreen::GetInstance()->AddLogMessage("Sound: \"" + Name + "\" does not exist.", Red);
		return;
	}
	if(Loop)
		gSoundIterator->second.second->setMode(FMOD_LOOP_NORMAL);

	gResult = gSystem->playSound(FMOD_CHANNEL_FREE, gSoundIterator->second.second, true, &gChannel);
	ErrorCheck(gResult);
	gResult = gChannel->setPaused(false);
	ErrorCheck(gResult);
	gChannel->setVolume(gMasterVolume);

	
}

void SoundManager::Play3D(string Name, XMFLOAT3 Position)
{
	Play3D(Name, Position, false);
}


void SoundManager::Play3D(string Name, XMFLOAT3 Position, bool Loop)
{
	if(!Exists(Name))
	{
		DebugScreen::GetInstance()->AddLogMessage("Sound: \"" + Name + "\" does not exist.", Red);
		return;
	}
	if(Loop)
		gSoundIterator->second.second->setMode(FMOD_LOOP_NORMAL);

	ErrorCheck(gSystem->playSound(FMOD_CHANNEL_FREE, gSoundIterator->second.second, true, &gChannel));

	ErrorCheck(gChannel->setPaused(false));

	ErrorCheck(gChannel->setVolume(gMasterVolume));

	FMOD_VECTOR	tVelocity	=	{0, 0, 0};
	gResult	=	gChannel->set3DAttributes(&gListenerPosition, &tVelocity);
	ErrorCheck(gResult);
	
}







void SoundManager::ErrorCheck(FMOD_RESULT Result)
{
	//	Implement write to debug screen here.
	if (Result != FMOD_OK )
		DebugScreen::GetInstance()->AddLogMessage("Sound Error: " + Result, Red);
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

                gResult = gChannel->isPlaying(&playing);
                if ((gResult != FMOD_OK) && (gResult != FMOD_ERR_INVALID_HANDLE) && (gResult != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ErrorCheck(gResult);
                }

                gResult = gChannel->getPaused(&paused);
                if ((gResult != FMOD_OK) && (gResult != FMOD_ERR_INVALID_HANDLE) && (gResult != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ErrorCheck(gResult);
                }

                gResult = gChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
                if ((gResult != FMOD_OK) && (gResult != FMOD_ERR_INVALID_HANDLE) && (gResult != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ErrorCheck(gResult);
                }
               
                gChannel->getCurrentSound(&currentsound);
                if (currentsound)
                {
                    gResult = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
                    if ((gResult != FMOD_OK) && (gResult != FMOD_ERR_INVALID_HANDLE) && (gResult != FMOD_ERR_CHANNEL_STOLEN))
                    {
                        ErrorCheck(gResult);
                    }
                }
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