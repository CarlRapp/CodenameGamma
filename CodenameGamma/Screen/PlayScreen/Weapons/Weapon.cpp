#include "Weapon.h"

Weapon::Weapon(void)
{
	gCooldown	=	WeaponCooldown( 0.0f, 0.0f );
	gClip		=	WeaponClip( 0, 0 );
	gReloadTime	=	WeaponReloadTime( 0.0f, 0.0f );

	gState		=	Ready;

	gReloadSound = "";

	gAmmo		=	0;
	gWOGModel	=	0;
}

Weapon::~Weapon(void)
{

}

XMFLOAT4 Weapon::GetWeaponOffsetRotation()
{
	XMVECTOR quat = XMQuaternionIdentity();

	XMFLOAT3 direction = GetFloat3Value( Direction );

	XMFLOAT3 handPos;
	XMFLOAT3 pipePos;

	if (GetJointPosition("RightHand", handPos) && GetJointPosition("Pipe", pipePos))
	{
		XMVECTOR directionV	= XMLoadFloat3(&direction);
		XMVECTOR handPosV	= XMLoadFloat3(&handPos);
		XMVECTOR pipePosV	= XMLoadFloat3(&pipePos);

		XMVECTOR offsetDirectionV = pipePosV - handPosV;

		XMVECTOR angleV = XMVector3AngleBetweenVectors(directionV, offsetDirectionV);
		float angle;
		XMStoreFloat(&angle, angleV);

		if (angle != 0)
		{
			XMVECTOR axis = XMVector3Cross(directionV, offsetDirectionV);
			quat = XMQuaternionRotationAxis(axis, angle);
		}
	}

	XMFLOAT4 result;
	XMStoreFloat4(&result, quat);

	return result;
}

void Weapon::LowerCooldown(float DeltaTime)
{
	if ( gCooldown.first > 0 )
	{
		gCooldown.first	-=	DeltaTime;

		gCooldown.first	=	( gCooldown.first <= 0 ) ? 0 : gCooldown.first;
	}
}

void Weapon::ReloadCountdown(float DeltaTime)
{
	if ( gState == Reloading )
	{
		gReloadTime.first	-=	DeltaTime;

		if ( gReloadTime.first <= 0 )
		{
			//	Check so the gun has enough
			//	ammo
			if( gAmmo - gClip.second >= 0 || gAmmo == -1 )
			{
				gClip.first	=	gClip.second;
				gAmmo		-=	( gAmmo == -1 ) ? 0 : gClip.first;
			}
			else
			{
				gClip.first	=	gAmmo;
				gAmmo	=	0;
			}

			gState				=	Ready;
			gReloadTime.first	=	0;
		}
	}
}

void Weapon::Update(float deltaTime, Terrain* terrain)
{
	LowerCooldown(deltaTime);

	ReloadCountdown(deltaTime);
}

bool Weapon::Fire( GameObject* Owner, GameObject* Target, float DamageMul  )
{
	return false;
}

bool Weapon::Reload()
{
	if ( gState == Reloading || gState == Empty )
		return false;

	if( gClip.second == 0 )
	{
		gState	=	Empty;
		return false;
	}
	
	gClip.first			=	0;
	gReloadTime.first	=	gReloadTime.second;
	gState				=	Reloading;

	if (gReloadSound != "")
		gReloadSoundIndex	=	SoundManager::GetInstance()->Play(gReloadSound, SFX);
	return true;
}

bool Weapon::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	return false;
}

bool Weapon::IsDropable()
{
	return true;
}