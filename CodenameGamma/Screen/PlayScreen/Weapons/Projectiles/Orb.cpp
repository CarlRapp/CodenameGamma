#include "Orb.h"

Orb::Orb()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "EnergyOrb" ) );
	gDamage	=	10.0f;

	gLifeSpan	=	3.5f;
}
Orb::~Orb()
{

}

void Orb::Update(float deltaTime, Terrain* terrain)
{
	if ( gTarget )
	{
		XMVECTOR orbPos = XMLoadFloat3( &GetFloat3Value( Position ) );
		XMVECTOR targetPos = XMLoadFloat3( &gTarget->GetFloat3Value( Position ) );

		XMVECTOR current = XMVector3Normalize(XMLoadFloat3( &GetFloat3Value( Velocity ) ));
		XMVECTOR perfect = XMVector3Normalize(targetPos - orbPos);

		XMVECTOR diff = perfect - current;

		XMVECTOR newDir = (current + (diff * deltaTime * 1.0f));	
		newDir = XMVectorSetY(newDir, 0.0f);
		XMVECTOR newVel = XMVector3Normalize(newDir) * GetSpeed();

		XMFLOAT3 newVelocity;
		XMStoreFloat3( &newVelocity, newVel);

		SetVelocity( newVelocity );
	}
	
	Projectile::Update(deltaTime, terrain);
}

float Orb::GetSpeed()
{
	return 100.0f;
}