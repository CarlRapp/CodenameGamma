#include "LevelParser.h"

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

	if ( !tFileStream.is_open() )
	{
		DebugScreen::GetInstance()->AddLogMessage("Error loading \"" + LevelName + "\".", Red);
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
		else if ( tToken == "ResZ" )
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
			tStr	=	tLine.substr(tToken.size() + 1);

			string	Name	=	GetToken(tStr, true);
			LData.BlendMap.Filename	=	Name;

			tStr	=	tStr.substr(Name.size() + 1);

			tToken	=	GetToken(tStr, false);
			tStr	=	tStr.substr(tToken.size() + 1);
			LData.BlendMap.Width	=	atoi(tToken.c_str());

			
			tToken	=	GetToken(tStr, false);
			tStr	=	tStr.substr(tToken.size());
			LData.BlendMap.Height	=	atoi(tToken.c_str());
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

	DebugScreen::GetInstance()->AddLogMessage("Map: \"" + LevelName + "\" loaded.", Green);
	return LData;
}