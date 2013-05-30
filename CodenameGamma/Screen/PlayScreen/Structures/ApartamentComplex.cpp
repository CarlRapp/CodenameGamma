#include "ApartmentComplex.h"

ApartmentComplex::ApartmentComplex()
{
	SetModelInstance( ModelManager::GetInstance()->CreateModelInstance( "ApartmentComplex" ) );

	gVegModel = "ApartmentComplex_VEG";
}

ApartmentComplex::~ApartmentComplex()
{

}