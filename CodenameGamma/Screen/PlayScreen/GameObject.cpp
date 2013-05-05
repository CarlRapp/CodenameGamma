#include "GameObject.h"


GameObject::GameObject(void)
{
	gPosition			=	XMFLOAT3(0, 0, 0);
	gVelocity			=	XMFLOAT3(0, 0, 0);
	gAcceleration		=	XMFLOAT3(0, 0, 0); 
	gRotationInFloat	=	XMFLOAT3(0, 0, 0);

	XMStoreFloat4x4(&gScale, XMMatrixScaling(1, 1, 1));
	XMStoreFloat4x4(
		&gTranslation,
		XMMatrixTranslation( 
			(gPosition.x), 
			(gPosition.y), 
			(gPosition.z)
		)
	);
	XMStoreFloat4x4(
		&gRotation, 
		XMMatrixRotationRollPitchYaw(
			gRotationInFloat.y, 
			gRotationInFloat.x, 
			gRotationInFloat.z
		)
	);
	UpdateWorld(true);

	SetScale(XMFLOAT3(1,1,1));

	SetState( Alive );

	SetTeam(Team2);

	m_ModelInstance = NULL;
	m_QuadTreeType = NULL;
}


GameObject::~GameObject(void)
{
}

bool GameObject::Update(float deltaTime, Terrain* terrain)
{
	if ( !IsAlive() )
		return true;

	float heigth = gPosition.y - terrain->GetHeight(gPosition.x, gPosition.z);

	XMVECTOR pos = XMLoadFloat3(&gPosition);
	XMVECTOR vel = XMLoadFloat3(&gVelocity);
	//XMVECTOR vel = XMLoadFloat3(&XMFLOAT3(0, 0, 0));

	XMVECTOR mov = pos;
	pos += vel * deltaTime;

	//D3DXVECTOR3 move = m_Position;
	//m_Position += m_Velocity * deltaTime;

	XMStoreFloat3(&gPosition, pos);

	XMFLOAT3 min = XMFLOAT3(0, 0, 0);
	XMFLOAT3 max = XMFLOAT3(4000, 4000, 4000);

	if (gPosition.x < min.x)
	{
		gPosition.x = min.x;
		gVelocity.x = abs(gVelocity.x);
	}
	if (gPosition.x > max.x)
	{
		gPosition.x = max.x;
		gVelocity.x = -abs(gVelocity.x);
	}

	if (gPosition.y < min.y)
	{
		gPosition.y = min.y;
		gVelocity.y = abs(gVelocity.y);
	}
	if (gPosition.y > max.y)
	{
		gPosition.y = max.y;
		gVelocity.y = -abs(gVelocity.y);
	}

	if (gPosition.z < min.z)
	{
		gPosition.z = min.z;
		gVelocity.z = abs(gVelocity.z);
	}
	if (gPosition.z > max.z)
	{
		gPosition.z = max.z;
		gVelocity.z = -abs(gVelocity.z);
	}

	pos = XMLoadFloat3(&gPosition);
	mov = pos - mov;
	

	
	bool moved = XMVector3NotEqual(mov, XMVectorZero());

	if (moved && m_ModelInstance)
	{
		Move(XMFLOAT3(gVelocity.x * deltaTime, 0, gVelocity.z * deltaTime));

		m_ModelInstance->m_World	=	gWorld;
		m_ModelInstance->m_WorldInverseTranspose	=	gWorldInverseTranspose;

	}

	m_QuadTreeType->Update();
	m_ModelInstance->m_World	=	gWorld;
	m_ModelInstance->m_WorldInverseTranspose	=	gWorldInverseTranspose;

	return !MathHelper::BoundingSphereEqual(m_QuadTreeType->GetQuadTreeData().Old, m_QuadTreeType->GetQuadTreeData().Current);
}

void GameObject::SetModelInstance(ModelInstance *modelInstance)
{ 
	SAFE_DELETE(m_ModelInstance);
	SAFE_DELETE(m_QuadTreeType);

	m_ModelInstance = modelInstance; 		
	m_QuadTreeType = new QuadTreeTypeModel(m_ModelInstance);
}

float GameObject::GetRadius()
{
	if (m_ModelInstance)
		return m_ModelInstance->GetBoundingSphere().Radius;
	else
		return 0;
}

#pragma region Custom Get Methods
//	Will return the specific Float3
//	according to Value
XMFLOAT3 GameObject::GetFloat3Value(GOFloat3Value Value)
{
	switch ( Value )
	{
	case Position:
		return gPosition;
		break;

	case Velocity:
		return gVelocity;
		break;

	case Acceleration:
		return gAcceleration;
		break;

	case Rotations:
		return gRotationInFloat;
		break;
	case Direction:
		XMFLOAT3 result;
		XMVECTOR direction = XMLoadFloat3(&XMFLOAT3(0,0,-1));
		XMMATRIX rotation  = XMLoadFloat4x4(&GetFloat4x4Value( Rotation ));
		direction = XMVector3TransformCoord(direction, rotation);
		XMStoreFloat3(&result, direction);
		return result;
		break;
	}

	return XMFLOAT3(0, 0, 0);
}

//	Will return the specific Float4x4
//	according to Value
XMFLOAT4X4 GameObject::GetFloat4x4Value(GOFloat4x4Value Value)
{
	switch ( Value )
	{
	case Rotation:
		return gRotation;
		break;

	case Translation:
		return gTranslation;
		break;

	case Scale:
		return gScale;
		break;

	case World:
		return gWorld;
		break;

	case WorldInverseTranspose:
		return gWorldInverseTranspose;
		break;
	}

	return XMFLOAT4X4();
}
#pragma endregion

#pragma region Rotation Methods
//	Will rotate the object
//	to Rotations value
void GameObject::SetRotation(XMFLOAT3 Rotation)
{
	XMStoreFloat4x4(&gRotation, XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z));
	gRotationInFloat.x	=	Rotation.x;
	gRotationInFloat.y	=	Rotation.y;
	gRotationInFloat.z	=	Rotation.z;

	UpdateWorld(true);
}

//	Will add Delta to
//	current rotation
void GameObject::AddRotation(XMFLOAT3 Delta)
{
	SetRotation(
		XMFLOAT3(
			Delta.x + gRotationInFloat.x,
			Delta.y + gRotationInFloat.y,
			Delta.z + gRotationInFloat.z
		)
	);
}

//	Will turn towards
//	the position, y-axis
void GameObject::LookAt(XMFLOAT3 Position)
{
	float	dX	=	(Position.x - gPosition.x);
	float	dZ	=	(Position.z - gPosition.z);

	float dAngle	=	atan2(dX, dZ);

	SetRotation(XMFLOAT3(0, dAngle, 0));
}
#pragma endregion

#pragma region Scale Methods
void GameObject::SetScale(XMFLOAT3 Scale)
{
	XMStoreFloat4x4(&gScale, XMMatrixScaling(Scale.x, Scale.y, Scale.z));
	UpdateWorld(true);
}
#pragma endregion

#pragma region Move Methods
//	Will move the object 
//	Delta units away
void GameObject::Move(XMFLOAT3 Delta)
{
	MoveTo(
		XMFLOAT3(
			gPosition.x + Delta.x,
			gPosition.y + Delta.y,
			gPosition.z + Delta.z
		)
	);
}
//	Will move the object
//	to Location
void GameObject::MoveTo(XMFLOAT3 Location)
{
	XMStoreFloat4x4(
		&gTranslation,
		XMMatrixTranslation( 
			(Location.x), 
			(Location.y), 
			(Location.z)
		)
	);
	gPosition	=	Location;
	UpdateWorld(false);
}
#pragma endregion

#pragma region Velocity Methods
void GameObject::SetVelocity(XMFLOAT3 Velocity)
{
	gVelocity	=	Velocity;
}
#pragma endregion

void GameObject::SetState(GOState Value)
{
	gState	=	Value;
}

#pragma region Team Methods
void GameObject::SetTeam(GOTeam Team)
{
	gTeam	=	Team;
}

GOTeam GameObject::GetTeam()
{
	return gTeam;
}

bool GameObject::IsEnemy(GameObject* Instance)
{
	return ( gTeam != Neutral && Instance->gTeam != Neutral && gTeam != Instance->gTeam );
}
#pragma endregion


//	Updates the world matrix,
//	and if bool is true will also
//	calculate the InverseTranspose
//	of world.
void GameObject::UpdateWorld(bool UpdateInverseTranspose)
{
	XMMATRIX	tWorld	=	XMLoadFloat4x4(&gScale) * XMLoadFloat4x4(&gRotation) * XMLoadFloat4x4(&gTranslation);
	XMStoreFloat4x4(&gWorld, tWorld);

	if ( UpdateInverseTranspose )
		XMStoreFloat4x4(&gWorldInverseTranspose, MathHelper::InverseTranspose(XMLoadFloat4x4(&gWorld)));
}

void GameObject::CollideWith(GameObject* Instance) { } 