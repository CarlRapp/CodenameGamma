#include "Wave.h"
#include "../Units/EnemyUnit.h"
#include "../Units/Rat.h"
#include "../Units/Ghost.h"
#include "../Units/Tank.h"

Wave::Wave()
{
	gWaveTimer			=	WaveTimer( 0.0f, 0.0f );
	gHealthMultiplier	=	1.0f;
	gSpeedMultiplier	=	1.0f;
	gDamageMultiplier	=	1.0f;
	gUnitsSpawned		=	WaveUnits( 0, 0 );
	gWaves				=	vector<WaveInfo>();
	gCurrentWave		=	0;
}

Wave::Wave(NodeMap* Instance)
{
	gWaveTimer			=	WaveTimer( 0.0f, 0.0f );
	gHealthMultiplier	=	1.0f;
	gSpeedMultiplier	=	1.0f;
	gDamageMultiplier	=	1.0f;
	gUnitsSpawned		=	WaveUnits( 0, 40 );
	gNodeMap			=	Instance;
	gWaves				=	vector<WaveInfo>();
	gCurrentWave		=	0;
}

void Wave::Update(float DeltaTime)
{
	gWaveTimer.first	-=	DeltaTime;
	if( gWaveTimer.first <= 0.0f )
		NewWave();
}


#pragma region New Wave method
void Wave::NewWave()
{
	if( IsLimitReached() )
	{
		gWaveTimer.first	=	gWaveTimer.second;
		return;
	}

	int correctIndex	=	(gCurrentWave) % ( gWaves.size() );
	WaveInfo	tInfo	=	gWaves[ correctIndex ];

	while( true )
	{
		EnemyUnit*	tUnit;
		bool		tCancel	=	true;

		if( tInfo.Ghosts > 0 && !IsLimitReached() )
		{
			tUnit	=	new Ghost();
			tUnit->SetMultipliers(
				tInfo.GhostInfo[0],
				tInfo.GhostInfo[1],
				tInfo.GhostInfo[2]
			);
			tUnit->Boost( 1 );
			tUnit->SetNodeMap( gNodeMap );
			MoveUnit( tUnit );

			--tInfo.Ghosts;
			++gUnitsSpawned.first;
			AddGameObject( tUnit );

			tCancel	=	false;
		}

		if( tInfo.Tanks > 0 && !IsLimitReached() )
		{
			tUnit	=	new Tank();
			tUnit->SetMultipliers(
				tInfo.TankInfo[0],
				tInfo.TankInfo[1],
				tInfo.TankInfo[2]
			);
			tUnit->Boost( 1 );
			tUnit->SetNodeMap( gNodeMap );
			MoveUnit( tUnit );

			--tInfo.Tanks;
			++gUnitsSpawned.first;
			AddGameObject( tUnit );

			tCancel	=	false;
		}

		if( tInfo.Rats > 0 && !IsLimitReached() )
		{
			tUnit	=	new Rat();
			tUnit->SetMultipliers(
				tInfo.RatInfo[0],
				tInfo.RatInfo[1],
				tInfo.RatInfo[2]
			);
			tUnit->Boost( 1 );
			tUnit->SetNodeMap( gNodeMap );
			MoveUnit( tUnit );

			--tInfo.Rats;
			++gUnitsSpawned.first;
			AddGameObject( tUnit );

			tCancel	=	false;
		}

		if( tCancel )
			break;
	}



	gWaveTimer.first	=	gWaveTimer.second;
	++gCurrentWave;
	int a=2;
}

void Wave::MoveUnit( GameObject* Instance )
{
	XMFLOAT2	tPos	=	gNodeMap->GetRandomNode()->Position;
	Instance->MoveTo( XMFLOAT3( tPos.x, 0, tPos.y ) );
}

#pragma endregion

#pragma region Load Wave
void Wave::LoadWaveData( string LevelPath )
{
	ifstream	tFileStream;

	//	Try and open the file
	tFileStream.open( LevelPath + "WaveData.txt" );

	string	tLine, tToken;
	while(!tFileStream.eof())
	{
		//	Read the current line
		getline(tFileStream, tLine);
		
		//	Read the lines token
		//	and remove it from tLine
		tToken	=	tLine.substr( 0, tLine.find(' ' ) );
		if( tLine.size() > 0 )
			tLine	=	tLine.substr( tToken.size() + 1 );

		if( tToken == "!" )
		{
			gHealthMultiplier	=	atof( tLine.substr( 0, tLine.find( ',' ) ).c_str() );
			tLine	=	tLine.substr( tLine.find( ',' ) + 1 );

			gDamageMultiplier	=	atof( tLine.substr( 0, tLine.find( ',' ) ).c_str() );
			tLine	=	tLine.substr( tLine.find( ',' ) + 1 );

			gSpeedMultiplier	=	atof( tLine.substr( 0, tLine.find( ',' ) ).c_str() );
		}
		else if( tToken == "#" )
			gWaves.push_back( ReadWaveLine( tLine ) );
		else if( tToken == "WaveTimer" )
		{
			gWaveTimer.second	=	atoi( tLine.c_str() );
			gWaveTimer.first	=	gWaveTimer.second;
		}
	}


	tFileStream.close();
}



WaveInfo Wave::ReadWaveLine( string Line )
{
	//	The final wave info
	WaveInfo	Result	=	WaveInfo();

	//	Rat(10, 1.0, 2.0, 3.0) Tank(2, 1.0, 1.0, 1.0) Ghost(2, 1.0, 2.0, 3.0)
	string	tLine, tUnitName;
	int		tAmount;
	float	tMultipliers[3];

	while(true)
	{
		//	Get the first chunk of unit info
		tLine		=	Line.substr( 0, Line.find( ')' ) + 1 );
		//	Remove it from the original line
		Line	=	Line.substr( tLine.length() );

		//	Get the units name
		tUnitName	=	tLine.substr( 0, tLine.find( '(' ) );
		if( tUnitName[0] == ' ' )
			tUnitName	=	tUnitName.substr( 1 );

		//	Get the info from within the ( BLA BLA )
		tLine	=	tLine.substr( tLine.find( '(' ) + 1 );
		tLine	=	tLine.substr( 0, tLine.find( ')' ) );

		//	Number of Units
		tAmount	=	atoi( tLine.substr( 0, tLine.find( ',' ) ).c_str() );
		tLine	=	tLine.substr( tLine.find( ',' ) + 1 );

		//	Health
		tMultipliers[0]	=	atof( tLine.substr( 0, tLine.find( ',' ) ).c_str() );
		tLine	=	tLine.substr( tLine.find( ',' ) + 1 );

		//	Damage
		tMultipliers[1]	=	atof( tLine.substr( 0, tLine.find( ',' ) ).c_str() );
		tLine	=	tLine.substr( tLine.find( ',' ) + 1 );

		//	Speed
		tMultipliers[2]	=	atof( tLine.substr( 0, tLine.find( ',' ) ).c_str() );

		if( tUnitName == "Rat" )
		{
			Result.Rats		=	tAmount;
			for( int i = 0; i < 3; ++i )
				Result.RatInfo[i]	=	tMultipliers[i];
		}
		else if( tUnitName == "Tank" )
		{
			Result.Tanks		=	tAmount;
			for( int i = 0; i < 3; ++i )
				Result.TankInfo[i]	=	tMultipliers[i];
		}
		else if( tUnitName == "Ghost" )
		{
			Result.Ghosts		=	tAmount;
			for( int i = 0; i < 3; ++i )
				Result.GhostInfo[i]	=	tMultipliers[i];
		}

		if( Line.length() == 0 )
			break;
	}

	return Result;
}
#pragma endregion