#include "SmallShop_A.h"

SmallShop_A::SmallShop_A()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallShopBase" ) );
}

SmallShop_A::~SmallShop_A()
{

}