#include "FireHydrant.h"

FireHydrant::FireHydrant()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "FireHydrant" ) );
	
	gVegModel = "FireHydrant_VEG";
}

FireHydrant::~FireHydrant()
{

}