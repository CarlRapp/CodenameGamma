#include "Unit.h"
#include "../Items/WeaponOnGround.h"
#include "../Weapons/Pistol.h"
#include "../Weapons/Shotgun.h"

Unit::Unit(void)
{
	gMoveState		= Stand;
	gWeaponState	= Hold;

	gHealth	=	UnitHealth(10.0f, 10.0f);

	gCurrentWeapon	=	NULL;

	gWalkSpeed = 1.0f;
	gRunSpeed  = 1.0f;
}

Unit::~Unit(void)
{
}

void Unit::DropWeapon()
{
	if ( gCurrentWeapon )
	{
		if( !gCurrentWeapon->IsDropable() )
			return;

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
	SetWeaponState(gWeaponState);

	if (gCurrentWeapon) 
		gCurrentWeapon->SetTeam(GetTeam());

	if ( !Weapon->CanFire() )
	{
		ReloadWeapon();
	}
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
	if ( IsAlive() && gHealth.first <= 0 )
	{
		DropWeapon();
		Kill();
		StopAllAnimations();
		PlayAnimation("Death");
		PlayAnimationAfter("Death", "Dead");

		if (!PlayingAnimation("Death"))
		{
			SetState( Dead );
			return;
		}		
	}

	GameObject::Update(DeltaTime, TerrainInstance);

	if ( GetState() == Dying )
		return;

	XMVECTOR vel = XMLoadFloat3(&GetFloat3Value( Velocity ));
	if (!XMVector3Equal(vel, XMVectorZero()))
	{
		XMVECTOR dir  = XMLoadFloat3(&GetFloat3Value( Direction ));
		XMVECTOR angleV = XMVector3AngleBetweenVectors(dir, vel);

		float angle;
		XMStoreFloat(&angle, angleV);

		XMVECTOR crossV = XMVector3Cross(dir, vel);

		if (XMVectorGetY(crossV) < 0)
			angle *= -1;

		//cout << 180 * angle / PI << endl;

		if (fabs(angle) <= PI * 0.20f)
			gMoveDirection = Forward;

		else if (fabs(angle) > PI * 0.80f)
			gMoveDirection = Back;

		else if (angle < 0)
			gMoveDirection = Left;

		else if (angle > 0)
			gMoveDirection = Right;

		XMVECTOR L = XMVector3Length(vel);
		float speed;
		XMStoreFloat(&speed, L);
		speed /= UnitsPerMeter;

		switch (gMoveDirection)
		{
		case Forward:
			if (speed > 1.05f * gWalkSpeed)
			{
				SetMoveState(Run);	
				SetAnimationSpeed("Run", speed);
				SetAnimationSpeed(GetAnimation("UpperRun"), speed);							
			}
			else
			{
				SetMoveState(Walk);
				SetAnimationSpeed("Walk", speed);
			}
			break;
		case Back:
			SetMoveState(Walk);
			SetAnimationSpeed("Back", speed);
			break;
		case Right:
			SetMoveState(Walk);
			SetAnimationSpeed("SideStepRight", speed);
			break;
		case Left:
			SetMoveState(Walk);
			SetAnimationSpeed("SideStepLeft", speed);
			break;
		}
	}
	else
	{
		gMoveDirection = None;

		if (!Crouching())
			SetMoveState(Stand);
	}

	if ( gCurrentWeapon )
	{
		XMFLOAT3 handPos;
		
		XMVECTOR pos = XMLoadFloat3(&GetFloat3Value( Position ));
		XMVECTOR dir = XMLoadFloat3(&GetFloat3Value( Direction ));
		XMVECTOR lv = XMVector3Length(dir);

		bool gotJointPos = false;
		if (m_ModelInstance)
		{
			gotJointPos = GetJointPosition("RightHand", handPos);
		}

		if (!gotJointPos)
		{
			pos += dir * (GetRadius() - 10);
			XMStoreFloat3(&handPos, pos);
		}
		
		if (gCurrentWeapon->NeedReload())
		{
			ReloadWeapon();
		}
		
		//gWeapon->Update(DeltaTime);
		//gWeapon->MoveTo( position );
		gCurrentWeapon->MoveTo( handPos );

		XMFLOAT3 weaponPos;
		if (gCurrentWeapon->GetJointPosition("RightHand", weaponPos))
		{
			XMStoreFloat3(&handPos, XMLoadFloat3(&handPos) + (XMLoadFloat3(&handPos) - XMLoadFloat3(&weaponPos)));
			gCurrentWeapon->MoveTo( handPos );
		}
		gCurrentWeapon->SetRotation( GetFloat3Value( Rotations ) );
	}
	/*
	else
	{
		LoopAnimation("PistolUpperWalk");
	}
	*/
}

void Unit::UpdateAnimation()
{

}

bool BodyInSync;
void Unit::SetMoveState(MoveState newMoveState)
{
	switch (newMoveState)
	{
	case Stand:

		if (gMoveState == Crouch)
		{
			float progress = 1.0f - GetAnimationProgress("Crouching");
			PlayAnimation("StandUp");
			PlayAnimationAfter("StandUp", "Pose");
			SetAnimationProgress("StandUp", progress);
		}

		if (!PlayingAnimation("StandUp"))
			PlayAnimation("Pose");	

		if (PlayingAnimation(GetAnimation("UpperWalk")) || PlayingAnimation(GetAnimation("UpperRun")))
		{
			StopAnimation(GetAnimation("UpperWalk"));
			StopAnimation(GetAnimation("UpperRun"));
			LoopAnimation(GetAnimation("UpperStand"));
		}
		break;
	case Walk:

		switch (gMoveDirection)
		{
		case Forward:
			LoopAnimation("Walk");
			break;
		case Back:
			LoopAnimation("Back");
			break;
		case Right:
			LoopAnimation("SideStepRight");
			break;
		case Left:
			LoopAnimation("SideStepLeft");
			break;		
		}

		if (PlayingAnimation(GetAnimation("UpperStand")) || PlayingAnimation(GetAnimation("UpperRun")))
		{
			StopAnimation(GetAnimation("UpperStand"));
			StopAnimation(GetAnimation("UpperRun"));
			LoopAnimation(GetAnimation("UpperWalk"));
		}

		break;
	
	case Run:

		if (gMoveState != Run)
			BodyInSync = false;

		LoopAnimation("Run");

		if (PlayingAnimation(GetAnimation("DrawReloadPutAway")))
			LoopAnimationAfter(GetAnimation("DrawReloadPutAway"), GetAnimation("UpperRun"));

		else if (PlayingAnimation(GetAnimation("Reload")))
		{
			PlayAnimationAfter(GetAnimation("Reload"), GetAnimation("PutAway"));
			LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperRun"));
		}

		else if (PlayingAnimation(GetAnimation("PutAway")))			
			LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperRun"));

		else
		{
			LoopAnimation(GetAnimation("UpperRun"));

			if (!BodyInSync)
			{
				float progress = GetAnimationProgress(GetAnimation("UpperRun")) + 0.5f;
				progress -= (int)progress;
				if (SetAnimationProgress(GetAnimation("UpperRun"), progress))
					BodyInSync = true;
			}
		}

		break;

	case Crouch:
		SetVelocity(XMFLOAT3(0, 0, 0));
		if (!PlayingAnimation("Crouch"))
		{
			float progress = 1.0f - GetAnimationProgress("StandUp");
			PlayAnimation("Crouching");
			PlayAnimationAfter("Crouching", "Crouch");
			SetAnimationProgress("Crouching", progress);
		}
				
		if (PlayingAnimation(GetAnimation("UpperWalk")) || PlayingAnimation(GetAnimation("UpperRun")))
		{
			StopAnimation(GetAnimation("UpperWalk"));
			StopAnimation(GetAnimation("UpperRun"));
			LoopAnimation(GetAnimation("UpperStand"));
		}
		break;
	}

	gMoveState = newMoveState;
}

void Unit::SetWeaponState(WeaponState newWeaponState)
{
	switch (gWeaponState)
	{
	case Hold:

		switch (newWeaponState)
		{
		case Hold:
			if ( gMoveState == Walk || gMoveState == Stand )
			{
				LoopAnimation(GetAnimation("UpperWalk"));
			}
			else if ( gMoveState == Run )
			{
				LoopAnimation(GetAnimation("UpperRun"));
			}
			break;
		case Aim:
			float progress = 1.0f - GetAnimationProgress(GetAnimation("PutAway"));

			StopAnimation(GetAnimation("UpperWalk"));
			StopAnimation(GetAnimation("PutAway"));

			if (PlayingAnimation(GetAnimation("DrawReloadPutAway")))
				PlayAnimationAfter(GetAnimation("DrawReloadPutAway"), GetAnimation("Draw"));

			else if (PlayingAnimation(GetAnimation("Reload")))
				PlayAnimationAfter(GetAnimation("Reload"), GetAnimation("Draw"));

			else
			{
				PlayAnimation(GetAnimation("Draw"));
				SetAnimationProgress(GetAnimation("Draw"), progress);
			}
			PlayAnimationAfter(GetAnimation("Draw"), GetAnimation("Aim"));
			break;
		}

		break;
	case Aim:

		switch (newWeaponState)
		{
		case Aim:
			PlayAnimation(GetAnimation("Aim"));
			break;
		case Hold:

			float progress = 1.0f - GetAnimationProgress(GetAnimation("Draw"));

			StopAnimation(GetAnimation("Aim"));
			StopAnimation(GetAnimation("Draw"));

			if (PlayingAnimation(GetAnimation("Reload")))
				PlayAnimationAfter(GetAnimation("Reload"), GetAnimation("PutAway"));

			else if (PlayingAnimation(GetAnimation("DrawReloadPutAway")))
				PlayAnimationAfter(GetAnimation("DrawReloadPutAway"), GetAnimation("PutAway"));

			else
			{
				PlayAnimation(GetAnimation("PutAway"));
				SetAnimationProgress(GetAnimation("PutAway"), progress);
			}
			LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperWalk"));
			break;
		}
		break;
	}

	gWeaponState = newWeaponState;
}

void Unit::Hurt(float Damage)
{
	gHealth.first	-=	Damage;

	//if ( gHealth.first <= 0 )
	//	SetState( Dead );
}

void Unit::ReloadWeapon()
{
	if ( gCurrentWeapon )
	{
		if (gCurrentWeapon->Reload())
		{
			float reloadTime	 = gCurrentWeapon->GetReloadTime();

			string animation;
			string animationAfter;
			switch (gWeaponState)
			{
			case Hold:
				animation = GetAnimation("DrawReloadPutAway");

				if ( gMoveState == Run )
					animationAfter = GetAnimation("UpperRun");
				else
					animationAfter = GetAnimation("UpperWalk");

				if (PlayingAnimation(GetAnimation("PutAway")))
					PlayAnimationAfter(GetAnimation("PutAway"), animation);
				else
					PlayAnimation(animation);

				LoopAnimationAfter(animation, animationAfter);
				SetAnimationSpeed(animation, GetAnimationTime(animation) / reloadTime);
				break;
			case Aim:
				animation = GetAnimation("Reload");
				animationAfter = GetAnimation("Aim");

				if (PlayingAnimation(GetAnimation("Draw")))
					PlayAnimationAfter(GetAnimation("Draw"), animation);
				else
					PlayAnimation(animation);

				PlayAnimationAfter(animation, animationAfter);
				SetAnimationSpeed(animation, GetAnimationTime(animation) / reloadTime);

				break;
			}
		}		
	}
}

void Unit::FireWeapon()
{
	if ( gCurrentWeapon )
	{
		if (gWeaponState == Aim && !PlayingAnimation(GetAnimation("Draw")))
		{
			if (gCurrentWeapon->Fire( this, 1.0f ))
			{
				PlayAnimation(GetAnimation("Shoot"));
				PlayAnimationAfter(GetAnimation("Shoot"), GetAnimation("Aim"));
			}
		}
	}
}

bool Unit::Intersects(GameObject* B, vector<CollisionData>& CD)
{
	return false;
}

void Unit::CollideWith(GameObject* Instance)
{

}

void Unit::Heal( float Value )
{
	gHealth.first	+=	Value;
	gHealth.first	=	( gHealth.first >= gHealth.second ) ? gHealth.second : gHealth.first;
}