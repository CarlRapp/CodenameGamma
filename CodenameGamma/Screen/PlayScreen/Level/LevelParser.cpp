#include "LevelParser.h"
#include "../Structures/StructureList.h"
#include "../Graphics/ModelManager.h"

LevelParser* LevelParser::gInstance	=	0;
LevelParser::LevelParser()
{

}
LevelParser::~LevelParser()
{
}

string LevelParser::GetToken(string Line, bool IncludeDigits)
{
	string	tToken;

	for(int i = 0; i < Line.length(); ++i)
		if( Line[i] != ' ' )
			if ( IncludeDigits )
			{
				if ( Line[i] != '1' && Line[i] != '2' && Line[i] != '3' && Line[i] != '4' )
					tToken += Line[i];
			}
			else
			{
				tToken += Line[i];
			}
		else
			break;

	return tToken;
}
LevelData LevelParser::ParseLevel(string LevelName, string LevelsRootPath)
{
	LevelData	LData	=	LevelData();
	LData.LevelRootPath	=	LevelsRootPath + LevelName + "/";

	ifstream	tFileStream;
	string		tLine, tToken, tStr;

	tFileStream.open(LData.LevelRootPath + "map.cfg");

	LData.PathMapResX	= 800;
	LData.PathMapResY	= 800;
	LData.NumPathMaps	= 6;

	if ( !tFileStream.is_open() )
	{
		DebugScreen::GetInstance()->AddLogMessage("Error loading " + LevelName + ".cfg", Red);
		return LData;
	}

	while(!tFileStream.eof())
	{
		//	Read the current line
		getline(tFileStream, tLine);

		//	Get the "token", i.e the
		//	first part of the line
		tToken	=	GetToken(tLine, true);

		if ( tToken == "Width" )
		{
			tStr	=	tLine.substr(tToken.size() + 1);
			LData.Width	=	atoi(tStr.c_str());
		}
		else if ( tToken == "Height" )
		{
			tStr	=	tLine.substr(tToken.size() + 1);
			LData.Height	=	atoi(tStr.c_str());
		}
		else if ( tToken == "ResX" )
		{
			tStr	=	tLine.substr(tToken.size() + 1);
			LData.ResolutionX	=	atoi(tStr.c_str());
		}
		else if ( tToken == "ResY" )
		{
			tStr	=	tLine.substr(tToken.size() + 1);
			LData.ResolutionY	=	atoi(tStr.c_str());
		}
		//	Texture
		else if ( tToken == "Texture" )
		{
			int	Index	=	atoi(tLine.substr(tToken.size(), 1).c_str()) - 1;
			tStr	=	tLine.substr(tToken.size() + 2);
			LData.Textures[Index].Texture	=	tStr;
		}
		//	Normal Map
		else if ( tToken == "NormalMap" )
		{
			int	Index	=	atoi(tLine.substr(tToken.size(), 1).c_str()) - 1;
			tStr	=	tLine.substr(tToken.size() + 2);
			LData.Textures[Index].NormalMap	=	tStr;
		}
		else if ( tToken == "BlendMap" )
		{
			LData.BlendMap	=	tLine.substr(tToken.size() + 1);
		}
		else if ( tToken == "HeightMap" )
		{
			tStr	=	tLine.substr(tToken.size() + 1);

			string	Name	=	GetToken(tStr, true);
			LData.HeightMap.Filename	=	Name;

			tStr	=	tStr.substr(Name.size() + 1);

			tToken	=	GetToken(tStr, false);
			tStr	=	tStr.substr(tToken.size() + 1);
			LData.HeightMap.Width	=	atoi(tToken.c_str());

			
			tToken	=	GetToken(tStr, false);
			tStr	=	tStr.substr(tToken.size());
			LData.HeightMap.Height	=	atoi(tToken.c_str());
		}
		else if ( tToken == "TextureDimensions" )
		{
			tStr	=	tLine.substr(tToken.size() + 1);

			tToken	=	GetToken(tStr, false);
			tStr	=	tStr.substr(tToken.size() + 1);
			LData.TextureX	=	atoi(tToken.c_str());

			
			tToken	=	GetToken(tStr, false);
			tStr	=	tStr.substr(tToken.size());
			LData.TextureY	=	atoi(tToken.c_str());
		}
			


	}
	tFileStream.close();

	return LData;
}

EntityData LevelParser::ParseLevelEntities(LevelData Data)
{
	vector<GameObject*>	GameObjects		=	vector<GameObject*>();
	vector<Light*>		Lights			=	vector<Light*>();
	vector<string>		GameObjectNames	=	vector<string>();
	vector<PatrolNode*>	PatrolNodes		=	vector<PatrolNode*>();
	NodeMap*			NodeMapInstance	=	new NodeMap();

	ifstream	tFileStream;
	string		tLine, tToken, tStr;
	
	tFileStream.open(Data.LevelRootPath + "data.map");

	if ( !tFileStream.is_open() )
	{
		DebugScreen::GetInstance()->AddLogMessage("Error loading " + Data.LevelRootPath + "data.map", Red);
		return EntityData();
	}

	while(!tFileStream.eof())
	{
		//	Read the current line
		getline(tFileStream, tLine);
		if( tLine == "" )
			break;

		//	A new GameObject name
		if( tLine[0] == '-' )
			GameObjectNames.push_back( tLine.substr( 1 ) );

		if ( tLine[0] == '#' )
		{
			string tString	=	tLine.substr( 1, tLine.find( ';' ) - 1 );
			int	Index		=	atoi( tString.c_str() );
			tLine			=	tLine.substr( tString.size() + 2 );

			GameObject*	tObject	=	ParseGameObject( tLine, GameObjectNames[Index], Data );

			if( tObject != 0 )
				GameObjects.push_back( tObject );
		}
		else if( tLine[0] == '$' )
		{
			string tString	=	tLine.substr( 1, tLine.find( ';' ) - 1 );
			int	Index		=	atoi( tString.c_str() );
			tLine			=	tLine.substr( tString.size() + 2 );

			Light*	tLight	=	0;

			if( Index == 0 )
				tLight	=	ParsePointLight( tLine, Data );
			else
				tLight	=	ParseSpotLight( tLine, Data );


			if( tLight != 0 )
				Lights.push_back( tLight );
		}
		else if( tLine[0] == '&' )
		{
			string tString	=	tLine.substr( 1 );

			PatrolNode*	tNode	=	ParsePatrolNode( tString, Data );

			if( tNode != 0 )
				PatrolNodes.push_back( tNode );
		}
		else if( tLine[0] == '?' )
		{
			string tString	=	tLine.substr( 1, tLine.find( ';' ) - 1 );
			int	Index		=	atoi( tString.c_str() );
			tLine			=	tLine.substr( tString.size() + 2 );

			PatrolNode*	NodeA	=	PatrolNodes[Index];

			bool	Last	=	false;
			while(!Last)
			{
				if( tLine.find( ';' ) == -1 )
					Last	=	true;

				int			IndexB	=	atoi( tLine.substr( 0, tLine.find( ';' ) ).c_str() );
				PatrolNode*	NodeB	=	PatrolNodes[ IndexB ];

				NodeMapInstance->SetNodeAdjacent( NodeA, NodeB );

				//	Remove the index 
				if( !Last )
					tLine	=	tLine.substr( tLine.find( ';' ) + 1 );
			}
		}


	}
	tFileStream.close();

	for each( PatrolNode* PN in PatrolNodes )
		NodeMapInstance->AddNode( PN );

	EntityData	Result		=	EntityData();
	Result.GameObjects		=	GameObjects;
	Result.Lights			=	Lights;
	Result.NodeMapInstance	=	NodeMapInstance;

	return Result;
}


GameObject* LevelParser::GetGameObject( string GameObjectName )
{
	GameObject*	Result	=	0;

	if( GameObjectName == "UnitCube" )
	{
		ModelManager::GetInstance()->LoadModel("UnitCube", "UnitCube.obj", "DATA/Models/UnitCube/");
		Result	=	new UnitCube();
	}
	else if( GameObjectName == "CrowdBarrier" )
	{
		ModelManager::GetInstance()->LoadModel("CrowdBarrier", "CrowdBarrier.obj", "DATA/Models/CrowdBarrier/");
		Result	=	new CrowdBarrier();
	}
	else if( GameObjectName == "SmallStore" )
	{
		ModelManager::GetInstance()->LoadModel("SmallStore", "SmallStore.obj", "DATA/Models/SmallStore/");
		Result	=	new SmallStore();
	}
	else if( GameObjectName == "TownHall" )
	{
		ModelManager::GetInstance()->LoadModel("TownHall", "TownHall.dae", "DATA/Models/TownHall/");
		Result	=	new TownHall();
	}
	else if( GameObjectName == "Container" )
	{
		ModelManager::GetInstance()->LoadModel("Container", "Container.obj", "DATA/Models/Container/");
		Result	=	new Container();
	}
	else if( GameObjectName == "TrashCan" )
	{
		ModelManager::GetInstance()->LoadModel("TrashCan", "TrashCan.obj", "DATA/Models/TrashCan/");
		Result	=	new TrashCan();
	}
	else if( GameObjectName == "VolvoCar" )
	{
		ModelManager::GetInstance()->LoadModel("VolvoCar", "VolvoCar.obj", "DATA/Models/VolvoCar/");
		Result	=	new VolvoCar();
	}
	else if( GameObjectName == "UtilityPole" )
	{
		ModelManager::GetInstance()->LoadModel("UtilityPole", "UtilityPole.obj", "DATA/Models/UtilityPole/");
		Result	=	new UtilityPole();
	}
	else if( GameObjectName == "TankBarrier" )
	{
		ModelManager::GetInstance()->LoadModel("TankBarrier", "TankBarrier.obj", "DATA/Models/TankBarrier/");
		Result	=	new TankBarrier();
	}
	else if( GameObjectName == "SmallShop_B" )
	{
		ModelManager::GetInstance()->LoadModel("SmallShop", "SmallShop_B.obj", "DATA/Models/SmallShop_B/");
		Result	=	new SmallShop_B();
	}
	else if( GameObjectName == "SmallShop_A" )
	{
		ModelManager::GetInstance()->LoadModel("SmallShopBase", "SmallShop_A.obj", "DATA/Models/SmallShop_A/");
		Result	=	new SmallShop_A();
	}
	else if( GameObjectName == "SandBag" )
	{
		ModelManager::GetInstance()->LoadModel("SandBag", "SandBag.obj", "DATA/Models/SandBag/");
		Result	=	new SandBag();
	}
	else if( GameObjectName == "PickUpTruck" )
	{
		ModelManager::GetInstance()->LoadModel("PickUpTruck", "PickUpTruck.obj", "DATA/Models/PickUpTruck/");
		Result	=	new PickupTruck();
	}
	else if( GameObjectName == "MarketStore" )
	{
		ModelManager::GetInstance()->LoadModel("MarketStore", "MarketStore.obj", "DATA/Models/MarketStore/");
		Result	=	new MarketStore();
	}
	else if( GameObjectName == "Lamp" )
	{
		ModelManager::GetInstance()->LoadModel("Lamp", "Lamp.obj", "DATA/Models/Lamp/");
		Result	=	new Lamp();
	}
	else if( GameObjectName == "HotDogStand" )
	{
		ModelManager::GetInstance()->LoadModel("HotDogStand", "HotDogStand.obj", "DATA/Models/HotDogStand/");
		Result	=	new HotDogStand();
	}
	else if( GameObjectName == "FireHydrant" )
	{
		ModelManager::GetInstance()->LoadModel("FireHydrant", "FireHydrant.obj", "DATA/Models/FireHydrant/");
		Result	=	new FireHydrant();
	}
	else if( GameObjectName == "FactoryBuilding" )
	{
		ModelManager::GetInstance()->LoadModel("FactoryBuilding", "FactoryBuilding.obj", "DATA/Models/FactoryBuilding/");
		Result	=	new FactoryBuilding();
	}
	else if( GameObjectName == "Crate" )
	{
		ModelManager::GetInstance()->LoadModel("Crate", "Crate.obj", "DATA/Models/Crate/");
		Result	=	new Crate();
	}
	else if( GameObjectName == "ApartmentBuilding" )
	{
		ModelManager::GetInstance()->LoadModel("ApartmentBuilding", "ApartmentBuilding.obj", "DATA/Models/ApartmentBuilding/");
		Result	=	new ApartmentBuilding();
	}
	else if( GameObjectName == "AbandonedBuilding" )
	{
		ModelManager::GetInstance()->LoadModel("AbandonedBuilding", "AbandonedBuilding.obj", "DATA/Models/AbandonedBuilding/");
		Result	=	new AbandonedBuilding();
	}
	else if( GameObjectName == "OilBarrel" )
	{
		ModelManager::GetInstance()->LoadModel("OilBarrel", "OilBarrel.obj", "DATA/Models/OilBarrel/");
		Result	=	new OilBarrel();
	}
	else
	{
		ModelManager::GetInstance()->LoadModel("UnitCube", "UnitCube.obj", "DATA/Models/UnitCube/");
		Result	=	new UnitCube();
	}



	return Result;
}
GameObject* LevelParser::ParseGameObject( string Line, string GameObjectName, LevelData Data )
{
	GameObject*	Result	=	GetGameObject( GameObjectName );
	if( Result == 0 )
		return Result;

	float	PosX, PosY, PosZ, RotationY, RotationX;

	PosX	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	PosY	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	PosZ	=	Data.Height - atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	RotationY	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() ) + PI;
	Line		=	Line.substr( Line.find( ';' ) + 1 );
	RotationX	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );

	Result->MoveTo( XMFLOAT3( PosX, PosY, PosZ ) );
	Result->SetRotation( XMFLOAT3( RotationX, RotationY, 0 ) );

	return Result;
}

SpotLight* LevelParser::ParseSpotLight( string Line, LevelData Data )
{
	XMFLOAT3	Position, Color;
	float		Range, ConeSize, RotationY, RotationX;
	bool		CastShadow;

	Position.x	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Position.y	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Position.z	=	Data.Height - atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Color.x	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Color.y	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Color.z	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Range	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	CastShadow	=	( Line.substr( 0, Line.find( ';' ) ) == "True" );
	Line		=	Line.substr( Line.find( ';' ) + 1 );

	ConeSize	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line		=	Line.substr( Line.find( ';' ) + 1 );

	RotationY	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() ) + PI;
	Line		=	Line.substr( Line.find( ';' ) + 1 );

	RotationX	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line		=	Line.substr( Line.find( ';' ) + 1 );

	XMFLOAT3	Direction	=	XMFLOAT3( 0, 0, -1 );
	XMMATRIX	RotX		=	XMMatrixRotationX(RotationX);
	XMMATRIX	RotY		=	XMMatrixRotationY(RotationY);
	XMMATRIX	Rotation	=	RotX * RotY;

	XMStoreFloat3(
		&Direction,
		XMVector3TransformCoord( XMLoadFloat3( &Direction ), Rotation )
	);

	SpotLight*	spotLight	=	new SpotLight();

	spotLight->GetGPULight()->Color		=	XMFLOAT4( Color.x, Color.y, Color.z, 0.0f );
	spotLight->GetGPULight()->HasShadow	=	CastShadow;
	spotLight->GetGPULight()->Position	=	Position;
	spotLight->GetGPULight()->Range		=	Range;
	spotLight->GetGPULight()->angle		=	ConeSize;
	spotLight->GetGPULight()->Direction	=	Direction;

	return	spotLight;
}
PointLight* LevelParser::ParsePointLight( string Line, LevelData Data )
{
	XMFLOAT3	Position, Color;
	float		Range;
	bool		CastShadow;

	Position.x	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Position.y	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Position.z	=	Data.Height - atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Color.x	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Color.y	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Color.z	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Range	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	CastShadow	=	( Line == "True" );

	PointLight*	pointLight	=	new PointLight();

	pointLight->GetGPULight()->Color		=	XMFLOAT4( Color.x, Color.y, Color.z, 0.0f );
	pointLight->GetGPULight()->HasShadow	=	CastShadow;
	pointLight->GetGPULight()->Position		=	Position;
	pointLight->GetGPULight()->Range		=	Range;

	return	pointLight;
}


PatrolNode*	LevelParser::ParsePatrolNode( string Line, LevelData Data )
{
	XMFLOAT2	Position;
	Position.x	=	atof( Line.substr( 0, Line.find( ';' ) ).c_str() );
	Line	=	Line.substr( Line.find( ';' ) + 1 );

	Position.y	=	Data.Height - atof( Line.c_str() );

	return new PatrolNode( Position );
}