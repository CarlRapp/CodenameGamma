#include "SniperRifle.h"
#include "../../ScreenEnums.h"

SniperRifle::SniperRifle()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SniperRifle" ) );
	SetWOGModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SniperRifle-WOG" ) );

	gCooldown	=	WeaponCooldown(0.0f, 1.5f);
	gClip		=	WeaponClip(8, 8);
	gAmmo		=	16;

	gReloadTime	=	WeaponReloadTime(0.0f, 4.0f);

	gWeaponAnimations.Aim				= "RifleAim";
	gWeaponAnimations.Draw				= "WeaponDraw";
	gWeaponAnimations.DrawReloadPutAway = "RifleDrawReloadPutAway";
	gWeaponAnimations.PutAway			= "WeaponPutAway";
	gWeaponAnimations.Shoot				= "RifleShoot";
	gWeaponAnimations.Reload			= "RifleReload";
	gWeaponAnimations.UpperWalk			= "WeaponUpperWalk";
	gWeaponAnimations.UpperRun			= "WeaponUpperRun";
	gWeaponAnimations.UpperStand		= "WeaponUpperStand";

	gReloadSound = "Pistol_Reload";
}

SniperRifle::~SniperRifle()
{

}

bool SniperRifle::CanFire()
{
	return (gClip.first > 0 && gCooldown.first <= 0);
}

bool SniperRifle::Fire( GameObject* Owner, GameObject* Target, float DamageMul )
{
	if ( CanFire() )
	{
		Bullet*	tBullet	=	new Bullet();
		tBullet->MultiplyDamage( DamageMul );

		XMFLOAT4 quat = GetQuaternation();
		XMFLOAT4 offsetQuat		= GetWeaponOffsetRotation();
		XMVECTOR quatV			= XMLoadFloat4(&quat);
		XMVECTOR offsetQuatV	= XMLoadFloat4(&offsetQuat);
		quatV = XMQuaternionMultiply(quatV, offsetQuatV);
		XMStoreFloat4(&quat, quatV);
		tBullet->SetRotation( quat );

		XMMATRIX	offsetRotation = XMMatrixRotationQuaternion( offsetQuatV );
		XMVECTOR	tVelocityV	= tBullet->GetSpeed() * XMVector3Normalize( XMLoadFloat3( &GetFloat3Value(Direction) ) );

		tVelocityV = XMVector3TransformCoord(tVelocityV, offsetRotation);
		tVelocityV = XMVectorSetY(tVelocityV, 0);

		XMFLOAT3	tVelocity;
		XMStoreFloat3(&tVelocity, tVelocityV);

		XMFLOAT3 pipePos;

		if (GetJointPosition("Pipe", pipePos))
			tBullet->MoveTo( pipePos );
		else
			tBullet->MoveTo( GetFloat3Value( Position ) );

		tBullet->SetVelocity( tVelocity );
		tBullet->SetTeam( Owner->GetTeam() );
		tBullet->SetOwner( Owner );

		tBullet->SetDamage(75.0f);
		tBullet->SetLifeSpan(2.5f);

		SoundManager::GetInstance()->Play("Rifle_Fire", SFX);

		gCooldown.first	=	gCooldown.second;
		--gClip.first;

		tBullet->SetHP( 3 );

		AddGameObject( tBullet );

		WeaponFire* fire = new WeaponFire(0.1f, 1.0f, 2.0f, this);
		AddGameObject(fire);

		return true;
	}
	else if ( gClip.first == 0 && gCooldown.first <= 0)
	{
		
		gCooldown.first	=	gCooldown.second;
	}

	return false;
}