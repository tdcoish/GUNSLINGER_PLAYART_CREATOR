
#include <iostream>
#include <fstream>

#include "Structs.h"
#include "DefensivePlays.h"
#include "Globals.h"

ZoneHolder ZoneList;
DefPlayHolder DefPlayList;

Graphic gDefender;
Graphic gZone;

bool LoadZones(std::string filePath)
{
	std::ifstream file("zones.txt");
	std::string str;
	std::getline(file, str);
	int numZones = std::stoi(str);
	ZoneList.aZones = new DATA_Zone[numZones];
	ZoneList.numZones = numZones;
	for (int i = 0; i < numZones; i++)
	{
		std::getline(file, str);
		ZoneList.aZones[i].mName = str;
		std::getline(file, str);
		ZoneList.aZones[i].mSpot = GetVecFromString(str);
	}

	return false;
}

bool LoadDefensivePlays(std::string filePath)
{
	std::ifstream file("defPlays.txt");
	std::string str;
	std::getline(file, str);
	int numPlays = std::stoi(str);
	DefPlayList.aPlays = new DATA_Def_Play[numPlays];
	DefPlayList.numPlays = numPlays;
	for (int i = 0; i < numPlays; i++)
	{
		std::getline(file, str);
		DefPlayList.aPlays[i].mName = str;

		// now here's where we have to format this stuff.
		std::getline(file, str);
		int numJobs = std::stoi(str);
		DefPlayList.aPlays[i].numPlayers = numJobs;
		DefPlayList.aPlays[i].pRoles = new DATA_PlayRole[numJobs];

		for (int j = 0; j < numJobs; j++)
		{
			std::getline(file, str);
			DefPlayList.aPlays[i].pRoles[j].mTag = str;
			std::getline(file, str);
			DefPlayList.aPlays[i].pRoles[j].mRole = str;
			std::getline(file, str);
			DefPlayList.aPlays[i].pRoles[j].mDetail = str;
			std::getline(file, str);
			DefPlayList.aPlays[i].pRoles[j].mStart = GetVecFromString(str);
		}
	}

	return false;
}

void PrintAllZones()
{
	for (int i = 0; i < ZoneList.numZones; i++)
	{
		std::cout << ZoneList.aZones[i].mName << std::endl;
		std::cout << ConvertVecToString(ZoneList.aZones[i].mSpot) << std::endl;
	}
}

void PrintAllDefensivePlays()
{
	std::cout << "Num Plays Loaded: " << DefPlayList.numPlays << std::endl;
	for (int i = 0; i < DefPlayList.numPlays; i++)
	{
		std::cout << "Name of Play: " << DefPlayList.aPlays[i].mName << std::endl;
		for (int j = 0; j < DefPlayList.aPlays[i].numPlayers; j++)
		{
			std::cout << "Tag: " << DefPlayList.aPlays[i].pRoles[j].mTag << " Role: " << DefPlayList.aPlays[i].pRoles[j].mRole << " Detail: " << DefPlayList.aPlays[i].pRoles[j].mDetail;
			std::cout << "\n";
		}
		std::cout << "\n\n";
	}
}

void LoadDefGraphics(std::string path)
{
	gDefender.type = "Defender";
	LoadImage(&gDefender.img, path + "\\Defender.png");
}

void CreateAllDefensivePlayArt()
{
	for (int i = 0; i < DefPlayList.numPlays; i++)
	{
		CreateDefensiveFieldPNG(i);
	}
}

// I know that the play art field is 500x500, although I can do the math.
void CreateDefensiveFieldPNG(int ind)
{
	ScaleApplyImage(&FINAL_PLAY, &gField.img, 0, 0, FINAL_PLAY.wd);

	// let's see if we can place all the players in the right spot.
	for (int i = 0; i < DefPlayList.aPlays[ind].numPlayers; i++)
	{
		// Start is relative to snap, which is always 25 yards in, and 40 yards deep.
		Vec2 adjPos = DefPlayList.aPlays[ind].pRoles[i].mStart;
		adjPos.x = (25 + adjPos.x) * 2;
		adjPos.y = (40 - adjPos.y) * 2;			// y goes top to bottom

		CenteredScaleApplyImage(&FINAL_PLAY, &gDefender.img, adjPos.x, adjPos.y, 5);

		//// Now we also have to render their assignments.
		//if (DefPlayList.aPlays[ind].pRoles[i].mRole == "Route") {
		//	RenderRoute(DefPlayList.aPlays[ind].pRoles[i].mDetail, adjPos);
		//}
		//else if (DefPlayList.aPlays[ind].pRoles[i].mRole == "Pass Block") {
		//	CenteredScaleApplyImage(&FINAL_PLAY, &gBlock.img, adjPos.x, adjPos.y + 2, 2);
		//}
	}

	//ScaleApplyImage(&FINAL_PLAY, &gPlayer.img, 50, 50, 10);
	std::string sName = "PlayArt/Defense/" + DefPlayList.aPlays[ind].mName + ".png";
	WriteImage(FINAL_PLAY, sName);
}




