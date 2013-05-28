#include "Fence.h"

Fence::Fence()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Fence" ) );
}

Fence::~Fence()
{

}