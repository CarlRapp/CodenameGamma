#include "Unit.h"


Unit::Unit(void)
{
	gHealth	=	UnitHealth(10.0f, 10.0f);

	gCooldown	=	2.0f;
}

Unit::~Unit(void)
{
}

UnitHealth Unit::GetHealth()
{
	return gHealth;
}
void Unit::SetHealth(UnitHealth HealthData)
{
	gHealth	=	HealthData;
}

bool Unit::Update(float DeltaTime, Terrain* TerrainInstance)
{
	if ( gHealth.first <= 0 )
		SetState( Dead );

	if( gCooldown > 0 )
		gCooldown	-=	DeltaTime;

	return GameObject::Update(DeltaTime, TerrainInstance);
}

void Unit::ReceiveDamage(float Damage)
{
	gHealth.first	-=	Damage;
}

void Unit::Hit(Unit* Target)
{
	if ( gCooldown > 0 )
		return;

	gCooldown	=	0.5f;
	Target->ReceiveDamage(1.0f);
	ReceiveDamage(-1.0f);
}