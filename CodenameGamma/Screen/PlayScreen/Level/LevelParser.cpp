#include "LevelParser.h"
#include "../Structures/StructureList.h"

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


	}
	tFileStream.close();

	EntityData	Result	=	EntityData();
	Result.GameObjects	=	GameObjects;
	Result.Lights		=	Lights;

	return Result;
}


GameObject* LevelParser::GetGameObject( string GameObjectName )
{
	GameObject*	Result	=	0;

	if( GameObjectName == "UnitCube" )
	{
		Result	=	new UnitCube();
	}
	else if( GameObjectName == "CrowdBarrier" )
	{
		Result	=	new CrowdBarrier();
	}
	else if( GameObjectName == "SmallStore" )
	{
		Result	=	new SmallStore();
	}
	else if( GameObjectName == "TownHall" )
	{
		Result	=	new TownHall();
	}
	else if( GameObjectName == "Container" )
	{
		Result	=	new Container();
	}
	else if( GameObjectName == "TrashCan" )
	{
		Result	=	new TrashCan();
	}
	else if( GameObjectName == "VolvoCar" )
	{
		Result	=	new VolvoCar();
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