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
	gPrimaryWeapon	=	NULL;
	gSecondaryWeapon=	NULL;

	gWalkSpeed = 1.0f;
	gRunSpeed  = 1.0f;

	gMultipliers[0]	=	1.0f;
	gMultipliers[1]	=	1.0f;
	gMultipliers[2]	=	1.0f;

	gDeathSound	=	"";
}

Unit::~Unit(void)
{
}

void Unit::DropWeapon()
{
	if ( gCurrentWeapon && gWeaponState == Hold )
	{
		if( !gCurrentWeapon->IsDropable() || gCurrentWeapon->IsReloading() )
			return;
		if( gCurrentWeapon->GetState() == Reloading )
			gCurrentWeapon->CancelReload();

		WeaponOnGround*	tWoG	=	new WeaponOnGround( gCurrentWeapon );
		tWoG->MoveTo( gCurrentWeapon->GetFloat3Value( Position ) );

		//tWoG->SetRotation( GetQuaternation() );

		AddGameObject( tWoG );
		tWoG->Update(0, 0);

		gSecondaryWeapon=	0;
		gCurrentWeapon	=	gPrimaryWeapon;
		gCurrentWeapon->SetState( Alive );
		AddGameObject( gCurrentWeapon );
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
		SetWeaponState( Hold );
		DropWeapon();
		Kill();
		StopAllAnimations();
		PlayAnimation("Death");
		PlayAnimationAfter("Death", "Dead");
		SoundManager::GetInstance()->Play( gDeathSound, SFX, false );

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

		if (fabs(angle) <= PI * 0.25f)
			gMoveDirection = Forward;

		else if (fabs(angle) > PI * 0.75f)
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
		XMFLOAT4 handRot;
		
		XMVECTOR pos = XMLoadFloat3(&GetFloat3Value( Position ));
		XMVECTOR dir = XMLoadFloat3(&GetFloat3Value( Direction ));
		XMVECTOR lv = XMVector3Length(dir);

		bool gotJointPos = false;
		bool gotJointRot = false;
		if (m_ModelInstance)
		{
			gotJointPos = GetJointPosition("RightHand", handPos);
			gotJointRot = GetJointRotation("RightHand", handRot);
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
		
		if (gotJointRot)
		{
			gCurrentWeapon->SetRotation( handRot );
		}
		else
			gCurrentWeapon->SetRotation( GetQuaternation() );
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
	if (!IsAlive())
		return;

	switch (newMoveState)
	{
	case Stand:
		if (gMoveState == Crouch)
		{
			float progress = 1.0f - GetAnimationProgress("Crouching");
			PlayAnimation("StandUp");
			LoopAnimationAfter("StandUp", "Pose");
			SetAnimationProgress("StandUp", progress);

			if (PlayingAnimation(GetAnimation("PutAway")))
				LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperStand"));
		}

		if (gMoveState != Stand)
		{
			if (!PlayingAnimation("StandUp"))
				LoopAnimation("Pose");	

			if (PlayingAnimation(GetAnimation("UpperWalk")) || PlayingAnimation(GetAnimation("UpperRun")))
			{
				StopAnimation(GetAnimation("UpperWalk"));
				StopAnimation(GetAnimation("UpperRun"));
				LoopAnimation(GetAnimation("UpperStand"));
			}

			if (PlayingAnimation(GetAnimation("PutAway")))
				LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperStand"));
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

		if (PlayingAnimation(GetAnimation("PutAway")))
				LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperWalk"));

		break;
	
	case Run:

		if (gMoveState != Run)
			BodyInSync = false;

		LoopAnimation("Run");

		if (PlayingAnimation("Run"))
		{			
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

			if (PlayingAnimation(GetAnimation("PutAway")))
				LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperRun"));
		}

		else
			LoopAnimation("Walk");		

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
	if (!IsAlive())
		return;

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

			//else if (PlayingAnimation(GetAnimation("DrawReloadPutAway")))
			//	PlayAnimationAfter(GetAnimation("DrawReloadPutAway"), GetAnimation("PutAway"));

			else
			{
				PlayAnimation(GetAnimation("PutAway"));
				SetAnimationProgress(GetAnimation("PutAway"), progress);
			}

			switch (gMoveState)
			{
			case Stand:
			case Crouch:
				LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperStand"));
				break;
			case Walk:
				LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperWalk"));
			case Run:
				LoopAnimationAfter(GetAnimation("PutAway"), GetAnimation("UpperRun"));
			}

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
	if (!IsAlive())
		return;

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


				switch (gMoveState)
				{
				case Stand:
				case Crouch:
					animationAfter = GetAnimation("UpperStand");
					break;
				case Walk:
					animationAfter = GetAnimation("UpperWalk");
				case Run:
					animationAfter = GetAnimation("UpperRun");
				}

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

bool Unit::FireWeapon(GameObject* Target)
{
	if (!IsAlive())
		return false;

	if ( gCurrentWeapon )
	{
		if (gWeaponState == Aim && !PlayingAnimation(GetAnimation("Draw")))
		{
			if (gCurrentWeapon->Fire( this, Target, gMultipliers[1] ))
			{
				PlayAnimation(GetAnimation("Shoot"));
				PlayAnimationAfter(GetAnimation("Shoot"), GetAnimation("Aim"));

				return true;
			}
		}
	}
	return false;
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

void Unit::ChangeWeapon( )
{
	if (!IsAlive())
		return;

	if( !gSecondaryWeapon || !gCurrentWeapon )
		return;
	if( gWeaponState != Hold )
		return;

	if( gCurrentWeapon->GetState() == Reloading || gCurrentWeapon->GetInfo().ReloadTime.first > 0 )
		gCurrentWeapon->CancelReload();

	if( gCurrentWeapon == gPrimaryWeapon )
	{
		gPrimaryWeapon->SetState( Hidden );
		gCurrentWeapon	=	gSecondaryWeapon;
		gCurrentWeapon->SetState( Alive );
		AddGameObject( gSecondaryWeapon );
	}
	else
	{
		gSecondaryWeapon->SetState( Hidden );
		gCurrentWeapon	=	gPrimaryWeapon;
		gCurrentWeapon->SetState( Alive );
		AddGameObject( gPrimaryWeapon );
	}
	SetWeaponState( Hold );
}

void Unit::PickupWeapon( Weapon* Instance )
{
	//	Pick up the weapon and change
	//	to it
	gSecondaryWeapon	=	Instance;
	ChangeWeapon();
}