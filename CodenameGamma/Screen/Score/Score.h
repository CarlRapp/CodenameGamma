#pragma once
#ifndef SCORE_H
#define SCORE_H

struct PlayerScore
{
	PlayerScore()
	{
		PlayerIndex		=	-1;
		PlayScore		=	0;
		PlayKillCount	=	0;
		PlayTime		=	0.0f;
	}
	PlayerScore(int Index)
	{
		PlayScore		=	0;
		PlayKillCount	=	0;
		PlayTime		=	0.0f;
		PlayerIndex		=	Index;
	}
	

	int		PlayerIndex;
	int		PlayScore;
	int		PlayKillCount;
	float	PlayTime;

	bool	IsValid()
	{ 
		return PlayerIndex >= 0 && PlayerIndex <= 3;
	}
};

#endif