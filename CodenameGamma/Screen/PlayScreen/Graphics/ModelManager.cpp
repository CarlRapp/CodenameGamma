#include "ModelManager.h"

ModelManager* ModelManager::gInstance	=	0;
ModelManager::ModelManager(ID3D11Device* Device)
{
	gLoadedModels	=	map<string, Model*>();
	gDevice			=	Device;

	gTextureManager.Init(Device);
}
ModelManager::~ModelManager()
{
	for ( gModelIterator = gLoadedModels.begin(); gModelIterator != gLoadedModels.end(); ++gModelIterator )
		delete	gModelIterator->second;

	gLoadedModels.clear();
}

bool ModelManager::IsInitialized()
{
	return gInstance != 0;
}

void ModelManager::Initialize(ID3D11Device* Device)
{
	if ( IsInitialized() )
		return;

	gInstance		=	new ModelManager(Device);
}

ModelManager* ModelManager::GetInstance()
{
	return gInstance;
}

void ModelManager::LoadModel(string Name, string Filename, string Path)
{
	if ( gLoadedModels.count(Name) != 0 )
		return;

	Model*	tModel	=	new Model(gDevice, gTextureManager, Path + Filename, Path);
	gLoadedModels.insert(pair<string, Model*>(Name, tModel));

	DebugScreen::GetInstance()->AddLogMessage("Model: \"" + Filename + "\" loaded.", Green);
}

Model* ModelManager::GetModel(string Name)
{
	if ( gLoadedModels.count(Name) != 0 )
		return gLoadedModels[Name];

	DebugScreen::GetInstance()->AddLogMessage("Model: \"" + Name + "\" is not loaded.", Red);
	return 0;
}

ModelInstance* ModelManager::CreateModelInstance(string ModelName)
{
	Model*	tModel	=	GetModel(ModelName);

	if ( tModel == 0 )
		return 0;

	ModelInstance*	tInstance	=	new ModelInstance();
	tInstance->m_Model	=	tModel;

	return	tInstance;
}