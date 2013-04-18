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
}

void Player::UpdateCamera(float deltaTime)
{
	//Move Camera
	XMVECTOR cameraPosV = XMLoadFloat3(&m_Camera->GetPosition());
	XMVECTOR cameraFor = XMLoadFloat3(&m_Camera->GetForward());
	XMVECTOR cameraRig = XMLoadFloat3(&m_Camera->GetRight());

	m_Controller->Update();
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