#include "SandBag.h"

SandBag::SandBag()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SandBag" ) );
	
	gVegModel = "SandBag_VEG";
}

SandBag::~SandBag()
{

}