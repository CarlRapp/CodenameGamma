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
	{/*
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
		*/

		XMFLOAT4 quat = GetQuaternation();
		XMFLOAT4 offsetQuat		= GetWeaponOffsetRotation();
		XMVECTOR quatV			= XMLoadFloat4(&quat);
		XMVECTOR offsetQuatV	= XMLoadFloat4(&offsetQuat);
		quatV = XMQuaternionMultiply(quatV, offsetQuatV);
		XMStoreFloat4(&quat, quatV);
		

		XMMATRIX	offsetRotation = XMMatrixRotationQuaternion( offsetQuatV );
		XMVECTOR	tVelocityV	= XMVector3Normalize( XMLoadFloat3( &GetFloat3Value(Direction) ) );

		tVelocityV = XMVector3TransformCoord(tVelocityV, offsetRotation);
		tVelocityV = XMVectorSetY(tVelocityV, 0);

		

		XMFLOAT3 pipePos;
		if (!GetJointPosition("Pipe", pipePos))
			pipePos = GetFloat3Value( Position );

		for (int i = 0; i < NumBullets; ++i)
		{
			Bullet*	tBullet	=	new Bullet();
			tBullet->MultiplyDamage( DamageMul );
			tBullet->SetRotation( quat );
			tBullet->MoveTo( pipePos );

			XMVECTOR	velocityV	= tVelocityV * tBullet->GetSpeed();
			
			XMVECTOR	up			= XMLoadFloat3(&XMFLOAT3(0,1,0));
			
			XMVECTOR	axis		= XMVector3Cross(velocityV, up);
			float angle				= MathHelper::RandF(0, PI * 2);

			XMVECTOR tQuat = XMQuaternionRotationAxis(velocityV, angle);

			XMMATRIX tRot  = XMMatrixRotationQuaternion(tQuat);

			axis	= XMVector3TransformCoord(axis, tRot);

			angle	= XMConvertToRadians( MathHelper::RandF(0, MaxSpread) );
			tQuat	= XMQuaternionRotationAxis(axis, angle);
			tRot	= XMMatrixRotationQuaternion(tQuat);

			velocityV	= XMVector3TransformCoord(velocityV, tRot);

			XMFLOAT3	tVelocity;
			XMStoreFloat3(&tVelocity, velocityV);
			tBullet->SetVelocity( tVelocity );

			tBullet->SetTeam( Owner->GetTeam() );
			tBullet->SetOwner( Owner );

			AddGameObject( tBullet );

		}
		/*
		XMFLOAT3 pipePos;

		if (GetJointPosition("Pipe", pipePos))
			tBullet->MoveTo( pipePos );
		else
			tBullet->MoveTo( GetFloat3Value( Position ) );

		tBullet->SetVelocity( tVelocity );
		tBullet->SetTeam( Owner->GetTeam() );
		tBullet->SetOwner( Owner );
		*/
		SoundManager::GetInstance()->Play("Shotgun_Fire", SFX);

		gCooldown.first	=	gCooldown.second;
		--gClip.first;

		//AddGameObject( tBullet );

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