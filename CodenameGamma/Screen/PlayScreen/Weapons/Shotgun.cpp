#include "Shotgun.h"
#include "../../ScreenEnums.h"

Shotgun::Shotgun()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Shotgun" ) );
	SetWOGModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Shotgun-WOG" ) );

	gCooldown	=	WeaponCooldown(0.0f, 1.0f);
	gClip		=	WeaponClip(8, 8);
	gAmmo		=	16;

	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);

	gWeaponAnimations.Aim				= "ShotgunAim";
	gWeaponAnimations.Draw				= "WeaponDraw";
	gWeaponAnimations.DrawReloadPutAway = "ShotgunDrawReloadPutAway";
	gWeaponAnimations.PutAway			= "WeaponPutAway";
	gWeaponAnimations.Shoot				= "ShotgunShoot";
	gWeaponAnimations.Reload			= "ShotgunReload";
	gWeaponAnimations.UpperWalk			= "WeaponUpperWalk";
	gWeaponAnimations.UpperRun			= "WeaponUpperRun";
	gWeaponAnimations.UpperStand		= "WeaponUpperStand";

	gReloadSound = "Shotgun_Reload";
}

Shotgun::~Shotgun()
{

}

bool Shotgun::CanFire()
{
	return (gClip.first > 0 && gCooldown.first <= 0);
}

bool Shotgun::Fire( GameObject* Owner, GameObject* Target, float DamageMul )
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

		SoundManager::GetInstance()->Play("Shotgun_Fire", SFX);

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