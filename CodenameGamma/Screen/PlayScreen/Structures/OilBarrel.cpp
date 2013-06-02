#include "OilBarrel.h"

OilBarrel::OilBarrel()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "OilBarrel" ) );
}

OilBarrel::~OilBarrel()
{

}