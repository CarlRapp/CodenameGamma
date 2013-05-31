#include "Ghost.h"
#include "../Weapons/GhostAttack.h"

Ghost::Ghost()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Ghost" ) );
	SetWeaponState(Hold);

	gWalkSpeed = 3.0f;
	gRunSpeed  = 5.0f;

	gDeathTime = 5.0f;

	gCurrentWeapon	=	new GhostAttack();
	gPrimaryWeapon	=	gCurrentWeapon;
}

Ghost::~Ghost()
{

}

void Ghost::Update( float deltaTime, Terrain* terrain )
{
	EnemyUnit::Update( deltaTime, terrain );
	GetWeapon()->Update( deltaTime, terrain );

	if (!GetWeapon()->GotCallbackFunctions())
		SetCallbackFunctions(GetWeapon());

	if (!AddGameObject)
		Kill();

	if( gTargetPlayer == 0 || GetState() == Dying )
		return;

	float	distance;
	XMStoreFloat(
		&distance,
		XMVector3Length( XMLoadFloat3( &gTargetPlayer->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	if( distance > 5.0f * UnitsPerMeter )
		return;

	SetVelocity( XMFLOAT3( 0, 0, 0 ) );
	
	if( GetWeapon()->Fire( this, gTargetPlayer, 0 ) )
	{
		PlayAnimation( "Attack" );
	}
}