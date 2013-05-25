#include "LevelSelectScreen.h"
#include "../../External/dirent.h"
#include "PlayScreen/Units/Unit.h"
#include <fstream>

LevelSelectScreen::LevelSelectScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	gScreenData	=	Setup;
	gScreenData->NUMBER_OF_PLAYERS	=	1;

	gMapMenu	=	vector<MapMenuEntry>();

	gStartIndex		=	0;
	gListLimit		=	3;
	gCurrentIndex	=	0;
	gMenuTextSize	=	32;

	CreateMapMenu();
}

void LevelSelectScreen::Update(float DeltaTime)
{
	InputManager*	IM	=	InputManager::GetInstance();

	if( IM->GetKeyboard()->GetKeyState( VK_DOWN ) == PRESSED )
	{
		++gCurrentIndex;

		if( gCurrentIndex >= gMapMenu.size() )
			gCurrentIndex	=	0;
	}
	if( IM->GetKeyboard()->GetKeyState( VK_UP ) == PRESSED )
	{
		--gCurrentIndex;

		if( gCurrentIndex < 0 )
			gCurrentIndex	=	gMapMenu.size() - 1;
	}

	if( IM->GetKeyboard()->GetKeyState( VK_RETURN ) == PRESSED )
	{
		gScreenData->LEVEL_NAME	=	gMapMenu[gCurrentIndex].second.Name;

		gGotoNextFrame	=	PRE_PLAY_SCREEN;
	}

	gStartIndex	=	( gStartIndex < gCurrentIndex - gListLimit ) ? gCurrentIndex - gListLimit : gStartIndex;
	gStartIndex	=	( gStartIndex > gCurrentIndex ) ? gCurrentIndex : gStartIndex;
	gStartIndex	=	MathHelper::Clamp( 0, gStartIndex, (int)gMapMenu.size() - gListLimit );
}


void LevelSelectScreen::Render()
{
	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.05f, gScreenHeight * 0.05f);

	DrawString(*gTextInstance, "Level Selection", tPos.x, tPos.y, 72, Black, White, 2, FW1_LEFT);

	tPos.y	+=	1.5f * 72;
	tPos.x	+=	0.5f * gMenuTextSize;

	//	Print the list
	for( int i = gStartIndex; i <= gStartIndex + gListLimit; ++i )
	{
		if( i >= gMapMenu.size() )
			break;

		if( i == gCurrentIndex )
			DrawString(*gTextInstance, gMapMenu[i].first, tPos.x, tPos.y + 1.5f * gMenuTextSize * ( i - gStartIndex ), gMenuTextSize, White, Black, 2, FW1_LEFT);
		else
			DrawString(*gTextInstance, gMapMenu[i].first, tPos.x, tPos.y + 1.5f * gMenuTextSize * ( i - gStartIndex ), gMenuTextSize, WhiteTrans, BlackTrans, 2, FW1_LEFT);
	}

	tPos	=	XMFLOAT2(gScreenWidth * 0.40f, tPos.y);

	//	Print info about the map
	LevelInfo	tInfo	=	gMapMenu[ gCurrentIndex ].second;

	string	tArea	=	to_string( (long double)( tInfo.Width * tInfo.Height ) );
	string	tWidth	=	to_string( (long double)tInfo.Width );
	string	tHeight	=	to_string( (long double)tInfo.Height );
	string	tPlayers=	to_string( (long double)tInfo.PlayerCount );
	
	DrawString(*gTextInstance, "Map Details:", tPos.x, tPos.y, gMenuTextSize, White, Black, 2, FW1_LEFT);
	DrawString(*gTextInstance, "Area: " + tWidth + "x" + tHeight + " meter", tPos.x, tPos.y + 1.5f * gMenuTextSize, gMenuTextSize, White, Black, 2, FW1_LEFT);
	DrawString(*gTextInstance, "Players: " + tPlayers, tPos.x, tPos.y + 3.0f * gMenuTextSize, gMenuTextSize, White, Black, 2, FW1_LEFT);
}

ScreenType LevelSelectScreen::GetScreenType()
{
	return LEVEL_SELECT_SCREEN;
}

void LevelSelectScreen::CreateMapMenu()
{
	string		Path	=	"DATA/Maps/";
	string		tName;
	ifstream	tFileStream;

	DIR*	RootFolder;
	dirent*	tEntry;

	if( RootFolder	=	opendir( Path.c_str() ) )
	{
		while( tEntry = readdir( RootFolder ) )
		{
			tName	=	(string)tEntry->d_name;
			//	Skip everything that aint a folder
			if( tName == "." || tName == ".." || tEntry->d_type != 16384)
				continue;

			//	Try and open the file
			tFileStream.open( Path + tName + "/map.cfg" );
			

			//	If we can't open the file,
			//	skip it.
			if ( !tFileStream.is_open() )
				continue;

			//	Create a LevelInfo and
			//	calculate the data from the file
			LevelInfo	levelEntry	=	LevelInfo();

			string	tWidth, tHeight;
			//	Read the two first lines
			getline(tFileStream, tWidth);
			getline(tFileStream, tHeight);
			tFileStream.close();


			levelEntry.Name		=	tName;
			//	Make the length into meters
			levelEntry.Width	=	ceil( MeterPerUnits * atof( tWidth.substr( tWidth.find(' ') ).c_str() ) );
			levelEntry.Height	=	ceil( MeterPerUnits * atof( tHeight.substr( tHeight.find(' ') ).c_str() ) );

			//	Roughly estimate the number
			//	of players for the map. ( 100x100m ~1 player )
			int	tArea	=	levelEntry.Width * levelEntry.Height;
			levelEntry.PlayerCount	=	(int)( MathHelper::Clamp(1.0f, tArea * 0.0001f, 4.0f) );

			gMapMenu.push_back( MapMenuEntry( tName, levelEntry ) );
		}
		closedir( RootFolder );
	}


}