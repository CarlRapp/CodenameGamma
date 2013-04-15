#include "DebugScreen.h"

DebugScreen::DebugScreen(ScreenData* Setup)
{
	gDebugData	=	new vector<DebugData*>();
	gTextSize	=	18;

	LoadScreenData(Setup);
}

bool DebugScreen::Load()
{
	return true;
}

bool DebugScreen::Unload()
{
	for(UINT32 i = 0; i < gDebugData->size(); ++i)
		delete	gDebugData->at(i);
	gDebugData->clear();
	delete gDebugData;

	return true;
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

void DebugScreen::Update(float DeltaTime)
{

}

void DebugScreen::Render()
{
	for(UINT32 i = 0; i < gDebugData->size(); ++i)
	{
		DebugData*	tData	=	gDebugData->at( i );
		string	Title	=	tData->Title;
		string	Value	=	tData->Value;

		//	If the colors are the same
		//	only make one DrawString call
		if(tData->TitleColor == tData->ValueColor)
			DrawString(*gTextInstance, Title + Value, 5.0f, 5.0f + i * (gTextSize + gTextSize * 0.1f), gTextSize, tData->TitleColor, 0);
		else
		{
			DrawString(*gTextInstance, Title, 5.0f, 5.0f + i * (gTextSize + gTextSize * 0.1f), gTextSize, tData->TitleColor, 0);
			DrawString(*gTextInstance, Value, 5.0f + GetTextWidth(Title, gTextSize), 5.0f + i * (gTextSize + gTextSize * 0.1f), gTextSize, tData->ValueColor, 0);
		}
	}
}