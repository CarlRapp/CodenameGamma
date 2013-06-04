#include "OptionScreen.h"
#include <iostream>
#include <io.h>

OptionScreen::OptionScreen(ScreenData* Setup)
{
	LoadScreenData(Setup);
	
	gResolutions[0]	=	ResolutionEntry( 1280, 720 );
	gResolutions[1]	=	ResolutionEntry( 1920, 1080 );
	int	ResIndex	=	0;
	for( int i = 0; i < 2; ++i )
		if( gResolutions[i].first == Setup->WIDTH )
			ResIndex	=	i;

	gMenu	=	vector<OptionEntry>();
	gMenu.push_back( OptionEntry( "Resolution: ", EntryData( Resolution, ResIndex ) ) );
	gMenu.push_back( OptionEntry( "Fullscreen: ", EntryData( Checkbox, (int)Setup->FULLSCREEN ) ) );
	gMenu.push_back( OptionEntry( "Visible Mouse: ", EntryData( Checkbox, (int)Setup->MOUSEVISIBLE ) ) );
	gMenu.push_back( OptionEntry( "Master Volume: ", EntryData( Volume, (int)( 10.0f * SoundManager::GetInstance()->GetVolume( Master ) ) ) ) );
	gMenu.push_back( OptionEntry( "Song Volume: ", EntryData( Volume, (int)( 10.0f * SoundManager::GetInstance()->GetVolume( Song ) ) ) ) );
	gMenu.push_back( OptionEntry( "SFX Volume: ", EntryData( Volume, (int)( 10.0f * SoundManager::GetInstance()->GetVolume( SFX ) ) ) ) );
	gMenuIndex	=	0;
}

bool OptionScreen::Load()
{
	IFW1Factory				*pFW1Factory = 0;
	FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	pFW1Factory->CreateFontWrapper(gDevice, L"Visitor TT1 BRK", &gTextInstance);
	pFW1Factory->Release();

	D3DX11CreateShaderResourceViewFromFile( gScreenData->DEVICE, "DATA/MAIN_MENU.png", 0, 0, &gBackground, 0 );

	return true;
}

bool OptionScreen::Unload()
{
	SAFE_RELEASE( gBackground );
	SAFE_RELEASE( gTextInstance );

	return true;
}

void OptionScreen::Update(float DeltaTime)
{
	Controller*	tC	=	InputManager::GetInstance()->GetController(0);
	Keyboard*	tK	=	InputManager::GetInstance()->GetKeyboard();

	bool	UP		=	tC->GetButtonState( D_UP ) == PRESSED		|| tK->GetKeyState( VK_UP ) == PRESSED		|| tK->GetKeyState( 'W' ) == PRESSED;
	bool	DOWN	=	tC->GetButtonState( D_DOWN ) == PRESSED		|| tK->GetKeyState( VK_DOWN ) == PRESSED	|| tK->GetKeyState( 'S' ) == PRESSED;
	bool	LEFT	=	tC->GetButtonState( D_LEFT ) == PRESSED		|| tK->GetKeyState( VK_LEFT ) == PRESSED	|| tK->GetKeyState( 'A' ) == PRESSED;
	bool	RIGHT	=	tC->GetButtonState( D_RIGHT ) == PRESSED	|| tK->GetKeyState( VK_RIGHT ) == PRESSED	|| tK->GetKeyState( 'D' ) == PRESSED;
	bool	CONFIRM	=	tC->GetButtonState( A ) == PRESSED			|| tK->GetKeyState( VK_RETURN ) == PRESSED;
	bool	BACK	=	tC->GetButtonState( B ) == PRESSED			|| tK->GetKeyState( VK_ESCAPE ) == PRESSED;

	if( UP )
		--gMenuIndex;
	if( DOWN )
		++gMenuIndex;

	if( gMenuIndex < gMenu.size() )
	{
		switch( gMenu[gMenuIndex].second.first )
		{
		case Resolution:
			if( LEFT && gMenu[gMenuIndex].second.second > 0)
				--gMenu[gMenuIndex].second.second;
			if( RIGHT  && gMenu[gMenuIndex].second.second < 1)
				++gMenu[gMenuIndex].second.second;
			break;

		case Volume:
			if( LEFT && gMenu[gMenuIndex].second.second > 0)
				--gMenu[gMenuIndex].second.second;
			if( RIGHT  && gMenu[gMenuIndex].second.second < 10 )
				++gMenu[gMenuIndex].second.second;

			if( LEFT ||RIGHT )
				switch( gMenuIndex )
				{
				case 3:
					UpdateVolume( Master, gMenu[gMenuIndex].second.second );
					SoundManager::GetInstance()->Play( "MenuChange", SFX );
					break;

				case 4:
					UpdateVolume( Song,	gMenu[gMenuIndex].second.second );
					break;

				case 5:
					UpdateVolume( SFX,	gMenu[gMenuIndex].second.second );
					SoundManager::GetInstance()->Play( "MenuChange", SFX );
					break;
				}

		break;

		case Checkbox:
			if( LEFT && gMenu[gMenuIndex].second.second == 1 )
				--gMenu[gMenuIndex].second.second;
			if( RIGHT  && gMenu[gMenuIndex].second.second == 0 )
				++gMenu[gMenuIndex].second.second;
			break;
		}
	}

	if( CONFIRM && gMenuIndex == gMenu.size() )
		Confirm();
	if( BACK )
		gGotoNextFrame	=	MAIN_MENU_SCREEN;



	gMenuIndex	=	( gMenuIndex < 0 ) ? gMenu.size() : gMenuIndex;
	gMenuIndex	=	( gMenuIndex > gMenu.size() ) ? 0 : gMenuIndex;
}


void OptionScreen::Render()
{
	gGraphicsManager->RenderQuad( gFullscreenVP, gBackground, Effects::CombineFinalFX->AlphaTransparencyColorTech );

	XMFLOAT2	tPos	=	XMFLOAT2(gScreenWidth * 0.05f, gScreenHeight * 0.05f);
	DrawString(*gTextInstance, "Options", tPos.x, tPos.y, 72, White, Black, 2, FW1_LEFT);
	tPos.y	+=	1.2f * 72;
	DrawString(*gTextInstance, "Restart the game when changing graphics!", tPos.x, tPos.y, 16, White, Black, 2, FW1_LEFT);
	tPos.y	+=	2.0f * 72;

	int		i = 0;
	float	gTextSize	=	32;
	for each( OptionEntry Entry in gMenu )
	{
		EntryData	tData		=	Entry.second;
		bool		isActive	=	(i == gMenuIndex);
		TextColor	tTColor		=	isActive ? Green : WhiteTrans;
		TextColor	tBColor		=	isActive ? Black : BlackTrans;

		XMFLOAT2	tPosition	=	XMFLOAT2( tPos.x, gScreenHeight * 0.3f + i * ( 1.5f * gTextSize ) );

		DrawString(*gTextInstance, Entry.first, tPosition.x, tPosition.y, gTextSize, tTColor, tBColor, 2, FW1_LEFT);
		tPosition.x	+=	gTextSize * Entry.first.length();

		if( tData.first == Volume )
		{
			for( int n = 1; n <= 10; ++n )
			{
				if( isActive )
				{
					tTColor		=	( n <= tData.second ) ? Green : WhiteTrans;
					tBColor		=	( n <= tData.second ) ? BlackTrans : BlackTrans;
				}
				else
				{
					tTColor		=	( n <= tData.second ) ? YellowTrans : WhiteTrans;
					tBColor		=	( n <= tData.second ) ? BlackTrans : BlackTrans;
				}
				tPosition.x	=	gScreenWidth * 0.25f;

				DrawString(*gTextInstance, "|", tPosition.x + n * gTextSize, tPosition.y, gTextSize, tTColor, tBColor, 1, FW1_LEFT);

			}
		}
		else if( tData.first == Checkbox )
		{
			tTColor		=	isActive ? Green : WhiteTrans;
			tBColor		=	isActive ? BlackTrans : BlackTrans;
			tPosition.x	=	gScreenWidth * 0.25f;

			string	tText	=	tData.second == 1 ? "YES" : "NO";
			DrawString(*gTextInstance, tText, tPosition.x + gTextSize, tPosition.y, gTextSize, tTColor, tBColor, 1, FW1_LEFT);
		}
		else if( tData.first == Resolution )
		{
			tTColor					=	isActive ? Green : WhiteTrans;
			tBColor					=	isActive ? BlackTrans : BlackTrans;
			tPosition.x				=	gScreenWidth * 0.25f;
			ResolutionEntry	tRes	=	gResolutions[ tData.second ];

			string	tText	=	to_string( (long double)tRes.first ) + "x" + to_string( (long double)tRes.second );

			DrawString(*gTextInstance, tText, tPosition.x + gTextSize, tPosition.y, gTextSize, tTColor, tBColor, 1, FW1_LEFT);
		}

		++i;
	}

	tPos	=	XMFLOAT2(gScreenWidth * 0.05f, gScreenHeight * 0.85f);
	if( gMenuIndex == gMenu.size() )
		DrawString(*gTextInstance, "Apply", tPos.x, tPos.y, 40, White, Black, 2, FW1_LEFT);
	else
		DrawString(*gTextInstance, "Apply", tPos.x, tPos.y, 40, WhiteTrans, BlackTrans, 2, FW1_LEFT);
}

void OptionScreen::UpdateVolume( SoundType VolumeType, int Value )
{
	Value	=	MathHelper::Clamp( Value, 0, 10 );

	SoundManager::GetInstance()->SetVolume( VolumeType, 0.1f * Value );
}

void OptionScreen::Confirm()
{
	//	asd

	//	Set the volumes
	UpdateVolume( Master,	gMenu[3].second.second );
	UpdateVolume( Song,		gMenu[4].second.second );
	UpdateVolume( SFX,		gMenu[5].second.second );

	
	ResolutionEntry	tRes	=	gResolutions[ gMenu[0].second.second ];
	//	Write to file
	ofstream settingsFile;
	settingsFile.open( "DATA/Settings.cfg" );

	settingsFile << "Width " + to_string( (long double)tRes.first ) << endl;
	settingsFile << "Height " + to_string( (long double)tRes.second ) << endl;
	settingsFile << "Fullscreen " + to_string( (long double)gMenu[1].second.second ) << endl;
	settingsFile << "MouseVisible " + to_string( (long double)gMenu[2].second.second ) << endl;
	settingsFile << "Master " + to_string( (long double)gMenu[3].second.second ) << endl;
	settingsFile << "Song " + to_string( (long double)gMenu[4].second.second ) << endl;
	settingsFile << "SFX " + to_string( (long double)gMenu[5].second.second ) << endl;

	settingsFile.close();

	//	Next screen
	gGotoNextFrame	=	MAIN_MENU_SCREEN;
}

ScreenType OptionScreen::GetScreenType()
{
	return OPTIONS_SCREEN;
}