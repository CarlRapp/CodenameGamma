#include "Fence.h"

Fence::Fence()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Fence" ) );
	
	gVegModel = "Fence_VEG";
}

Fence::~Fence()
{

}