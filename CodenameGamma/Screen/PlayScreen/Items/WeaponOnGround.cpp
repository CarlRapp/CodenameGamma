#include "WeaponOnGround.h"


WeaponOnGround::WeaponOnGround()
{
	gWeapon		=	0;
	gPointLight	=	0;
	gOffset		=	XMFLOAT3( 0, 3, 0 );
	gTimeSpan	=	MathHelper::RandF( 0.0f, 8000.0f );

	SetRotation( XMFLOAT3( 0, MathHelper::RandF( -PI, PI ), 0 ) );
}

WeaponOnGround::WeaponOnGround( Weapon* Instance )
{
	SetModelInstance( Instance->GetModelInstance() );
	SetScale( 2 );
	gWeapon	=	Instance;
	gWeapon->SetState( Hidden );


	gPointLight	=	0;
	gOffset		=	XMFLOAT3( 0, 3, 0 );
	gTimeSpan	=	MathHelper::RandF( 0.0f, 8000.0f );

	SetRotation( XMFLOAT3( 0, MathHelper::RandF( -PI, PI ), 0 ) );
}

WeaponOnGround::~WeaponOnGround()
{
	delete	gPointLight;
}

bool WeaponOnGround::Update(float DeltaTime, Terrain* terrain)
{
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

	return true;
}


void WeaponOnGround::OnPickUp(Unit* Instance)
{
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