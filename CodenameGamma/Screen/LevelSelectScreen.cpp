#include "LevelSelectScreen.h"
#include "../../External/dirent.h"
#include "PlayScreen/Units/Unit.h"
#include <fstream>

LevelSelectScreen::LevelSelectScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);

	gScreenData	=	Setup;
	//gScreenData->NUMBER_OF_PLAYERS	=	1;

	gMapMenu	=	vector<MapMenuEntry>();

	gStartIndex		=	0;
	gListLimit		=	8;
	gCurrentIndex	=	0;
	gMenuTextSize	=	36;

	CreateMapMenu();
}

bool LevelSelectScreen::Load()
{
	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(gDevice, L"Apocalypse 1", &gTextInstance);
	pFW1Factory->Release();

	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/MAIN_MENU.png", 0, 0, &gBackground, 0 );

	return true;
}

bool LevelSelectScreen::Unload()
{
	SAFE_RELEASE( gBackground );
	SAFE_RELEASE( gTextInstance );

	return true;
}

void LevelSelectScreen::Update(float DeltaTime)
{
	Controller*	tC	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	tK	=	InputManager::GetInstance()->GetKeyboard();

	bool	UP		=	tC->GetButtonState( D_UP ) == PRESSED		|| tK->GetKeyState( VK_UP ) == PRESSED		|| tK->GetKeyState( 'W' ) == PRESSED;
	bool	DOWN	=	tC->GetButtonState( D_DOWN ) == PRESSED		|| tK->GetKeyState( VK_DOWN ) == PRESSED	|| tK->GetKeyState( 'S' ) == PRESSED;
	bool	CONFIRM	=	tC->GetButtonState( A ) == PRESSED			|| tK->GetKeyState( VK_RETURN ) == PRESSED;
	bool	BACK	=	tC->GetButtonState( B ) == PRESSED			|| tK->GetKeyState( VK_ESCAPE ) == PRESSED;

	if( UP )
	{
		SoundManager::GetInstance()->Play("MenuChange", SFX);
		--gCurrentIndex;

		if( gCurrentIndex < 0 )
			gCurrentIndex	=	gMapMenu.size() - 1;
	}
	if( DOWN )
	{
		SoundManager::GetInstance()->Play("MenuChange", SFX);
		++gCurrentIndex;

		if( gCurrentIndex >= gMapMenu.size() )
			gCurrentIndex	=	0;

	}

	if( CONFIRM )
	{
		gScreenData->LEVEL_NAME	=	gMapMenu[gCurrentIndex].second.Name;
		SoundManager::GetInstance()->Play("MenuPick", SFX);
		gGotoNextFrame	=	PRE_PLAY_SCREEN;
	}
	if( BACK )
		gGotoNextFrame	=	MAIN_MENU_SCREEN;

	gStartIndex	=	( gStartIndex < gCurrentIndex - gListLimit ) ? gCurrentIndex - gListLimit : gStartIndex;
	gStartIndex	=	( gStartIndex > gCurrentIndex ) ? gCurrentIndex : gStartIndex;
	gStartIndex	=	MathHelper::Clamp( 0, gStartIndex, (int)gMapMenu.size() - gListLimit );
}


void LevelSelectScreen::Render()
{
	gGraphicsManager->RenderQuad( gFullscreenVP, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.05f, gScreenHeight * 0.05f);
	
	DrawString(*gTextInstance, "Level Selection", tPos.x, tPos.y, 72, White, WhiteTrans, 2, FW1_LEFT);

	tPos.y	+=	1.5f * 72;

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

	tPos	=	XMFLOAT2(gScreenWidth * 0.50f, tPos.y);

	//	Print info about the map
	LevelInfo	tInfo	=	gMapMenu[ gCurrentIndex ].second;

	string	tArea		=	to_string( (long double)( tInfo.Width * tInfo.Height ) );
	string	tWidth		=	to_string( (long double)tInfo.Width );
	string	tHeight		=	to_string( (long double)tInfo.Height );
	string	tPlayers	=	to_string( (long double)tInfo.PlayerCount );
	float	tInfoSize	=	0.8f * gMenuTextSize;

	DrawString(*gTextInstance, "Area:     " + tWidth + "x" + tHeight + " m2", tPos.x, tPos.y, tInfoSize, White, BlackTrans, 2, FW1_LEFT);
	DrawString(*gTextInstance, "Players:  " + tPlayers, tPos.x, tPos.y + 1.2f * tInfoSize, tInfoSize, White, BlackTrans, 2, FW1_LEFT);
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

	if( gMapMenu.size() < gListLimit )
		gListLimit	=	gMapMenu.size() - 1;
}

void LevelSelectScreen::Reset()
{
	gStartIndex		=	0;
	gCurrentIndex	=	0;
}