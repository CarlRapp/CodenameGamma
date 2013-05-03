#include "Player.h"


Player::Player(void)
{
	m_Controller = InputManager::GetInstance()->GetController(0);
	m_Camera = new Camera();	
	m_Camera->SetPosition(2000, 100, 500);
}

Player::Player(int index)
{
	m_Controller = InputManager::GetInstance()->GetController(index);
	m_Camera = new Camera();
	m_Camera->SetPosition(2000, 100, 500);
}

Player::~Player(void)
{

}

void Player::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
	
	if ( m_GameObject )
	{
		XMFLOAT3	newVel	=	XMFLOAT3(0, 0, 0);
		XMFLOAT3	tPosition	=	m_GameObject->GetFloat3Value(Position);

		XMFLOAT2	rStickDir		=	m_Controller->GetStickDirection( RIGHT );

		if ( m_Controller->GetStickLength( RIGHT ) > 0.3f)
			m_GameObject->LookAt(XMFLOAT3(tPosition.x - rStickDir.x, 0, tPosition.z - rStickDir.y));

		XMFLOAT2	lStickDir		=	m_Controller->GetStickDirection( LEFT );
		float		lStickLength	=	m_Controller->GetStickLength( LEFT ) * (80);
		lStickLength	*=	1 + m_Controller->GetTriggerValue( RIGHT );

		newVel	=	XMFLOAT3(lStickDir.x * lStickLength, 0, lStickDir.y * lStickLength);

		m_GameObject->SetVelocity(newVel);

		m_Camera->SetPosition(tPosition.x, tPosition.y + 350, tPosition.z - 50);
		m_Camera->SetLookAt(tPosition);
	}
	
}

void Player::UpdateCamera(float deltaTime)
{
	//Move Camera
	XMVECTOR cameraPosV = XMLoadFloat3(&m_Camera->GetPosition());
	XMVECTOR cameraFor = XMLoadFloat3(&m_Camera->GetForward());
	XMVECTOR cameraRig = XMLoadFloat3(&m_Camera->GetRight());

	XMFLOAT2 stickleft = m_Controller->GetStickDirection(Xbox_Direction::LEFT);

	cameraPosV += stickleft.y * cameraFor * deltaTime * 300;
	cameraPosV += stickleft.x * cameraRig * deltaTime * 300;

	XMFLOAT3 cameraPos;
	XMStoreFloat3(&cameraPos, cameraPosV);

	m_Camera->SetPosition(cameraPos);

	//Change Look-Direction
	XMFLOAT2 stickright = m_Controller->GetStickDirection(Xbox_Direction::RIGHT);

	float dx = stickright.x * deltaTime * 4;
	float dy = stickright.y * deltaTime * 4;

	m_Camera->Rotate(-dy, dx);
}

void Player::SetGameObject(GameObject* Instance)
{
	m_GameObject	=	Instance;
}