
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "OffensivePlays.h"
#include "Structs.h"

#include "Globals.h"

/***********************************************************************************
Alrighty, the data in a play has changed, which means getting that data is going to
require us to rewrite this.
***********************************************************************************/


OffPlayHolder OffensivePlays;
OffFormationHolder rFormationHolder;
RouteHolder rHolder;

Graphic gPlayer;
Graphic gRouteBase;
Graphic gRouteArrow;
Graphic gBlock;

void PrintAllOffensivePlays()
{
	std::cout << "Num Plays Loaded: " << OffensivePlays.numPlays << std::endl;
	for (int i = 0; i < OffensivePlays.numPlays; i++)
	{
		std::cout << "Name of Play: " << OffensivePlays.aPlays[i].mName << std::endl;
		for (int j = 0; j < OffensivePlays.aPlays[i].numPlayers; j++)
		{
			std::cout << "Tag: " << OffensivePlays.aPlays[i].pRoles[j].mTag << " Role: " << OffensivePlays.aPlays[i].pRoles[j].mRole << " Detail: " << OffensivePlays.aPlays[i].pRoles[j].mDetail;
			std::cout << "\n";
		}
		std::cout << "\n\n";
	}

}

void PrintAllRoutes()
{
	std::cout << "Num Routes loaded: " << rHolder.numRoutes << std::endl;
	for (int i = 0; i < rHolder.numRoutes; i++)
	{
		std::cout << "Name of Route: " << rHolder.aRoutes[i].mName << std::endl;
		for (int j = 0; j < rHolder.aRoutes[i].numSpots; j++)
		{
			std::cout << "(" << rHolder.aRoutes[i].aSpots[j].x << "," << rHolder.aRoutes[i].aSpots[j].y << ")";
		}

		std::cout << std::endl;
	}
}

//Loads the specific images that we need
void LoadImages(std::string path)
{
	gBlock.type = "Pass Block";
	LoadImage(&gBlock.img, path + "\\PBlock.png");

	gPlayer.type = "Player";
	LoadImage(&gPlayer.img, path + "\\Player.png");

	gField.type = "Field";
	LoadImage(&gField.img, path + "\\Field.png");

	gRouteBase.type = "RouteBase";
	LoadImage(&gRouteBase.img, path + "\\RouteBase.png");

	gRouteArrow.type = "RouteArrow";
	LoadImage(&gRouteArrow.img, path + "\\RouteArrow.png");
}


void CreateAllOffPlayArt()
{
	for (int i = 0; i < OffensivePlays.numPlays; i++)
	{
		CreateOffensiveFieldPNG(i);
	}
}

// I know that the play art field is 500x500, although I can do the math.
void CreateOffensiveFieldPNG(int ind)
{
	ScaleApplyImage(&FINAL_PLAY, &gField.img, 0, 0, FINAL_PLAY.wd);

	// let's see if we can place all the players in the right spot.
	for (int i = 0; i < OffensivePlays.aPlays[ind].numPlayers; i++)
	{
		// Start is relative to snap, which is always 25 yards in, and 40 yards deep.
		Vec2 adjPos = OffensivePlays.aPlays[ind].pRoles[i].mStart;
		adjPos.x = (25 + adjPos.x) * 2;
		adjPos.y = (40 - adjPos.y) * 2;			// y goes top to bottom

		CenteredScaleApplyImage(&FINAL_PLAY, &gPlayer.img, adjPos.x, adjPos.y, 5);

		// Now we also have to render their assignments.
		if (OffensivePlays.aPlays[ind].pRoles[i].mRole == "Route") {
			RenderRoute(OffensivePlays.aPlays[ind].pRoles[i].mDetail, adjPos);
		}
		else if (OffensivePlays.aPlays[ind].pRoles[i].mRole == "Pass Block") {
			CenteredScaleApplyImage(&FINAL_PLAY, &gBlock.img, adjPos.x, adjPos.y + 2, 2);
		}
	}

	//ScaleApplyImage(&FINAL_PLAY, &gPlayer.img, 50, 50, 10);
	std::string sName = "PlayArt/" + OffensivePlays.aPlays[ind].mName + ".png";
	WriteImage(FINAL_PLAY, sName);
}

/******************************************************************
Ultimately the solution is to render stretched and rotated graphics, but
that requires too much changes. So just make a little dot trail for now.

Render a solid white line from each point.
Then render an arrow, just a little beyond the last point.
Yes.

The starting position has already been adjusted for.
******************************************************************/
void RenderRoute(std::string name, Vec2 vStart)
{
	DATA_Route rt;
	for (int i = 0; i < rHolder.numRoutes; i++)
	{
		if (rHolder.aRoutes[i].mName == name) {
			rt = rHolder.aRoutes[i];
		}
	}

	// Render an image, from one spot to the next.
	for (int i = 0; i < rt.numSpots; i++)
	{
		// if we're at the end, place the arrow. Shit, have to rotate the arrow.
		if (i + 1 == rt.numSpots)
		{
		}
		else {
			// unfortunately, since y is inverted, we have to do this manually
			Vec2 startPos = vStart;
			startPos.x += rt.aSpots[i].x;
			startPos.y -= rt.aSpots[i].y;
			//Vec2 startPos = vStart + rt.aSpots[i] * (PLAY_SIZE_PIXELS / 50);
			Vec2 endPos = vStart;
			endPos.x += rt.aSpots[i + 1].x;
			endPos.y -= rt.aSpots[i + 1].y;

			CenteredScaleApplyImage(&FINAL_PLAY, &gRouteArrow.img, startPos.x, startPos.y, 2);
			CenteredScaleApplyImage(&FINAL_PLAY, &gRouteArrow.img, endPos.x, endPos.y, 2);
			Vec2 dis;
			dis = endPos - startPos;
			int numSteps = 10;
			for (int j = 0; j < numSteps; j++) {
				float xStep = (float)dis.x * ((float)j / (float)numSteps);
				float yStep = (float)dis.y * ((float)j / (float)numSteps);
				CenteredScaleApplyImage(&FINAL_PLAY, &gRouteArrow.img, startPos.x + xStep, startPos.y + yStep, 2);
			}
		}
	}
}

bool LoadRoutes(RouteHolder* pRouteHolder, std::string filePath)
{
	std::ifstream file("routes.txt");
	std::string str;
	std::getline(file, str);
	int numRoutes = std::stoi(str);
	pRouteHolder->aRoutes = new DATA_Route[numRoutes];
	pRouteHolder->numRoutes = numRoutes;
	for (int i = 0; i < numRoutes; i++)
	{
		std::getline(file, str);
		pRouteHolder->aRoutes[i].mName = str;

		// now here's where we have to format this stuff.
		std::getline(file, str);
		int numSpots = std::stoi(str);
		pRouteHolder->aRoutes[i].numSpots = numSpots;
		pRouteHolder->aRoutes[i].aSpots = new Vec2[numSpots];
		std::getline(file, str);
		int ind = 0;
		int endInd;
		for (int j = 0; j < numSpots; j++)
		{
			endInd = str.find(')', ind);
			// alright, read in until the ), strip away all else, then convert what remains to an int.
			std::string sVec = str.substr(ind, 1 + endInd - ind);

			pRouteHolder->aRoutes[i].aSpots[j] = GetVecFromString(sVec);

			ind = endInd + 1;
		}
	}

	return false;
}

/**********************************************************************************
Alrighty now! There are two seperate problems here. 
1) Get some way of loading in all the text files individually.
2) Read the data from the individual text file into the formation struct.
**********************************************************************************/
OffFormationHolder LoadOffensiveFormations(std::string directoryPath)
{
	// First iterate through, getting the number of plays. 
	int numFormations = 0;
	// ooooooohhhhhh. experimental! Fancy!
	for (const auto & entry : std::experimental::filesystem::directory_iterator(directoryPath)) {
		std::string filePath = entry.path().string();
		if (filePath[(filePath.length() - 1)] == 'a') {
			std::cout << "This is a meta file" << std::endl;
		}
		else {
			numFormations++;
		}
	}

	// Now we actually store those plays.
	OffFormationHolder holder;
	holder.numFormations = numFormations;
	holder.aFormations = new DATA_Off_Formation[numFormations];
	int ix = 0;
	for (const auto & entry : std::experimental::filesystem::directory_iterator(directoryPath)) {
		//std::cout << entry.path() << std::endl;
		std::string filePath = entry.path().string();
		if (filePath[(filePath.length() - 1)] == 't') {
			holder.aFormations[ix] = LoadFormationFromFile(filePath);
			ix++;
		}
	}

	return holder;
}

// Yes, you can return from the stack. This results in a copy being made into the waiting object.
DATA_Off_Formation LoadFormationFromFile(std::string filePath)
{
	std::ifstream file(filePath);
	std::string str;
	std::vector<std::string> vOfStrings;

	while (std::getline(file, str)) {
		if (str.size() > 0) {
			vOfStrings.push_back(str);
		}
	}

	for (int i = 0; i < vOfStrings.size(); i++) {
		//std::cout << vOfStrings[i] << std::endl;
	}

	DATA_Off_Formation f;
	// It's safer to first allocate any memory for the size of players/tags.
	for (int i = 0; i < vOfStrings.size(); i++) {
		if (vOfStrings[i].find("NUM PLAYERS") != std::string::npos) {
			f.mNumPlayers = std::stoi(vOfStrings[i + 1]);
			std::cout << "Number of players: " << f.mNumPlayers << std::endl;
			f.aSpots = new Vec2[f.mNumPlayers];
			f.aTags = new std::string[f.mNumPlayers];
		}
	}
	// Now we can safely allocate data into our formation.
	for (int i = 0; i < vOfStrings.size(); i++)
	{
		if (vOfStrings[i].find("NAME") != std::string::npos) {
			f.mName = vOfStrings[i + 1];
			std::cout << "Name: " << f.mName << std::endl;
		}

		if (vOfStrings[i].find("POSITIONS AND ") != std::string::npos) {
			int ix = i + 1;
			for (int j = 0; j < f.mNumPlayers; j++)
			{
				f.aTags[j] = vOfStrings[ix++];
				f.aSpots[j] = GetVecFromString(vOfStrings[ix++]);
				std::cout << f.aTags[j] << std::endl;
			}
		}

	}

	return f;
}


bool LoadOffensivePlays(std::string filePath)
{
	std::ifstream file("plays.txt");
	std::string str;
	std::getline(file, str);
	int numPlays = std::stoi(str);
	OffensivePlays.aPlays = new DATA_Off_Play[numPlays];
	OffensivePlays.numPlays = numPlays;
	for (int i = 0; i < numPlays; i++)
	{
		std::getline(file, str);
		OffensivePlays.aPlays[i].mName = str;

		// now here's where we have to format this stuff.
		std::getline(file, str);
		int numJobs = std::stoi(str);
		OffensivePlays.aPlays[i].numPlayers = numJobs;
		OffensivePlays.aPlays[i].pRoles = new DATA_PlayRole[numJobs];

		for (int j = 0; j < numJobs; j++)
		{
			std::getline(file, str);
			OffensivePlays.aPlays[i].pRoles[j].mTag = str;
			std::getline(file, str);
			OffensivePlays.aPlays[i].pRoles[j].mRole = str;
			std::getline(file, str);
			OffensivePlays.aPlays[i].pRoles[j].mDetail = str;
			std::getline(file, str);
			OffensivePlays.aPlays[i].pRoles[j].mStart = GetVecFromString(str);
		}
	}

	return false;
}

//bool LoadOffensivePlays(std::string filePath, bool second)
//{
//
//	return false;
//}



/***********************************************
LEAVING AS REFERENCE
void CreateBoardPNG()
{
//blit the white pieces
float xOffset = 0; float yOffset = 87.5;
int correctPiece;

//blit the board as the background.
ScaleApplyImage(&FINAL_BOARD, &imgs[10].img, 0, 0, 100);		//should really not use magic numbers for the board location.

for (int i = 0; i<8; i++) {
correctPiece = ChoosePieceToRender(i, true);
ScaleApplyImage(&FINAL_BOARD, &imgs[correctPiece].img, xOffset, yOffset, 12.5);
xOffset += 12.5;
}
//add pawns
xOffset = 0; yOffset = 75;
for (int i = 0; i<8; i++) {
ScaleApplyImage(&FINAL_BOARD, &imgs[FindPawn(true)].img, xOffset, yOffset, 12.5);
xOffset += 12.5;
}
//blit black pieces
xOffset = 0; yOffset = 0;
correctPiece = 0;

for (int i = 0; i<8; i++) {
correctPiece = ChoosePieceToRender(i, false);
ScaleApplyImage(&FINAL_BOARD, &imgs[correctPiece].img, xOffset, yOffset, 12.5);
xOffset += 12.5;
}
//add pawns
xOffset = 0; yOffset = 12.5;
for (int i = 0; i<8; i++) {
ScaleApplyImage(&FINAL_BOARD, &imgs[12].img, xOffset, yOffset, 12.5);
xOffset += 12.5;
}

WriteImage(FINAL_BOARD, "full board.png");
}
********************************/

