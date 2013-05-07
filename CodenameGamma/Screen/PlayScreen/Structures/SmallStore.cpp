#include "SmallStore.h"

SmallStore::SmallStore()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallStore" ) );
	SetScale( XMFLOAT3( 8, 8, 8 ) );
}

SmallStore::~SmallStore()
{

}