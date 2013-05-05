#include "Unit.h"


Unit::Unit(void)
{
	gHealth	=	UnitHealth(10.0f, 10.0f);

	gWeapon	=	new Pistol();
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

	bool updated = GameObject::Update(DeltaTime, TerrainInstance);

	XMVECTOR pos = XMLoadFloat3(&GetFloat3Value( Position ));
	XMVECTOR dir = XMLoadFloat3(&GetFloat3Value( Direction ));

	pos += dir * (GetRadius() + 10);

	XMFLOAT3 position;
	XMStoreFloat3(&position, pos);

	gWeapon->Update(DeltaTime);
	gWeapon->MoveTo( position );
	gWeapon->SetRotation( GetFloat3Value( Rotations ) );

	return updated;
}

void Unit::ReceiveDamage(float Damage)
{
	gHealth.first	-=	Damage;
}

void Unit::Hit(Unit* Target)
{
	if ( !gWeapon->CanFire() )
		return;

	gWeapon->Fire();

	Target->ReceiveDamage(1.0f);
	ReceiveDamage(-1.0f);
}

vector<Projectile*> Unit::FireWeapon()
{
	if ( !gWeapon->CanFire() )
		return vector<Projectile*>();

	return gWeapon->Fire();
}

void Unit::CollideWith(GameObject* Instance)
{

}