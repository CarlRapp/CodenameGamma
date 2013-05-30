#include "TrashCan.h"

TrashCan::TrashCan()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "TrashCan" ) );
	
	gVegModel = "TrashCan_VEG";
}

TrashCan::~TrashCan()
{

}