#include "Pistol.h"
#include "../../ScreenEnums.h"

Pistol::Pistol()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Glock" ) );

	gCooldown	=	WeaponCooldown(0.0f, 0.45f);
	gClip		=	WeaponClip(12, 12);
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

		SoundManager::GetInstance()->Play("Pistol_Fire", SFX);

		gCooldown.first	=	gCooldown.second;
		--gClip.first;

		tBullet->SetDamage(10.0f);

		AddGameObject( tBullet );

		WeaponFire* fire = new WeaponFire(0.1f, 0.5f, 1.0f, this);
		AddGameObject(fire);

		return true;
	}
	else if ( gClip.first == 0 && gCooldown.first <= 0)
	{
		
		gCooldown.first	=	gCooldown.second;
	}

	return false;
}