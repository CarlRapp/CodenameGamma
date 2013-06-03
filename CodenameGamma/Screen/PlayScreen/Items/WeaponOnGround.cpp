#include "WeaponOnGround.h"


WeaponOnGround::WeaponOnGround()
{
	SetState( Dead );
}

WeaponOnGround::WeaponOnGround( Weapon* Instance )
{
	//	Null the light pointer
	gPointLight	=	0;

	//	Set the initial values on offset, cooldown and
	//	a random "life span" that will give it an offset
	//	in position methods, making every new drop look
	//	unique
	gCooldown	=	0.5f;
	gOffset		=	XMFLOAT3( 0, 3, 0 );
	gTimeSpan	=	MathHelper::RandF( 0.0f, 8000.0f );

	//	Set the current modelinstance
	//	and scale it to look better
	SetModelInstance( Instance->GetWOGModelInstance() );
	SetScale( 2 );

	//	Update the instance
	//	to hidden so it will
	//	be removed from the tree.
	gWeapon	=	Instance;
	gWeapon->SetState( Hidden );


	//	Give it a random rotation

	XMVECTOR QuatV = XMQuaternionRotationRollPitchYaw(0, MathHelper::RandF( -PI, PI ), 0);
	XMFLOAT4 Quat;

	XMStoreFloat4(&Quat, QuatV);
	SetRotation( Quat );
}

WeaponOnGround::~WeaponOnGround()
{
	delete	gPointLight;
}

void WeaponOnGround::Update(float DeltaTime, Terrain* terrain)
{
	if ( !IsAlive() )
		return;

	XMFLOAT3	newPos	=	GetFloat3Value( Position );

	if ( !gPointLight )
	{
		gPointLight	=	new PointLight();
		gPointLight->GetGPULight()->Color		=	XMFLOAT4( 2.0f, 1.5f, 0.0f, 0.0f );
		gPointLight->GetGPULight()->Position	=	newPos;
		gPointLight->GetGPULight()->Range		=	33.333f * 0.40f;
		gPointLight->GetGPULight()->HasShadow	=	false;

		AddLight( gPointLight );
	}

	gTimeSpan	+=	DeltaTime;
	
	newPos.y	=	gOffset.y + ( gOffset.y - 2 ) * sin( 8 * gTimeSpan );

	gWeapon->MoveTo( newPos );
	MoveTo( newPos );


	XMVECTOR QuatV = XMQuaternionRotationRollPitchYaw(0, DeltaTime, 0);
	XMFLOAT4 Quat;

	XMStoreFloat4(&Quat, QuatV);
	AddRotation( Quat );

	newPos.y	-=	1.0f;

	if ( gPointLight != 0 )
		gPointLight->GetGPULight()->Position	=	newPos;

	if ( gCooldown > 0 )
	{
		gCooldown	-=	DeltaTime;
		gCooldown	=	( gCooldown < 0 ) ? 0 : gCooldown;
	}

	Item::Update( DeltaTime, terrain );
}


/*
void WeaponOnGround::SetRotation(XMFLOAT4 Rotation)
{

	XMVECTOR rotationV	= XMLoadFloat4( &Rotation );
	XMVECTOR offsetV	= XMQuaternionIdentity();
	
	if (gWeapon)
	{
		XMFLOAT3 handPos;
		XMFLOAT3 pipePos;

		if (gWeapon->GetJointPosition("RightHand", handPos) && gWeapon->GetJointPosition("Pipe", pipePos))
		{
			XMVECTOR V1 = XMLoadFloat3(&XMFLOAT3(0,0,1));
			XMVECTOR V2 = XMVector3Normalize( XMLoadFloat3(&pipePos) - XMLoadFloat3(&handPos) );
			
			if (!XMVector3Equal(V1, V2))
			{


				XMVECTOR axis = XMVector3Cross(V1, V2);
				float angle = -XMVectorGetX( XMVector3AngleBetweenNormals(V1, V2) );

				offsetV = XMQuaternionRotationAxis( axis, angle );
			}
		}
	}
	
	rotationV = XMQuaternionMultiply(offsetV, rotationV);
	XMStoreFloat4(&Rotation, rotationV);
	GameObject::SetRotation(Rotation);
}
*/

void WeaponOnGround::OnPickUp(Unit* Instance)
{
	if ( gCooldown != 0 )
		return;

	if ( !IsOfType<PlayerUnit>(Instance) )
		return;

	
	PlayerUnit*	pUnit	=	(PlayerUnit*)Instance;

	if (pUnit->IsAiming())
		return;

	if (!pUnit->HasSecondaryWeapon())
	{
		SetScale( 1 );
		pUnit->PickupWeapon( gWeapon );
	
		SetState( Dead );
		RemoveLight( gPointLight );

		gWeapon->SetState( Alive );
		//AddGameObject( gWeapon );
	}
}