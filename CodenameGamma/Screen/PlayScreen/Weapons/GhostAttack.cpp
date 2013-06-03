#include "GhostAttack.h"

GhostAttack::GhostAttack()
{
	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(1, 1);
	gReloadTime	=	WeaponReloadTime(0.0f, 3.0f);
	gAmmo		=	-1;
	gWeaponAnimations.Aim				= "Aim";
	gWeaponAnimations.Draw				= "Draw";
	gWeaponAnimations.PutAway			= "PutAway";
	gWeaponAnimations.Shoot				= "Shoot";
}

GhostAttack::~GhostAttack()
{

}

bool GhostAttack::Fire( GameObject* Owner, GameObject* Target, float DamageMul )
{
	if ( CanFire() )
	{
		Orb*	tOrb	=	new Orb();
		tOrb->MultiplyDamage( DamageMul );

		tOrb->SetTarget( Target );

		XMVECTOR	tVelocityV	= tOrb->GetSpeed() * XMVector3Normalize( XMLoadFloat3( &GetFloat3Value(Direction) ) );
		XMFLOAT3	tVelocity	=	GetFloat3Value( Direction );
		XMStoreFloat3(&tVelocity, tVelocityV);

		tOrb->SetRotation( GetQuaternation() );

		XMFLOAT3 pipePos;

		tOrb->MoveTo( GetFloat3Value( Position ) );

		tOrb->SetVelocity( tVelocity );
		tOrb->SetTeam( GetTeam() );
		tOrb->SetOwner( Owner );

		//SoundManager::GetInstance()->Play("Pistol", SFX);

		gCooldown.first	=	gCooldown.second;
		--gClip.first;

		AddGameObject( tOrb );

		return true;
	}
	else if ( gClip.first == 0 && gCooldown.first <= 0)
	{
		gCooldown.first	=	gCooldown.second;
	}

	return false;
}