#include "WeaponFire.h"


WeaponFire::WeaponFire(void)
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Fire" ) );
	gTimer = 0.0f;
	gTimerStop = 0.1f;
	gMaxScale = 1.0f;
	gOwner = 0;
}

WeaponFire::WeaponFire(float timer, float maxScale, GameObject* owner)
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Fire" ) );
	gTimer = 0.0f;
	gTimerStop = timer;
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
		SetRotation(quat);
	}

	float progress = gTimer / gTimerStop;
	float scale = progress * gMaxScale;

	SetScale(scale);
}
