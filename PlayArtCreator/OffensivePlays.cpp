
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
Graphic gReceiver;
Graphic gBlock;

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

void PrintAllOffensivePlays(OffPlayHolder holder)
{
	std::cout << "Num Plays: " << holder.mNumPlays << std::endl;
	std::cout << "-------------------------------------------\n";

	for (int i = 0; i < holder.mNumPlays; i++) {
		std::cout << "Name: " << holder.aPlays[i].mName << std::endl;
		std::cout << "Formation: " << holder.aPlays[i].mFormation << std::endl;
		std::cout << "Num Players: " << holder.aPlays[i].mNumPlayers << std::endl;
		for (int j = 0; j < holder.aPlays[i].mNumPlayers; j++) {
			std::cout << "Tag: " << holder.aPlays[i].aTags[j] << ", Job: " << holder.aPlays[i].aRoles[j] << std::endl;
		}
		std::cout << "Num Receivers: " << holder.aPlays[i].mNumReceivers << std::endl;
		for (int j = 0; j < holder.aPlays[i].mNumReceivers; j++) {
			std::cout << "Owner: " << holder.aPlays[i].aRoutes[j].mOwner;
			for (int k = 0; k < holder.aPlays[i].aRoutes[j].mNumSpots; k++) {
				std::cout << ConvertVecToString(holder.aPlays[i].aRoutes[j].mSpots[k]) << ", ";
			}
			std::cout << "\n";
		}

	}

}

Vec2 GetStartingPositionFromFormation(std::string tag, std::string formation, OffFormationHolder formHolder)
{
	// get the correct formation
	for (int i = 0; i < formHolder.numFormations; i++) {
		if (formHolder.aFormations[i].mName == formation) {
			for (int j = 0; j < formHolder.aFormations[i].mNumPlayers; j++) {
				if (formHolder.aFormations[i].aTags[j] == tag) {
					return formHolder.aFormations[i].aSpots[j];
				}
			}
		}
	}

	Vec2 vFalse; vFalse.x = 0; vFalse.y = 0;
	return vFalse;
}

void CreateAllOffPlayArt(OffPlayHolder plays, OffFormationHolder formations)
{
	for (int i = 0; i < plays.mNumPlays; i++) {
		ScaleApplyImage(&FINAL_PLAY, &gField.img, 0, 0, FINAL_PLAY.wd);
			
		//ScaleApplyImage(&FINAL_PLAY, &gPlayer.img, 50, 50, 10);

		// Need to get the starting position for each player.
		for (int j = 0; j < plays.aPlays[i].mNumPlayers; j++) {
			Vec2 vStart = GetStartingPositionFromFormation(plays.aPlays[i].aTags[j], plays.aPlays[i].mFormation, formations);
			vStart.x *= 2; vStart.y *= 2;
			// Now we need to shove into the center.
			vStart.x += 50; vStart.y += 50;
			
			if (plays.aPlays[i].aRoles[j] == "QB") {
				CenteredScaleApplyImage(&FINAL_PLAY, &gPlayer.img, vStart.x, vStart.y + 2, 4);
			}
			else if (plays.aPlays[i].aRoles[j] == "BLOCK") {
				CenteredScaleApplyImage(&FINAL_PLAY, &gBlock.img, vStart.x, vStart.y + 2, 4);
			}
			else if (plays.aPlays[i].aRoles[j] == "ROUTE") {
				CenteredScaleApplyImage(&FINAL_PLAY, &gReceiver.img, vStart.x, vStart.y + 2, 4);
				RenderRoute(vStart, GetRouteFromPlayTag(plays.aPlays[i].aTags[j], plays.aPlays[i].mName, plays));
			}
		}



		std::string sName = "PlayArt/Offense/" + plays.aPlays[i].mName + ".png";
		WriteImage(FINAL_PLAY, sName);
	}
}

// Shove in WR1, for a given play, and you should get the route data.
DATA_Off_Route GetRouteFromPlayTag(std::string tag, std::string playName, OffPlayHolder plays)
{
	for (int i = 0; i < plays.mNumPlays; i++) {
		if (plays.aPlays[i].mName == playName) {
			for (int j = 0; j < plays.aPlays[i].mNumReceivers; j++) {
				if (plays.aPlays[i].aRoutes[j].mOwner == tag) {
					return plays.aPlays[i].aRoutes[j];
				}
			}
		}
	}

	DATA_Off_Route r; 
	r.mOwner = "NO_ONE";
	return r;
}

void RenderRoute(Vec2 vStart, DATA_Off_Route route)
{
	for (int i = 1; i < route.mNumSpots; i++)
	{
		Vec2 v1 = route.mSpots[i-1];
		v1 = v1 * 2;
		v1.x += vStart.x; v1.y += vStart.y;
		Vec2 v2 = route.mSpots[i];
		v2 = v2 * 2;
		v2.x += vStart.x; v2.y += vStart.y;
		
		Vec2 dis = v2 - v1;
		int numSteps = 10;
		for (int j = 0; j < numSteps; j++) {
			float xStep = (float)dis.x * ((float)j / (float)numSteps);
			float yStep = (float)dis.y * ((float)j / (float)numSteps);
			CenteredScaleApplyImage(&FINAL_PLAY, &gRouteArrow.img, v1.x + xStep, v1.y + yStep, 1);
		}

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
	
	gReceiver.type = "Receiver";
	LoadImage(&gReceiver.img, path + "\\ReceiverAlpha.png");
}


//void CreateAllOffPlayArt()
//{
//	for (int i = 0; i < OffensivePlays.numPlays; i++)
//	{
//		CreateOffensiveFieldPNG(i);
//	}
//}

//// I know that the play art field is 500x500, although I can do the math.
//void CreateOffensiveFieldPNG(int ind)
//{
//	ScaleApplyImage(&FINAL_PLAY, &gField.img, 0, 0, FINAL_PLAY.wd);
//
//	// let's see if we can place all the players in the right spot.
//	for (int i = 0; i < OffensivePlays.aPlays[ind].numPlayers; i++)
//	{
//		// Start is relative to snap, which is always 25 yards in, and 40 yards deep.
//		Vec2 adjPos = OffensivePlays.aPlays[ind].pRoles[i].mStart;
//		adjPos.x = (25 + adjPos.x) * 2;
//		adjPos.y = (40 - adjPos.y) * 2;			// y goes top to bottom
//
//		CenteredScaleApplyImage(&FINAL_PLAY, &gPlayer.img, adjPos.x, adjPos.y, 5);
//
//		// Now we also have to render their assignments.
//		if (OffensivePlays.aPlays[ind].pRoles[i].mRole == "Route") {
//			RenderRoute(OffensivePlays.aPlays[ind].pRoles[i].mDetail, adjPos);
//		}
//		else if (OffensivePlays.aPlays[ind].pRoles[i].mRole == "Pass Block") {
//			CenteredScaleApplyImage(&FINAL_PLAY, &gBlock.img, adjPos.x, adjPos.y + 2, 2);
//		}
//	}
//
//	//ScaleApplyImage(&FINAL_PLAY, &gPlayer.img, 50, 50, 10);
//	std::string sName = "PlayArt/" + OffensivePlays.aPlays[ind].mName + ".png";
//	WriteImage(FINAL_PLAY, sName);
//}

/******************************************************************
Ultimately the solution is to render stretched and rotated graphics, but
that requires too much changes. So just make a little dot trail for now.

Render a solid white line from each point.
Then render an arrow, just a little beyond the last point.
Yes.

The starting position has already been adjusted for.
******************************************************************/
void RenderRouteOld(std::string name, Vec2 vStart)
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

OffPlayHolder LoadOffensivePlays(std::string directoryPath)
{
	int numPlays = 0;
	for (const auto & entry : std::experimental::filesystem::directory_iterator(directoryPath)) {
		std::string filePath = entry.path().string();
		if (filePath[(filePath.length() - 1)] != 'a') {
			numPlays++;
		}
	}

	OffPlayHolder holder;
	holder.mNumPlays = numPlays;
	holder.aPlays = new DATA_Off_Play[numPlays];
	int ix = 0;
	for (const auto & entry : std::experimental::filesystem::directory_iterator(directoryPath)) {
		std::cout << entry.path() << std::endl;
		std::string filePath = entry.path().string();
		if (filePath[(filePath.length() - 1)] == 't') {
			holder.aPlays[ix] = LoadOffensivePlayFromFile(filePath);
			ix++;
		}
	}

	return holder;
}

DATA_Off_Play LoadOffensivePlayFromFile(std::string filePath)
{
	std::ifstream file(filePath);
	std::string str;
	std::vector<std::string> vOfStrings;

	while (std::getline(file, str)) {
		if (str.size() > 0) {
			vOfStrings.push_back(str);
		}
	}

	DATA_Off_Play p;
	// It's safer to first allocate any memory for the size of players/tags.
	// Also for the number of "routes"
	for (int i = 0; i < vOfStrings.size(); i++) {
		if (vOfStrings[i].find("NUM PLAYERS") != std::string::npos) {
			p.mNumPlayers = std::stoi(vOfStrings[i + 1]);
			p.aRoles = new std::string[p.mNumPlayers];
			p.aTags = new std::string[p.mNumPlayers];
		}

		if (vOfStrings[i].find("NUM REC") != std::string::npos) {
			p.mNumReceivers = std::stoi(vOfStrings[i + 1]);
			p.aRoutes = new DATA_Off_Route[p.mNumReceivers];
		}
	}

	// Now we can safely allocate data into our play.
	for (int i = 0; i < vOfStrings.size(); i++)
	{
		if (vOfStrings[i].find("NAME") != std::string::npos) {
			p.mName = vOfStrings[i + 1];
		}

		if (vOfStrings[i].find("FORMA") != std::string::npos) {
			p.mFormation = vOfStrings[i + 1];
		}

		if (vOfStrings[i].find("NUM PLAYERS") != std::string::npos) {
			int ix = i + 2;
			for (int j = 0; j < p.mNumPlayers; j++)
			{
				p.aTags[j] = vOfStrings[ix++];
				p.aRoles[j] = vOfStrings[ix++];
			}
		}

		if (vOfStrings[i].find("ROUTES") != std::string::npos) {
			int ix = i + 1;
			for (int j = 0; j < p.mNumReceivers; j++) {
				p.aRoutes[j].mOwner = vOfStrings[ix++];
				ix++;
				p.aRoutes[j].mNumSpots = std::stoi(vOfStrings[ix++]);
				p.aRoutes[j].mSpots = new Vec2[p.aRoutes[j].mNumSpots];
				for (int k = 0; k < p.aRoutes[j].mNumSpots; k++) {
					p.aRoutes[j].mSpots[k] = GetVecFromString(vOfStrings[ix++]);
				}
			}
		}

	}
	
	return p;
}



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

