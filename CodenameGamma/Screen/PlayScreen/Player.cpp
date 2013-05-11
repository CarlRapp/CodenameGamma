#include "Player.h"


Player::Player(void)
{
	m_Controller = InputManager::GetInstance()->GetController(0);
	m_Camera = new Camera();	
	m_Camera->SetPosition(2000, 100, 500);
	m_Unit = NULL;
	m_PlayerIndex = 0;
}

Player::Player(int index)
{
	m_Controller = InputManager::GetInstance()->GetController(index);
	m_Camera = new Camera();
	m_Camera->SetPosition(2000, 100, 500);
	m_Unit = NULL;
	m_PlayerIndex = index;
}

Player::~Player(void)
{

}

vector<Projectile*> Player::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
	
	if ( m_Unit )
	{
		//XBOX-CONTROL
		XMFLOAT3	newVel	=	XMFLOAT3(0, 0, 0);
		XMFLOAT3	tPosition	=	m_Unit->GetFloat3Value(Position);

		XMFLOAT2	rStickDir		=	m_Controller->GetStickDirection( RIGHT );

		if ( m_Controller->GetStickLength( RIGHT ) > 0.3f)
			m_Unit->LookAt(XMFLOAT3(tPosition.x - rStickDir.x, 0, tPosition.z - rStickDir.y));

		XMFLOAT2	lStickDir		=	m_Controller->GetStickDirection( LEFT );
		float		lStickLength	=	m_Controller->GetStickLength( LEFT ) * (160);
		lStickLength				*=	1 + m_Controller->GetTriggerValue( RIGHT );

		newVel	=	XMFLOAT3(lStickDir.x * lStickLength, 0, lStickDir.y * lStickLength);

		//KEYBOARD AND MOUSE
		if (m_PlayerIndex == 0)
		{			
			//if (InputManager::GetInstance()->GetKeyboard()->GetKeyState(VK_SPACE) == DOWN)
			if (InputManager::GetInstance()->GetMouse()->GetButtonState(M_RIGHT) == DOWN)
			{
				D3D11_VIEWPORT vp	= m_Camera->GetViewPort();
				XMVECTOR center		= XMLoadFloat2(&XMFLOAT2((float)(vp.TopLeftX + vp.Width / 2), (float)(vp.TopLeftY + vp.Height / 2)));
				XMVECTOR mousePos	= XMLoadFloat2(&InputManager::GetInstance()->GetMouse()->GetPosition(true));			

				XMFLOAT2 mouseDir;
				XMStoreFloat2(&mouseDir, mousePos - center);
				m_Unit->LookAt(XMFLOAT3(tPosition.x - mouseDir.x, 0, tPosition.z + mouseDir.y));
			}

			XMFLOAT2 walkdir = XMFLOAT2(0,0);
			if (InputManager::GetInstance()->GetKeyboard()->GetKeyState('W') == DOWN)
				walkdir.y += 1;
			if (InputManager::GetInstance()->GetKeyboard()->GetKeyState('S') == DOWN)
				walkdir.y -= 1;
			if (InputManager::GetInstance()->GetKeyboard()->GetKeyState('A') == DOWN)
				walkdir.x -= 1;
			if (InputManager::GetInstance()->GetKeyboard()->GetKeyState('D') == DOWN)
				walkdir.x += 1;

			float speed = 160;

			if (InputManager::GetInstance()->GetKeyboard()->GetKeyState(VK_SHIFT) == DOWN)
				speed *= 2;

			if (walkdir.x != 0 || walkdir.y != 0)
				newVel	=	XMFLOAT3(walkdir.x * speed, 0, walkdir.y * speed);
		}

		m_Unit->SetVelocity(newVel);
		
		m_Camera->SetPosition(tPosition.x, tPosition.y + 200, tPosition.z - 100);
		m_Camera->SetLookAt(tPosition);


		vector<Projectile*>	tBullets;
		if ( m_Controller->GetButtonState( RIGHT_BUMPER ) == DOWN ||
			(m_PlayerIndex == 0 && InputManager::GetInstance()->GetMouse()->GetButtonState(M_LEFT) == DOWN))
		{
			tBullets=	m_Unit->FireWeapon();
		}
		return tBullets;
		//m_Camera->SetFarZ(tPosition.y + 700.0f);
	}

	return vector<Projectile*>();
}

void Player::UpdateCamera(float deltaTime)
{
	//Move Camera
	XMVECTOR cameraPosV = XMLoadFloat3(&m_Camera->GetPosition());
	XMVECTOR cameraFor = XMLoadFloat3(&m_Camera->GetForward());
	XMVECTOR cameraRig = XMLoadFloat3(&m_Camera->GetRight());

	XMFLOAT2 stickleft = m_Controller->GetStickDirection(LEFT);

	cameraPosV += stickleft.y * cameraFor * deltaTime * 300;
	cameraPosV += stickleft.x * cameraRig * deltaTime * 300;

	XMFLOAT3 cameraPos;
	XMStoreFloat3(&cameraPos, cameraPosV);

	m_Camera->SetPosition(cameraPos);

	//Change Look-Direction
	XMFLOAT2 stickright = m_Controller->GetStickDirection(RIGHT);

	float dx = stickright.x * deltaTime * 4;
	float dy = stickright.y * deltaTime * 4;

	m_Camera->Rotate(-dy, dx);
}

void Player::SetUnit(PlayerUnit* Instance)
{
	m_Unit	=	Instance;
}