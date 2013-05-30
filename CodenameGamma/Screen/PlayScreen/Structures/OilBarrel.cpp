#include "OilBarrel.h"

OilBarrel::OilBarrel()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "OilBarrel" ) );
	
	gVegModel = "OilBarrel_VEG";
}

OilBarrel::~OilBarrel()
{

}