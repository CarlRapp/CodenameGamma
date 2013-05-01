#include "GameObject.h"


GameObject::GameObject(void)
{
	m_Position = XMFLOAT3(0,0,0); 
	m_Velocity = XMFLOAT3(0,0,0); 
	m_ModelInstance = NULL;
	m_QuadTreeType = NULL;
}


GameObject::~GameObject(void)
{
}

bool GameObject::Update(float deltaTime, Terrain* terrain)
{
	float heigth = m_Position.y - terrain->GetHeight(m_Position.x, m_Position.z);

	XMVECTOR pos = XMLoadFloat3(&m_Position);
	XMVECTOR vel = XMLoadFloat3(&m_Velocity);

	XMVECTOR mov = pos;
	pos += vel * deltaTime;

	//D3DXVECTOR3 move = m_Position;
	//m_Position += m_Velocity * deltaTime;

	XMStoreFloat3(&m_Position, pos);

	XMFLOAT3 min = XMFLOAT3(0, 0, 0);
	XMFLOAT3 max = XMFLOAT3(4000, 4000, 4000);

	if (m_Position.x < min.x)
	{
		m_Position.x = min.x;
		m_Velocity.x = abs(m_Velocity.x);
	}
	if (m_Position.x > max.x)
	{
		m_Position.x = max.x;
		m_Velocity.x = -abs(m_Velocity.x);
	}

	if (m_Position.y < min.y)
	{
		m_Position.y = min.y;
		m_Velocity.y = abs(m_Velocity.y);
	}
	if (m_Position.y > max.y)
	{
		m_Position.y = max.y;
		m_Velocity.y = -abs(m_Velocity.y);
	}

	if (m_Position.z < min.z)
	{
		m_Position.z = min.z;
		m_Velocity.z = abs(m_Velocity.z);
	}
	if (m_Position.z > max.z)
	{
		m_Position.z = max.z;
		m_Velocity.z = -abs(m_Velocity.z);
	}

	pos = XMLoadFloat3(&m_Position);
	mov = pos - mov;
	

	
	bool moved = XMVector3NotEqual(mov, XMVectorZero());

	if (moved && m_ModelInstance)
	{
		XMMATRIX world = XMLoadFloat4x4(&m_ModelInstance->m_World);
		world.r[3] += mov;
		XMStoreFloat4x4(&m_ModelInstance->m_World, world);

		m_ModelInstance->m_World._42 = terrain->GetHeight(m_ModelInstance->m_World._41, m_ModelInstance->m_World._43) + heigth;
		m_Position.y = m_ModelInstance->m_World._42;
		return true;
	}
	return false;

	m_QuadTreeType->Update();
}

void GameObject::SetModelInstance(ModelInstance *modelInstance)
{ 
	SAFE_DELETE(m_ModelInstance);
	SAFE_DELETE(m_QuadTreeType);

	m_ModelInstance = modelInstance; 		
	m_QuadTreeType = new QuadTreeTypeModel(m_ModelInstance);
}
