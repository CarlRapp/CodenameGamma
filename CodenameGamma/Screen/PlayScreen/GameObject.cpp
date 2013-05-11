#include "GameObject.h"


GameObject::GameObject(void)
{
	m_ModelInstance = NULL;
	m_QuadTreeType = NULL;

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

	SetScale(1.0f);

	SetState( Alive );

	SetTeam(Team2);
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

	pos += vel * deltaTime;

	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, pos);

	XMFLOAT3 min = XMFLOAT3(0, 0, 0);
	XMFLOAT3 max = XMFLOAT3(4000, 4000, 4000);

#pragma region WALL
	if (newPos.x < min.x)
	{
		newPos.x = min.x;
		gVelocity.x = abs(gVelocity.x);
	}
	if (newPos.x > max.x)
	{
		newPos.x = max.x;
		gVelocity.x = -abs(gVelocity.x);
	}

	if (newPos.y < min.y)
	{
		newPos.y = min.y;
		gVelocity.y = abs(gVelocity.y);
	}
	if (newPos.y > max.y)
	{
		newPos.y = max.y;
		gVelocity.y = -abs(gVelocity.y);
	}

	if (newPos.z < min.z)
	{
		newPos.z = min.z;
		gVelocity.z = abs(gVelocity.z);
	}
	if (newPos.z > max.z)
	{
		newPos.z = max.z;
		gVelocity.z = -abs(gVelocity.z);
	}
#pragma endregion Wallbounce

	if (m_ModelInstance)
		m_ModelInstance->Update(deltaTime);

	MoveTo(newPos);

	//Move(XMFLOAT3(gVelocity.x * deltaTime, 0, gVelocity.z * deltaTime));



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
void GameObject::SetScale(float Scale)
{
	gScaleInFloat = Scale;
	XMStoreFloat4x4(&gScale, XMMatrixScaling(Scale, Scale, Scale));
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

bool GameObject::PlayAnimation(string name)
{
	if (m_ModelInstance)
	{
		if (m_ModelInstance->GetModel()->SkinnedData.HasAnimation(name))
		{
			m_ModelInstance->ClipName  = name;
			m_ModelInstance->TimePos   = 0.0;
			m_ModelInstance->Animating = true;
			return true;
		}
	}
	return false;
}

string GameObject::CurrentAnimationOrPose()
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->ClipName;
	}
	return "";
}

bool GameObject::UsePose(string name)
{
	if (m_ModelInstance)
	{
		if (m_ModelInstance->GetModel()->SkinnedData.HasPose(name))
		{
			m_ModelInstance->ClipName = name;
			m_ModelInstance->Animating = false;			
			m_ModelInstance->UpdatePose();
			return true;
		}
	}
	return false;
}

bool GameObject::Intersects(const GameObject* A, const GameObject* B, vector<CollisionData>& CD)
{
	if (A == B)
		return false;

	bool result = false;

	BoundingSphere As = A->m_QuadTreeType->GetQuadTreeData().Current;
	BoundingSphere Bs = B->m_QuadTreeType->GetQuadTreeData().Current;


	if (As.Intersects(Bs))
	{

		if (A->m_ModelInstance != NULL && B->m_ModelInstance != NULL)
		{

			BoundingOrientedBox Ab = A->m_ModelInstance->GetBoundingOrientedBox();
			BoundingOrientedBox Bb = B->m_ModelInstance->GetBoundingOrientedBox();

			if (Ab.Intersects(Bb))
			{
				if (!A->m_ModelInstance->m_BoneBoxes.empty())
				{
					//for each (BoundingOrientedBox Abb in A->m_ModelInstance->m_BoneBoxes)
					for (int a = 0; a < A->m_ModelInstance->m_BoneBoxes.size(); ++a)
					{
						BoundingOrientedBox Abb = A->m_ModelInstance->m_BoneBoxes[a];
						if (!B->m_ModelInstance->m_BoneBoxes.empty())
						{
							//for each (BoundingOrientedBox Bbb in B->m_ModelInstance->m_BoneBoxes)
							for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
							{
								BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
								if (Abb.Intersects(Bbb))
								{
									//cout << A->m_ModelInstance->GetBoneName(a) << " hit " << B->m_ModelInstance->GetBoneName(b) << endl;
									CollisionData cd;
									cd.A = HitBox(A->m_ModelInstance->GetBoneName(a), Abb);
									cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);
									CD.push_back(cd);
										/*
									HitboxA.name = A->m_ModelInstance->GetBoneName(a);
									HitboxA.box	 = Abb;

									HitboxB.name = B->m_ModelInstance->GetBoneName(b);
									HitboxB.box  = Bbb;
									*/
									result = true;
								}									
							}
						}
						else
						{
							if (Abb.Intersects(Bb))
							{		
								//cout << A->m_ModelInstance->GetBoneName(a) << " hit Body" << endl;

								CollisionData cd;
								cd.A = HitBox(A->m_ModelInstance->GetBoneName(a), Abb);
								cd.B = HitBox("Body", Bb);
								CD.push_back(cd);

								/*
								HitboxA.name = A->m_ModelInstance->GetBoneName(a);
								HitboxA.box	 = Abb;

								HitboxB.name = "Body";
								HitboxB.box  = Bb;
								*/
								result = true;
							}
						}
					}
				}

				else if (!B->m_ModelInstance->m_BoneBoxes.empty())
				{
					//for each (BoundingOrientedBox Bbb in B->m_ModelInstance->m_BoneBoxes)
					for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
					{
						BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
						if (Ab.Intersects(Bbb))
						{
							//cout << "Body hit " << B->m_ModelInstance->GetBoneName(b) << endl;
							CollisionData cd;
							cd.A = HitBox("Body", Ab);
							cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);
							CD.push_back(cd);
							/*
							HitboxA.name = "Body";
							HitboxA.box	 = Ab;

							HitboxB.name = B->m_ModelInstance->GetBoneName(b);
							HitboxB.box  = Bbb;	
							*/
							result = true;
						}
					}
				}

				else
				{
					//cout << "Body hit Body" << endl;
					CollisionData cd;
					cd.A = HitBox("Body", Ab);
					cd.B = HitBox("Body", Bb);
					CD.push_back(cd);
					/*
					HitboxA.name = "Body";
					HitboxA.box	 = Ab;

					HitboxB.name = "Body";
					HitboxB.box  = Bb;		
					*/
					result = true;
				}
			}
		}

		else if (A->m_ModelInstance != NULL && B->m_ModelInstance == NULL)
		{
			BoundingOrientedBox Ab = A->m_ModelInstance->GetBoundingOrientedBox();
			return Ab.Intersects(Bs);
		}

		else if (A->m_ModelInstance == NULL && B->m_ModelInstance != NULL)
		{
			BoundingOrientedBox Bb = B->m_ModelInstance->GetBoundingOrientedBox();
			return Bb.Intersects(As);
		}
		else
			return true;
	}
	return result;
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

	if (m_ModelInstance)
	{
		//m_ModelInstance->m_World		=	gWorld;		

		//m_ModelInstance->m_Scale		=	gScaleInFloat;
		XMVECTOR Quaternion = XMQuaternionRotationMatrix(XMLoadFloat4x4(&gRotation));
		XMStoreFloat4(&m_ModelInstance->m_Rotation, Quaternion);
		m_ModelInstance->m_Translation	=	gPosition;

		//m_ModelInstance->m_WorldInverseTranspose	=	gWorldInverseTranspose;
		m_ModelInstance->SetWorld(gWorld, gWorldInverseTranspose, gScaleInFloat);
		m_QuadTreeType->Update();
	}
}

void GameObject::CollideWith(GameObject* Instance, vector<CollisionData> CD) { } 