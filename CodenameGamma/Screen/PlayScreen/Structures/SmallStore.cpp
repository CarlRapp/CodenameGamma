#include "SmallStore.h"

SmallStore::SmallStore()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "SmallStore" ) );
	SetScale( XMFLOAT3( 5, 5, 5 ) );
}

SmallStore::~SmallStore()
{

}