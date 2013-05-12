#include "Unit.h"


Unit::Unit(void)
{
	gHealth	=	UnitHealth(10.0f, 10.0f);

	gWeapon	=	NULL;
}

Unit::~Unit(void)
{
}

void Unit::DropWeapon()
{
	if (gWeapon)
	{
		gWeapon->SetState( Dead );
		gWeapon = NULL;
	}
}

void Unit::SetWeapon(Weapon* Weapon)
{
	if (gWeapon)
		gWeapon->SetState( Dead );

	gWeapon = Weapon; 

	if (gWeapon) 
		gWeapon->SetTeam(GetTeam()); 
}


UnitHealth Unit::GetHealth()
{
	return gHealth;
}

void Unit::SetHealth(UnitHealth HealthData)
{
	gHealth	=	HealthData;
}

bool Unit::Update(float DeltaTime, Terrain* TerrainInstance)
{
	if ( gHealth.first <= 0 )
	{
		SetState( Dead );
		return true;
	}

	bool updated = GameObject::Update(DeltaTime, TerrainInstance);

	XMVECTOR vel = XMLoadFloat3(&GetFloat3Value( Velocity ));
	if (!XMVector3Equal(vel, XMVectorZero()))
	{
		string animation = "Walk";
		XMVECTOR dir  = XMLoadFloat3(&GetFloat3Value( Direction ));
		XMVECTOR dotV = XMVector3Dot(vel, dir);

		float dot;
		XMStoreFloat(&dot, dotV);

		if (dot < 0)
			animation = "Back";

		if (animation != CurrentAnimationOrPose())
			PlayAnimation(animation);
	}
	else
	{
		string pose = "Stand";
		if (pose != CurrentAnimationOrPose())
			UsePose(pose);
	}

	if (gWeapon)
	{
		XMFLOAT3 position;

		XMVECTOR pos = XMLoadFloat3(&GetFloat3Value( Position ));
		XMVECTOR dir = XMLoadFloat3(&GetFloat3Value( Direction ));
		XMVECTOR lv = XMVector3Length(dir);

		bool gotJointPos = false;
		if (m_ModelInstance)
		{
			//gotJointPos = m_ModelInstance->GetJointPosition("RightArm", position);
			//gotJointPos = m_ModelInstance->GetJointPosition("Head", position);
			//gotJointPos = m_ModelInstance->GetJointPosition("HeadEnd", position);
			gotJointPos = m_ModelInstance->GetJointPosition("RightHand", position);
			//gotJointPos = m_ModelInstance->GetJointPosition("Hips", position);
			//gotJointPos = m_ModelInstance->GetJointPosition("LeftFoot", position);
			//gotJointPos = m_ModelInstance->GetJointPosition("RightFoot", position);
		}

		if (!gotJointPos)
		{
			pos += dir * (GetRadius() - 10);

			XMStoreFloat3(&position, pos);
			position.y += 40;
		}

		//gWeapon->Update(DeltaTime);
		//gWeapon->MoveTo( position );
		gWeapon->MoveTo( position );
		gWeapon->SetRotation( GetFloat3Value( Rotations ) );
	}

	return updated;
}

void Unit::ReceiveDamage(float Damage)
{
	gHealth.first	-=	Damage;
}

void Unit::Hit(Unit* Target)
{
	if ( !gWeapon->CanFire() )
		return;

	gWeapon->Fire();

	Target->ReceiveDamage(1.0f);
	ReceiveDamage(-1.0f);
}

void Unit::FireWeapon()
{
	if (gWeapon)
		gWeapon->Fire();
}

void Unit::CollideWith(GameObject* Instance)
{
}