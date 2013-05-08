#include "TrashCan.h"

TrashCan::TrashCan()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TrashCan" ) );
	SetScale( 2 );
}

TrashCan::~TrashCan()
{

}