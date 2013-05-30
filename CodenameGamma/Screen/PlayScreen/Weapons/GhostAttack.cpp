#include "GhostAttack.h"

GhostAttack::GhostAttack()
{
	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(1, 1);
	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);

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

		float	tRotationY	=	GetFloat3Value( Rotations ).y;

		XMFLOAT3	tVelocity	=	XMFLOAT3(0, 0, 0);
		tVelocity.x	=	-tOrb->GetSpeed() * cos( tRotationY - PI * 0.5f );
		tVelocity.z	=	tOrb->GetSpeed() * sin( tRotationY - PI * 0.5f );

		tOrb->SetRotation( XMFLOAT3( 0, tRotationY, 0 ) );

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