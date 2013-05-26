#include "Wave.h"
#include "../Units/EnemyUnit.h"
#include "../Units//Rat.h"

Wave::Wave()
{
	gWaveTimer			=	WaveTimer( 0.0f, 0.0f );
	gHealthMultiplier	=	1.0f;
	gSpeedMultiplier	=	1.0f;
	gDamageMultiplier	=	1.0f;
}

Wave::Wave(NodeMap* Instance)
{
	gWaveTimer			=	WaveTimer( 0.0f, 5.0f );
	gHealthMultiplier	=	1.0f;
	gSpeedMultiplier	=	1.0f;
	gDamageMultiplier	=	1.0f;
	gNodeMap			=	Instance;
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
	for( int i = 0; i < 5; ++i )
	{
		EnemyUnit*	Unit	=	new Rat();
		PatrolNode*	Node	=	gNodeMap->GetRandomNode();
		XMFLOAT3	Pos		=	XMFLOAT3( Node->Position.x, 0, Node->Position.y );

		Unit->SetNodeMap( gNodeMap );
		Unit->MoveTo( Pos );

		AddGameObject(Unit);
	}

	gWaveTimer.first	=	gWaveTimer.second;
}
#pragma endregion