#include "Rat.h"
#include "../Weapons/RatAttack.h"

Rat::Rat()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Rat" ) );
	SetWeaponState(Hold);

	SetHealth( UnitHealth( 50.0f, 50.0f ) );

	SetScale(2.5f);

	gWalkSpeed = 3.0f;
	gRunSpeed  = 5.0f;

	gDeathTime = 5.0f;

	SetWeapon( new RatAttack() );
}

Rat::~Rat()
{

}

void Rat::Update( float deltaTime, Terrain* terrain )
{
	EnemyUnit::Update( deltaTime, terrain );
	GetWeapon()->Update( deltaTime, terrain );

	if( gTargetPlayer == 0 || GetState() == Dying )
		return;

	float	distance;
	XMStoreFloat(
		&distance,
		XMVector3Length( XMLoadFloat3( &gTargetPlayer->GetFloat3Value( Position ) ) - XMLoadFloat3( &GetFloat3Value( Position ) ) )
	);

	if( distance > 1.5f * UnitsPerMeter )
		return;

	SetVelocity( XMFLOAT3( 0, 0, 0 ) );
	
	if( GetWeapon()->Fire( this, gTargetPlayer, gMultipliers[1] ) )
	{
		PlayAnimation( "Attack" );
	}
}