#include "SmallShop_B.h"

SmallShop_B::SmallShop_B()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallShopCarlbucks" ) );

	gVegModel = "SmallShopCarlbucks_VEG";
}

SmallShop_B::~SmallShop_B()
{

}