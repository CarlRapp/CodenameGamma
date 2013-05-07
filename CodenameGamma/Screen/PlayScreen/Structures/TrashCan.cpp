#include "TrashCan.h"

TrashCan::TrashCan()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TrashCan" ) );
	SetScale( XMFLOAT3( 1, 1, 1 ) );
}

TrashCan::~TrashCan()
{

}