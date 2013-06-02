#include "WeaponFire.h"
#include "Weapon.h"


WeaponFire::WeaponFire(void)
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Fire" ) );
	gTimer = 0.0f;
	gTimerStop = 0.1f;
	gMaxScale = 1.0f;
	gOwner = 0;
}

WeaponFire::WeaponFire(float timer, float minScale, float maxScale, GameObject* owner)
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Fire" ) );
	gTimer = 0.0f;
	gTimerStop = timer;
	gMinScale = minScale;
	gMaxScale = maxScale;
	gOwner = owner;
}

WeaponFire::~WeaponFire(void)
{
}

void WeaponFire::Update(float deltaTime, Terrain* terrain)
{
	GameObject::Update(deltaTime, terrain);

	gTimer += deltaTime;

	if (gTimer >= gTimerStop)
	{
		SetState( Dead );
		return;
	}

	if (gOwner)
	{
		XMFLOAT3 pipePos;
		if (gOwner->GetJointPosition("Pipe", pipePos));
		{
			MoveTo(pipePos);
		}

		XMFLOAT4 quat = gOwner->GetQuaternation();

		if (IsOfType<Weapon>(gOwner))
		{
			XMFLOAT4 offsetQuat = ((Weapon*)gOwner)->GetWeaponOffsetRotation();

			XMVECTOR quatV			= XMLoadFloat4(&quat);
			XMVECTOR offsetQuatV	= XMLoadFloat4(&offsetQuat);

			quatV = XMQuaternionMultiply(quatV, offsetQuatV);

			XMStoreFloat4(&quat, quatV);

		}

		SetRotation(quat);
	}

	float progress = gTimer / gTimerStop;
	float diff = gMaxScale - gMinScale;
	float scale = gMinScale + progress * diff;

	SetScale(scale);
}
