#include "FireHydrant.h"

FireHydrant::FireHydrant()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "FireHydrant" ) );
}

FireHydrant::~FireHydrant()
{

}