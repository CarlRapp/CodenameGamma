#include "SmallShop_A.h"

SmallShop_A::SmallShop_A()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallShopBase" ) );

	gVegModel = "SmallShopBase_VEG";
}

SmallShop_A::~SmallShop_A()
{

}