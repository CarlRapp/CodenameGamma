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
}

void DebugScreen::Update(float DeltaTime)
{
	//	Page UP/DOWN is pressed, scroll the debug log
	if ( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_PRIOR ) == PRESSED || InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_NEXT ) == PRESSED )
	{
		//	Only if there are more messages
		//	than the total shown
		if ( gLogMaxRows < gDebugLog->size() )
		{
			int Direction	=	( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_PRIOR ) == PRESSED ) ? -1 : 1;

			gLogIndex	+=	Direction;

			gLogIndex	=	( gLogIndex + gLogMaxRows > gDebugLog->size() ) ? gLogIndex - 1 : gLogIndex;
			gLogIndex	=	( gLogIndex < 0 ) ? 0 : gLogIndex;
		}
	}

	//	Increase the number of messages shown
	if ( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_DOWN ) == PRESSED )
		gLogMaxRows	=	( gLogMaxRows + 1 > gDebugLog->size() ) ? gLogMaxRows : gLogMaxRows + 1;

	//	Decrease the number of messages shown
	else if ( InputManager::GetInstance()->GetKeyboard()->GetKeyState( VK_UP ) == PRESSED )
		gLogMaxRows	=	( gLogMaxRows - 1 < 1 ) ? 1 : gLogMaxRows - 1;
}

void DebugScreen::Render()
{
	for(UINT32 i = gLogIndex; i <= (gLogIndex + gLogMaxRows); ++i)
	{
		if ( i >= gDebugLog->size() || i == (gLogIndex + gLogMaxRows) )
		{
			DrawString(*gTextInstance, "Displaying " + to_string((long double)((gLogMaxRows > gDebugLog->size() ? gDebugLog->size() : gLogMaxRows))) + " of " + to_string((long double)gDebugLog->size()) + " messages.", gMarginX, gMarginY + (i - gLogIndex) * (gTextSize + gTextSize * 0.1f), gTextSize, White, 0);
			break;
		}

		LogData*	tData	=	gDebugLog->at( i );

		DrawString(*gTextInstance, tData->MESSAGE, gMarginX, gMarginY + (i - gLogIndex) * (gTextSize + gTextSize * 0.1f), gTextSize, tData->COLOR, 0);
	}
	
	
	for(UINT32 i = 0; i < gDebugData->size(); ++i)
	{
		DebugData*	tData	=	gDebugData->at( i );
		string	Title	=	tData->Title;
		string	Value	=	tData->Value;

		//	If the colors are the same
		//	only make one DrawString call
		if(tData->TitleColor == tData->ValueColor)
			DrawString(*gTextInstance, Title + Value, gScreenWidth - gMarginX, 5.0f + i * (gTextSize + gTextSize * 0.1f), gTextSize, tData->TitleColor, FW1_RIGHT);
		else
		{
			DrawString(*gTextInstance, Title, gScreenWidth - gMarginX - GetTextWidth(Value, gTextSize), gMarginY + i * (gTextSize + gTextSize * 0.1f), gTextSize, tData->TitleColor, FW1_RIGHT);
			DrawString(*gTextInstance, Value, gScreenWidth - gMarginX, gMarginY + i * (gTextSize + gTextSize * 0.1f), gTextSize, tData->ValueColor, FW1_RIGHT);
		}
	}
}


void DebugScreen::DrawString(IFW1FontWrapper& Instance, string Text, float x, float y, float Size, TextColor Color, UINT Flags)
{
	Instance.DrawString(
		gDeviceContext, 
		wstring(Text.begin(), Text.end()).c_str(), 
		floorf(Size * 100 + 0.5) * 0.01f, 
		floorf(x * 100 + 0.5) * 0.01f, 
		floorf(y * 100 + 0.5) * 0.01f, 
		Color, 
		Flags
	);
}

void DebugScreen::DrawString(IFW1FontWrapper& Instance, string Text, float x, float y, float Size, TextColor Color, TextColor BorderColor, float BorderSize, UINT Flags)
{
	DrawString(Instance, Text, x-BorderSize, y, Size, BorderColor, Flags);
	DrawString(Instance, Text, x+BorderSize, y, Size, BorderColor, Flags);
	DrawString(Instance, Text, x, y-BorderSize, Size, BorderColor, Flags);
	DrawString(Instance, Text, x, y+BorderSize, Size, BorderColor, Flags);

	DrawString(Instance, Text, x, y, Size, Color, Flags);
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