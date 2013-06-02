#include "Pistol.h"
#include "../../ScreenEnums.h"

Pistol::Pistol()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Glock" ) );

	gCooldown	=	WeaponCooldown(0.0f, 0.2f);
	gClip		=	WeaponClip(9, 9);
	gReloadTime	=	WeaponReloadTime(0.0f, 2.5f);
	gAmmo		=	-1;

	gWeaponAnimations.Aim				= "PistolAim";
	gWeaponAnimations.Draw				= "PistolDraw";
	gWeaponAnimations.DrawReloadPutAway = "PistolDrawReloadPutAway";
	gWeaponAnimations.PutAway			= "PistolPutAway";
	gWeaponAnimations.Shoot				= "PistolShoot";
	gWeaponAnimations.Reload			= "PistolReload";
	gWeaponAnimations.UpperWalk			= "PistolUpperWalk";
	gWeaponAnimations.UpperRun			= "PistolUpperRun";
	gWeaponAnimations.UpperStand		= "PistolUpperStand";

	gReloadSound = "Pistol_Reload";
}

Pistol::~Pistol()
{

}

bool Pistol::Fire( GameObject* Owner, GameObject* Target, float DamageMul )
{
	if ( CanFire() )
	{
		Bullet*	tBullet	=	new Bullet();
		tBullet->MultiplyDamage( DamageMul );

		XMVECTOR	tVelocityV	= tBullet->GetSpeed() * XMVector3Normalize( XMLoadFloat3( &GetFloat3Value(Direction) ) );
		XMFLOAT3	tVelocity	=	GetFloat3Value( Direction );
		XMStoreFloat3(&tVelocity, tVelocityV);

		tBullet->SetRotation( GetQuaternation() );

		XMFLOAT3 pipePos;

		if (GetJointPosition("Pipe", pipePos))
			tBullet->MoveTo( pipePos );
		else
			tBullet->MoveTo( GetFloat3Value( Position ) );

		tBullet->SetVelocity( tVelocity );
		tBullet->SetTeam( Owner->GetTeam() );
		tBullet->SetOwner( Owner );

		SoundManager::GetInstance()->Play("Pistol_Fire", SFX);

		gCooldown.first	=	gCooldown.second;
		--gClip.first;

		AddGameObject( tBullet );

		WeaponFire* fire = new WeaponFire(0.1f, 2.0f, this);

		AddGameObject(fire);

		return true;
	}
	else if ( gClip.first == 0 && gCooldown.first <= 0)
	{
		
		gCooldown.first	=	gCooldown.second;
	}

	return false;
}