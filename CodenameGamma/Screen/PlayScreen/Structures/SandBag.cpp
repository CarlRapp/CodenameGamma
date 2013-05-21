#include "SandBag.h"

SandBag::SandBag()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SandBag" ) );
}

SandBag::~SandBag()
{

}