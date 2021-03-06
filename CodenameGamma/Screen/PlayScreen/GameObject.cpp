#include "GameObject.h"


GameObject::GameObject(void)
{
	m_ModelInstance = NULL;
	m_QuadTreeType = NULL;

	gPosition			=	XMFLOAT3(0, 0, 0);
	gVelocity			=	XMFLOAT3(0, 0, 0);
	gAcceleration		=	XMFLOAT3(0, 0, 0); 

	XMStoreFloat4(&gQuaternation, XMQuaternionIdentity());

	gMoveDirection		=	None;

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
		XMMatrixRotationQuaternion(	XMLoadFloat4(&gQuaternation) )
	);

	SetScale(1.0f);

	SetState( Alive );

	SetTeam(Team2);

	gDeathTime = 0.0f;
}

GameObject::~GameObject(void)
{
}

void GameObject::Update(float deltaTime, Terrain* terrain)
{
	if ( WantsRemove() || terrain == 0)
		return;
	
	AnimateThisUpdate = false;
	if (m_ModelInstance)
		m_ModelInstance->Update(deltaTime, 1.0f, AnimateThisUpdate);

	if ( gState == Dying )
	{
		gDeathTimer += deltaTime;
		
		if (gDeathTimer >= gDeathTime)
			gState = Dead;
		return;
	}

	float heigth = gPosition.y - terrain->GetHeight(gPosition.x, gPosition.z);

	XMVECTOR pos = XMLoadFloat3(&gPosition);
	XMVECTOR vel = XMLoadFloat3(&gVelocity);
	//XMVECTOR vel = XMLoadFloat3(&XMFLOAT3(0, 0, 0));

	pos += vel * deltaTime;

	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, pos);

	XMFLOAT3 min = XMFLOAT3(0, 0, 0);
	XMFLOAT3 max = XMFLOAT3(terrain->GetDimensions().x, 500, terrain->GetDimensions().y);

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

	MoveTo(newPos);

	//Move(XMFLOAT3(gVelocity.x * deltaTime, 0, gVelocity.z * deltaTime));
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

void GameObject::SetWorld(XMFLOAT4X4 translation, XMFLOAT4X4 scale, XMFLOAT4X4 rotation)
{
	gTranslation = translation;
	gScale = scale;
	gRotation = rotation;

	UpdateWorld(true);
}

#pragma region Rotation Methods
/*
//	Will rotate the object
//	to Rotations value
void GameObject::SetRotation(XMFLOAT3 Rotation)
{
	XMStoreFloat4x4(&gRotation, XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z));

	UpdateWorld(true);
}
*/

//	Will rotate the object
//	to Rotations value
void GameObject::SetRotation(XMFLOAT4 Rotation)
{
	gQuaternation = Rotation;
	XMStoreFloat4x4(&gRotation, XMMatrixRotationQuaternion( XMLoadFloat4(&Rotation) ));

	UpdateWorld(true);
}

//	Will add Delta to
//	current rotation
void GameObject::AddRotation(XMFLOAT4 Delta)
{
	XMVECTOR start		= XMLoadFloat4(&gQuaternation);
	XMVECTOR add		= XMLoadFloat4(&Delta);
	XMVECTOR resultV	= XMQuaternionMultiply(start, add);

	XMFLOAT4 result;

	XMStoreFloat4(&result, resultV);

	GameObject::SetRotation(result);
}

//	Will turn towards
//	the position, y-axis
void GameObject::LookAtXZ(XMFLOAT3 Position)
{
	float	dX	=	(gPosition.x - Position.x);
	float	dZ	=	(Position.z - gPosition.z);

	float dAngle	=	atan2(dZ, dX);

	XMVECTOR QuatV = XMQuaternionRotationRollPitchYaw(0, dAngle + PI * 0.5f, 0);
	XMFLOAT4 Quat;

	XMStoreFloat4(&Quat, QuatV);
	SetRotation( Quat );
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

bool GameObject::PlayingAnimation(string name)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->PlayingAnimation(name);
	}
	return false;
}

bool GameObject::LoopAnimation(string name)
{
	if (m_ModelInstance)
	{
		if (!m_ModelInstance->PlayingAnimation(name))
			return m_ModelInstance->PlayAnimation(name, true);
		/*
		if (m_ModelInstance->GetModel()->SkinnedData.HasAnimation(name))
		{
			m_ModelInstance->ClipName  = name;
			m_ModelInstance->TimePos   = 0.0;
			m_ModelInstance->Animating = true;
			return true;
		}*/
	}
	return false;
}

bool GameObject::PlayAnimation(string name)
{
	if (m_ModelInstance)
	{
		if (!m_ModelInstance->PlayingAnimation(name))
			return m_ModelInstance->PlayAnimation(name, false);
	}
	return false;
}

bool GameObject::LoopAnimationAfter(string current, string next)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->PlayAnimationAfter(current, next, true);
	}
	return false;
}

bool GameObject::PlayAnimationAfter(string current, string next)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->PlayAnimationAfter(current, next, false);
	}
	return false;
}

void GameObject::StopAnimation(string name)
{
	if (m_ModelInstance)
	{
		m_ModelInstance->StopAnimation(name);
	}
}

void GameObject::StopAllAnimations()
{
	if (m_ModelInstance)
	{
		m_ModelInstance->StopAllAnimations();
	}
}

bool GameObject::SetAnimationSpeed(string name, float speed)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->SetAnimationSpeed(name, speed);
	}
	return false;
}

bool GameObject::SetAnimationProgress(string name, float progress)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->SetAnimationProgress(name, progress);
	}
	return false;
}

float GameObject::GetAnimationSpeed(string name)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->GetAnimationSpeed(name);
	}
	return 0.0f;
}

float GameObject::GetAnimationProgress(string name)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->GetAnimationProgress(name);
	}
	return 1.0f;
}

float GameObject::GetAnimationTime(string name)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->GetAnimationTime(name);
	}
	return 1.0f;
}

bool GameObject::GetJointPosition(string name, XMFLOAT3& pos)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->GetJointPosition(name, pos);
	}
	return false;
}

bool GameObject::GetJointDirection(string name, XMFLOAT3& dir)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->GetJointDirection(name, dir);
	}
	return false;
}

bool GameObject::GetJointRotation(string name, XMFLOAT4& rot)
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->GetJointRotation(name, rot);
	}
	return false;
}

void GameObject::Kill()
{
	SetState( Dying );
	
	gDeathTimer = 0.0f;
}

/*
string GameObject::CurrentAnimationOrPose()
{
	if (m_ModelInstance)
	{
		return m_ModelInstance->ClipName;
	}
	return "";
}
*/
/*
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
*/
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

		//ModelInstance->m_WorldInverseTranspose	=	gWorldInverseTranspose;
		m_ModelInstance->SetWorld(gWorld, gWorldInverseTranspose, gScaleInFloat);
		m_QuadTreeType->Update();
	}
}

void GameObject::CollideWith(GameObject* Instance, vector<CollisionData> CD) { } 


//Collision
/*
bool GameObject::Intersects(const GameObject* A, const GameObject* B, vector<CollisionData>& CD)
{
	return BoxVsBox(A, B, CD);
}*/

bool GameObject::SphereVsSphere(const GameObject* A, const GameObject* B)
{
	if (A == B)
		return false;
	BoundingSphere As = A->m_QuadTreeType->GetQuadTreeData().Current;
	BoundingSphere Bs = B->m_QuadTreeType->GetQuadTreeData().Current;
	if (As.Intersects(Bs))
		return true;
	return false;
}

bool GameObject::BoxVsBox(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD)
{
	if (A == B)
		return false;

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
				CollisionData cd;
				cd.A = HitBox("Body", Ab);
				cd.B = HitBox("Body", Bb);

				if (SwapCD)
				{
					HitBox temp = cd.A;
					cd.A = cd.B;
					cd.B = temp;
				}
				CD.push_back(cd);
				return true;
			}
		}
	}
	return false;
}

bool GameObject::BoneVsBone(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD)
{
	if (A == B)
		return false;

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
				A->m_ModelInstance->UpdateBoxes();
				B->m_ModelInstance->UpdateBoxes();
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
									CollisionData cd;
									cd.A = HitBox(A->m_ModelInstance->GetBoneName(a), Abb);
									cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);

									if (SwapCD)
									{
										HitBox temp = cd.A;
										cd.A = cd.B;
										cd.B = temp;
									}
									CD.push_back(cd);
									return true;
								}									
							}
						}
						else
						{
							if (Abb.Intersects(Bb))
							{		
								CollisionData cd;
								cd.A = HitBox(A->m_ModelInstance->GetBoneName(a), Abb);
								cd.B = HitBox("Body", Bb);

								if (SwapCD)
								{
									HitBox temp = cd.A;
									cd.A = cd.B;
									cd.B = temp;
								}
								CD.push_back(cd);
								return true;
							}
						}
					}
				}

				else if (!B->m_ModelInstance->m_BoneBoxes.empty())
				{
					for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
					{
						BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
						if (Ab.Intersects(Bbb))
						{
							CollisionData cd;
							cd.A = HitBox("Body", Ab);
							cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);

							if (SwapCD)
							{
								HitBox temp = cd.A;
								cd.A = cd.B;
								cd.B = temp;
							}
							CD.push_back(cd);
							return true;
						}
					}
				}
				else
				{
					CollisionData cd;
					cd.A = HitBox("Body", Ab);
					cd.B = HitBox("Body", Bb);

					if (SwapCD)
					{
						HitBox temp = cd.A;
						cd.A = cd.B;
						cd.B = temp;
					}
					CD.push_back(cd);
					return true;
				}
			}
		}
	}
	return false;
}

bool GameObject::AllBonesVsAllBones(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD)
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
				A->m_ModelInstance->UpdateBoxes();
				B->m_ModelInstance->UpdateBoxes();
				if (!A->m_ModelInstance->m_BoneBoxes.empty())
				{
					for (int a = 0; a < A->m_ModelInstance->m_BoneBoxes.size(); ++a)
					{
						BoundingOrientedBox Abb = A->m_ModelInstance->m_BoneBoxes[a];
						if (!B->m_ModelInstance->m_BoneBoxes.empty())
						{
							for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
							{
								BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
								if (Abb.Intersects(Bbb))
								{
									CollisionData cd;
									cd.A = HitBox(A->m_ModelInstance->GetBoneName(a), Abb);
									cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);					

									if (SwapCD)
									{
										HitBox temp = cd.A;
										cd.A = cd.B;
										cd.B = temp;
									}
									CD.push_back(cd);
									result = true;
								}									
							}
						}
						else
						{
							if (Abb.Intersects(Bb))
							{		
								CollisionData cd;
								cd.A = HitBox(A->m_ModelInstance->GetBoneName(a), Abb);
								cd.B = HitBox("Body", Bb);

								if (SwapCD)
								{
									HitBox temp = cd.A;
									cd.A = cd.B;
									cd.B = temp;
								}
								CD.push_back(cd);
								result = true;
							}
						}
					}
				}

				else if (!B->m_ModelInstance->m_BoneBoxes.empty())
				{
					for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
					{
						BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
						if (Ab.Intersects(Bbb))
						{
							CollisionData cd;
							cd.A = HitBox("Body", Ab);
							cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);

							if (SwapCD)
							{
								HitBox temp = cd.A;
								cd.A = cd.B;
								cd.B = temp;
							}
							CD.push_back(cd);
							result = true;
						}
					}
				}

				else
				{
					CollisionData cd;
					cd.A = HitBox("Body", Ab);
					cd.B = HitBox("Body", Bb);

					if (SwapCD)
					{
						HitBox temp = cd.A;
						cd.A = cd.B;
						cd.B = temp;
					}
					CD.push_back(cd);
					result = true;
				}
			}
		}
	}
	return result;
}

bool GameObject::SphereVsBox(const GameObject* A, const GameObject* B)
{
	if (A == B)
		return false;
	BoundingSphere As = A->m_QuadTreeType->GetQuadTreeData().Current;
	BoundingSphere Bs = B->m_QuadTreeType->GetQuadTreeData().Current;
	if (As.Intersects(Bs))
	{
		if (B->m_ModelInstance != NULL)
		{
			BoundingOrientedBox Bb = B->m_ModelInstance->GetBoundingOrientedBox();
			if (As.Intersects(Bb))
				return true;
		}
	}
	return false;
}

bool GameObject::SphereVsBone(const GameObject* A, const GameObject* B)
{
	if (A == B)
		return false;
	BoundingSphere As = A->m_QuadTreeType->GetQuadTreeData().Current;
	BoundingSphere Bs = B->m_QuadTreeType->GetQuadTreeData().Current;
	if (As.Intersects(Bs))
	{
		if (B->m_ModelInstance != NULL)
		{
			BoundingOrientedBox Bb = B->m_ModelInstance->GetBoundingOrientedBox();
			if (As.Intersects(Bb))
			{
				B->m_ModelInstance->UpdateBoxes();
				if (!B->m_ModelInstance->m_BoneBoxes.empty())
				{
					for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
					{
						BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
						if (As.Intersects(Bbb))
						{
							return true;
						}
					}
				}
				else
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool GameObject::BoxVsBone(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD)
{
	if (A == B)
		return false;

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
				B->m_ModelInstance->UpdateBoxes();
				if (!B->m_ModelInstance->m_BoneBoxes.empty())
				{
					for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
					{
						BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
						if (Ab.Intersects(Bbb))
						{
							CollisionData cd;
							cd.A = HitBox("Body", Ab);
							cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);

							if (SwapCD)
							{
								HitBox temp = cd.A;
								cd.A = cd.B;
								cd.B = temp;
							}
							CD.push_back(cd);
							return true;
						}
					}
				}
				else
				{
					CollisionData cd;
					cd.A = HitBox("Body", Ab);
					cd.B = HitBox("Body", Bb);

					if (SwapCD)
					{
						HitBox temp = cd.A;
						cd.A = cd.B;
						cd.B = temp;
					}
					CD.push_back(cd);
					return true;
				}
			}
		}
	}
	return false;
}

bool GameObject::BoxVsAllBones(const GameObject* A, const GameObject* B, vector<CollisionData>& CD, bool SwapCD)
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
				B->m_ModelInstance->UpdateBoxes();
				if (!B->m_ModelInstance->m_BoneBoxes.empty())
				{
					for (int b = 0; b < B->m_ModelInstance->m_BoneBoxes.size(); ++b)
					{
						BoundingOrientedBox Bbb = B->m_ModelInstance->m_BoneBoxes[b];
						if (Ab.Intersects(Bbb))
						{
							CollisionData cd;
							cd.A = HitBox("Body", Ab);
							cd.B = HitBox(B->m_ModelInstance->GetBoneName(b), Bbb);

							if (SwapCD)
							{
								HitBox temp = cd.A;
								cd.A = cd.B;
								cd.B = temp;
							}
							CD.push_back(cd);
							result = true;
						}
					}
				}

				else
				{
					CollisionData cd;
					cd.A = HitBox("Body", Ab);
					cd.B = HitBox("Body", Bb);

					if (SwapCD)
					{
						HitBox temp = cd.A;
						cd.A = cd.B;
						cd.B = temp;
					}
					CD.push_back(cd);
					result = true;
				}
			}
		}
	}
	return result;
}