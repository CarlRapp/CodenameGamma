#include "WeaponOnGround.h"


WeaponOnGround::WeaponOnGround()
{
	SetState( Dead );
}

WeaponOnGround::WeaponOnGround( Weapon* Instance )
{
	//	Null the light pointer
	gPointLight	=	0;

	//	Set the initial values on offset, cooldown and
	//	a random "life span" that will give it an offset
	//	in position methods, making every new drop look
	//	unique
	gCooldown	=	0.5f;
	gOffset		=	XMFLOAT3( 0, 3, 0 );
	gTimeSpan	=	MathHelper::RandF( 0.0f, 8000.0f );

	//	Set the current modelinstance
	//	and scale it to look better
	SetModelInstance( Instance->GetModelInstance() );
	SetScale( 2 );

	//	Update the instance
	//	to hidden so it will
	//	be removed from the tree.
	gWeapon	=	Instance;
	gWeapon->SetState( Hidden );


	//	Give it a random rotation
	SetRotation( XMFLOAT3( 0, MathHelper::RandF( -PI, PI ), 0 ) );
}

WeaponOnGround::~WeaponOnGround()
{
	delete	gPointLight;
}

void WeaponOnGround::Update(float DeltaTime, Terrain* terrain)
{
	if ( !IsAlive() )
		return;

	XMFLOAT3	newPos	=	GetFloat3Value( Position );

	if ( !gPointLight )
	{
		gPointLight	=	new PointLight();
		gPointLight->GetGPULight()->Color		=	XMFLOAT4( 2.0f, 1.0f, 1.0f, 0.0f );
		gPointLight->GetGPULight()->Position	=	newPos;
		gPointLight->GetGPULight()->Range		=	33.333f * 0.40f;
		gPointLight->GetGPULight()->HasShadow	=	false;

		AddLight( gPointLight );
	}

	gTimeSpan	+=	DeltaTime;
	
	newPos.y	=	gOffset.y + ( gOffset.y - 2 ) * sin( 8 * gTimeSpan );

	gWeapon->MoveTo( newPos );
	MoveTo( newPos );
	AddRotation( XMFLOAT3( 0, DeltaTime, 0 ) );

	newPos.y	-=	1.0f;

	if ( gPointLight != 0 )
		gPointLight->GetGPULight()->Position	=	newPos;

	if ( gCooldown > 0 )
	{
		gCooldown	-=	DeltaTime;
		gCooldown	=	( gCooldown < 0 ) ? 0 : gCooldown;
	}
}


void WeaponOnGround::OnPickUp(Unit* Instance)
{
	if ( gCooldown != 0 )
		return;

	if ( !IsOfType<PlayerUnit>(Instance) )
		return;

	
	PlayerUnit*	pUnit	=	(PlayerUnit*)Instance;
	if (!pUnit->HasWeapon())
	{
		SetScale( 1 );
		pUnit->SetWeapon( gWeapon );
	
		SetState( Dead );
		RemoveLight( gPointLight );

		gWeapon->SetState( Alive );
		AddGameObject( gWeapon );
	}
}