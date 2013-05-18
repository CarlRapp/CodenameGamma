#include "PlayerUnit.h"


PlayerUnit::PlayerUnit()
{
	gHealth	=	UnitHealth( 100.0f, 100.0f );
	gHunger	=	UnitHunger( 100.0f, 100.0f );
	gThirst	=	UnitThirst( 100.0f, 100.0f );
}

PlayerUnit::~PlayerUnit()
{
}

void PlayerUnit::Update(float DeltaTime, Terrain* terrain)
{
	UpdateMeters(DeltaTime);

	UpdateTrail(DeltaTime);

	Unit::Update(DeltaTime, terrain);
}

void PlayerUnit::UpdateMeters(float DeltaTime)
{
	float	speed	=	GetSpeed();

	//	TODO
	//	Make the reduction
	//	speed based on
	//	how fast the unit
	//	is going.
	gHunger.first	-=	DeltaTime * speed;
	gThirst.first	-=	DeltaTime * speed;

	gHunger.first	=	( gHunger.first < 0 ) ? 0 : gHunger.first;
	gThirst.first	=	( gThirst.first < 0 ) ? 0 : gThirst.first;

	
	if ( gHunger.first == 0 )
		Hurt( 1.0f * DeltaTime );
	if ( gThirst.first == 0 )
		Hurt( 1.0f * DeltaTime );
		
}

void PlayerUnit::UpdateTrail(float deltaTime)
{
	//Update trailtimer
	trailTimer += deltaTime;

	vector<Trail> trash;
	//Update trails
	for (int i = gTrails.size() - 1; i >= 0; --i)
	{

		gTrails[i].Update(deltaTime);
		if (!gTrails[i].IsAlive())
			gTrails.erase(gTrails.begin() + i);
			
	}

	//Add trail to gTrails
	if (trailTimer >= TRAIL_INTERVAL)
	{
		trailTimer = 0.0f;

		Trail trail = Trail(GetFloat3Value( Position ), 0.0f, TRAIL_LIFETIME);
		gTrails.push_back(trail);
	}	
}

void PlayerUnit::Hurt(float Damage)
{
	float	tDamage	=	Damage * ( 2 - gHunger.first * 0.01f );

	gHealth.first	-=	tDamage;
	if( gHealth.first < 0 )
		SetState( Dead );
}

void PlayerUnit::SetVelocity(XMFLOAT3 Velocity)
{
	XMFLOAT3	newVelocity;
	float		tModifier	=	0.5f + gThirst.first * 0.01f;
	tModifier	=	MathHelper::Clamp( tModifier, 0.1f, 1.0f );

	XMStoreFloat3(
		&newVelocity,
		tModifier * XMLoadFloat3( &Velocity )
	);

	GameObject::SetVelocity( newVelocity );
}