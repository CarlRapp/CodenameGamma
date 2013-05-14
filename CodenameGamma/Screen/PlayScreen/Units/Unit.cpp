#include "Unit.h"
#include "../Items/WeaponOnGround.h"

Unit::Unit(void)
{
	gHealth	=	UnitHealth(10.0f, 10.0f);

	gCurrentWeapon	=	NULL;
}

Unit::~Unit(void)
{
}

void Unit::DropWeapon()
{
	if ( gCurrentWeapon )
	{
		WeaponOnGround*	tWoG	=	new WeaponOnGround( gCurrentWeapon );
		tWoG->MoveTo( gCurrentWeapon->GetFloat3Value( Position ) );

		tWoG->SetRotation( GetFloat3Value( Rotations ) );

		AddGameObject( tWoG );
		tWoG->Update(0, 0);

		gCurrentWeapon	=	0;
	}
}

void Unit::SetWeapon(Weapon* Weapon)
{
	if ( !Weapon )
		return;

	if ( gCurrentWeapon )
		DropWeapon();

	gCurrentWeapon = Weapon; 

	if (gCurrentWeapon) 
		gCurrentWeapon->SetTeam(GetTeam());

	if ( !Weapon->CanFire() )
		Weapon->Reload();
}


UnitHealth Unit::GetHealth()
{
	return gHealth;
}

void Unit::SetHealth(UnitHealth HealthData)
{
	gHealth	=	HealthData;
}

void Unit::Update(float DeltaTime, Terrain* TerrainInstance)
{
	if ( gHealth.first <= 0 )
	{
		SetState( Dead );
		return;
	}

	GameObject::Update(DeltaTime, TerrainInstance);

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

	if ( gCurrentWeapon )
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
		gCurrentWeapon->MoveTo( position );
		gCurrentWeapon->SetRotation( GetFloat3Value( Rotations ) );
	}
}

void Unit::Hurt(float Damage)
{
	gHealth.first	-=	Damage;

	if ( gHealth.first <= 0 )
		SetState( Dead );
}

void Unit::ReloadWeapon()
{
	if ( gCurrentWeapon )
		gCurrentWeapon->Reload();
}

void Unit::FireWeapon()
{
	if ( gCurrentWeapon )
		gCurrentWeapon->Fire();
}

bool Unit::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	return false;
}

void Unit::CollideWith(GameObject* Instance)
{

}