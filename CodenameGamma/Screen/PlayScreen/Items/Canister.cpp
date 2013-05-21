#include "Canister.h"


Canister::Canister()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Cannister" ) );

	gPointLight	=	0;
	gOffset		=	XMFLOAT3( 0, 6, 0 );
	gTimeSpan	=	MathHelper::RandF( 0.0f, 8000.0f );
}

Canister::~Canister()
{
	delete	gPointLight;
}

void Canister::Update(float DeltaTime, Terrain* terrain)
{
	XMFLOAT3	newPos	=	GetFloat3Value( Position );

	if ( !gPointLight )
	{
		gPointLight	=	new PointLight();
		gPointLight->GetGPULight()->Color		=	XMFLOAT4( 1.0f, 1.0f, 1.0f, 0.0f );
		gPointLight->GetGPULight()->Position	=	newPos;
		gPointLight->GetGPULight()->Range		=	33.333f * 0.40f;
		gPointLight->GetGPULight()->HasShadow	=	false;

		AddLight( gPointLight );
	}

	gTimeSpan	+=	DeltaTime;
	
	newPos.y	=	gOffset.y + ( gOffset.y - 2 ) * sin( 8 * gTimeSpan );
	MoveTo( newPos );
	newPos.y	-=	1.0f;
	if ( gPointLight )
		gPointLight->GetGPULight()->Position	=	newPos;

	Item::Update( DeltaTime, terrain );
}


void Canister::OnPickUp(Unit* Instance)
{
	if ( !IsOfType<PlayerUnit>(Instance) )
		return;

	PlayerUnit*	pUnit	=	(PlayerUnit*)Instance;
	pUnit->Drink( 15 );
	
	SetState( Dead );

	RemoveLight( gPointLight );
}