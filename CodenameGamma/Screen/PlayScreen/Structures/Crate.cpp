#include "Crate.h"

Crate::Crate()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "Crate" ) );

	gVegModel = "Crate_VEG";
}

Crate::~Crate()
{

}