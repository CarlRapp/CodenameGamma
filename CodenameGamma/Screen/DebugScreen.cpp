#include "ScreenEnums.h"

DebugScreen*	DebugScreen::gInstance	=	0;
DebugScreen::DebugScreen(ScreenData* Setup)
{
	gDebugData	=	new vector<DebugData*>();
	gDebugLog	=	new vector<LogData*>();
	gTextSize	=	14;

	gTextInstance	=	Setup->TEXT_INSTANCE;
	gDeviceContext	=	Setup->DEVICE_CONTEXT;

	gScreenWidth	=	Setup->WIDTH;
	gScreenHeight	=	Setup->HEIGHT;

	gMarginX	=	(int)(gScreenWidth * 0.01f);
	gMarginY	=	(int)(gScreenHeight * 0.01f);

	gLogIndex	=	0;
	gLogMaxRows	=	10;
}

DebugScreen::~DebugScreen()
{
	for(unsigned int i = 0; i < gDebugData->size(); ++i)
		delete	gDebugData->at(i);
	gDebugData->clear();
	delete gDebugData;

	for(unsigned int i = 0; i < gDebugLog->size(); ++i)
		delete	gDebugLog->at(i);
	gDebugLog->clear();
	delete gDebugLog;
}

DebugScreen* DebugScreen::GetInstance()
{
	return gInstance;
}

void DebugScreen::Initialize(ScreenData* Setup)
{
	if ( gInstance )
		return;

	gInstance	=	new DebugScreen(Setup);
}

DebugData*	DebugScreen::AddDebugData(string Title)
{
	DebugData*	tDData	=	new DebugData();

	tDData->Title	=	Title;
	tDData->Value	=	"";

	tDData->TitleColor	=	White;
	tDData->ValueColor	=	Green;


	gDebugData->push_back(tDData);
	return	tDData;
}

void DebugScreen::RemoveDebugData(DebugData* Instance, bool DeletePointer)
{
	for ( unsigned int i = 0; i < gDebugData->size(); ++i )
		if ( gDebugData->at(i) == Instance )
		{
			gDebugData->erase( gDebugData->begin() + i );

			if ( DeletePointer )
				delete Instance;
		}
}

void DebugScreen::AddLogMessage(string Message, TextColor Color)
{
	gDebugLog->push_back( new LogData(Message, Color) );

	gLogIndex	+=	1;
	gLogIndex	=	( gLogIndex + gLogMaxRows > (int)gDebugLog->size() ) ? gLogIndex - 1 : gLogIndex;
}

void DebugScreen::Update(float DeltaTime)
{
	//	Page UP/DOWN is pressed, scroll the debug log
	if ( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_PRIOR ) == PRESSED || InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_NEXT ) == PRESSED )
	{
		//	Only if there are more messages
		//	than the total shown
		if ( gLogMaxRows < (int)gDebugLog->size() )
		{
			int Direction	=	( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_PRIOR ) == PRESSED ) ? -1 : 1;

			gLogIndex	+=	Direction;

			gLogIndex	=	( gLogIndex + gLogMaxRows > (int)gDebugLog->size() ) ? gLogIndex - 1 : gLogIndex;
			gLogIndex	=	( gLogIndex < 0 ) ? 0 : gLogIndex;
		}
	}

	//	Increase the number of messages shown
	if ( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_DOWN ) == PRESSED )
		gLogMaxRows	=	( gLogMaxRows + 1 > (int)gDebugLog->size() ) ? gLogMaxRows : gLogMaxRows + 1;

	//	Decrease the number of messages shown
	else if ( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_UP ) == PRESSED )
		gLogMaxRows	=	( gLogMaxRows - 1 < 1 ) ? 1 : gLogMaxRows - 1;
}

void DebugScreen::Render()
{
	for(int i = gLogIndex; i <= (gLogIndex + gLogMaxRows); ++i)
	{
		if ( i >= (int)gDebugLog->size() || i == (gLogIndex + gLogMaxRows) )
		{
			DrawString(
				"Displaying " + to_string((long double)((gLogMaxRows > (int)gDebugLog->size() ? (int)gDebugLog->size() : gLogMaxRows))) + " of " + to_string((long double)gDebugLog->size()) + " messages.", 
				gMarginX, 
				(int)(gMarginY + (i - gLogIndex) * (gTextSize + gTextSize * 0.1f)), 
				(float)gTextSize,
				White, 
				0
			);
			
			break;
		}

		LogData*	tData	=	gDebugLog->at( i );

		DrawString(
			tData->MESSAGE, 
			gMarginX, 
			(int)(gMarginY + (i - gLogIndex) * (gTextSize + gTextSize * 0.1f)), 
			(float)gTextSize, 
			tData->COLOR, 
			0
		);
	}
	
	
	for(UINT32 i = 0; i < gDebugData->size(); ++i)
	{
		DebugData*	tData	=	gDebugData->at( i );
		string	Title	=	tData->Title;
		string	Value	=	tData->Value;

		//	If the colors are the same
		//	only make one DrawString call
		if(tData->TitleColor == tData->ValueColor)
			DrawString(
				Title + Value, 
				gScreenWidth - gMarginX, 
				(int)(5 + i * (gTextSize + gTextSize * 0.1f)), 
				(float)gTextSize, 
				tData->TitleColor, 
				FW1_RIGHT
			);
		else
		{
			DrawString(
				Title,
				(int)(gScreenWidth - gMarginX - GetTextWidth(Value, (float)gTextSize)), 
				(int)(gMarginY + i * (gTextSize + gTextSize * 0.1f)), 
				(float)gTextSize,
				tData->TitleColor, 
				FW1_RIGHT
			);

			DrawString(
				Value, 
				gScreenWidth - gMarginX,
				(int)(gMarginY + i * (gTextSize + gTextSize * 0.1f)), 
				(float)gTextSize, 
				tData->ValueColor, 
				FW1_RIGHT
			);
		}
	}
}

void DebugScreen::DrawString(string Text, int x, int y, float Size, TextColor Color, UINT Flags)
{
	(*gTextInstance).DrawString(
		gDeviceContext, 
		wstring(Text.begin(), Text.end()).c_str(), 
		(float)floorf(Size * 100 + 0.5f) * 0.01f, 
		(float)floorf(x * 100 + 0.5f) * 0.01f, 
		(float)floorf(y * 100 + 0.5f) * 0.01f, 
		Color, 
		Flags
	);
}
void DebugScreen::DrawString(string Text, int x, int y, float Size, TextColor Color, TextColor BorderColor, int BorderSize, UINT Flags)
{
	DrawString(Text, x-BorderSize, y, Size, BorderColor, Flags);
	DrawString(Text, x+BorderSize, y, Size, BorderColor, Flags);
	DrawString(Text, x, y-BorderSize, Size, BorderColor, Flags);
	DrawString(Text, x, y+BorderSize, Size, BorderColor, Flags);

	DrawString(Text, x, y, Size, Color, Flags);
}

float DebugScreen::GetTextWidth(string Text, float TextSize)
{
	float	tWidth	=	0.0f;

	for(unsigned int i = 0; i < Text.size(); ++i)
	{
		char	tC	=	Text[i];

		if( tC == ' ' )
			tWidth	+=	TextSize * 0.25f;
		else if( tC == '(' || tC == ')' )
			tWidth	+=	TextSize * 0.369f;
		else if( tC == '?' )
			tWidth	+=	TextSize * 0.369f;
		else
			tWidth	+=	isupper(tC) ? TextSize * 0.755f : TextSize * 0.4625f;
	}

	return tWidth;
}