/***********************************************************************************
Loads in all Offensive Plays created in the PE_Editor scene of Unity. I've already 
converted them to text files, so we just load them all in, then make play art for them.

The play art is very... programmer-ish, but it gets the job done as a sort of placeholder.

The generated playart will be displayed when the player is picking plays.

Alright, parts of this program.

1) All the stuff about writing to an image.
2) The parts about loading a play in.
3) Shoving the play into the right part of the image.
4) Vec2, I'll make a float version.
***********************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "time.h"
#include <string>

#include "Blit.h"
#include "Structs.h"



PlayHolder pHolder;
RouteHolder rHolder;

Graphic gField;
Graphic gPlayer;
Graphic gRouteBase;
Graphic gRouteArrow;
Graphic gBlock;

Image FINAL_PLAY;
const int PLAY_SIZE_PIXELS = 500;		//500x500 pixels.

float scale;		//used for graphics.

// we assume that we are getting a vector2 in the format of (x,y)
Vec2 GetVecFromString(std::string sVec)
{
	
	int commaLoc = sVec.find(',');
	std::string xVal = sVec.substr(1, commaLoc - 1);
	int endBrack = sVec.find(')');
	std::string yVal = sVec.substr(commaLoc + 1, endBrack - commaLoc - 1);

	Vec2 vec;
	vec.x = std::stoi(xVal);
	vec.y = std::stoi(yVal);

	return vec;
}

bool LoadRoutes(RouteHolder* pRouteHolder, std::string filePath);

bool LoadPlays(PlayHolder* pPlayHolder, std::string filePath);

void LoadImages(std::string path);

void CreateAllPlayArt();
void CreateFieldPNG(int ind);

void PrintAllRoutes();
void PrintAllPlays();

void RenderRoute(std::string name, Vec2 vStart);

std::string bsPth = "C:/Users/Timothy/Documents/Visual Studio 2017/Projects/PlayArtCreator/GUNSLINGER_PLAYART_CREATOR/PlayArtCreator";
std::string imgPath(bsPth + "/Graphics");


int main(int args, char** argc)
{
	std::cout << "Base Path: " << bsPth << std::endl;

	CreateImage(&FINAL_PLAY, PLAY_SIZE_PIXELS);

	std::string sSavePath = bsPth + "New Plays/";
	std::cout << sSavePath;

	LoadImages(imgPath);

	LoadRoutes(&rHolder, bsPth+"/routes.txt");
	LoadPlays(&pHolder, bsPth + "/plays.txt");

	//PrintAllRoutes();
	//PrintAllPlays();

	// Finally, the time has come to actually print out the play art.
	CreateAllPlayArt();

	//CreateBoardPNG();

	//std::getchar();

	return 0;
}

void PrintAllPlays()
{
	std::cout << "Num Plays Loaded: " << pHolder.numPlays << std::endl;
	for (int i = 0; i < pHolder.numPlays; i++)
	{
		std::cout << "Name of Play: " << pHolder.aPlays[i].mName << std::endl;
		for (int j = 0; j < pHolder.aPlays[i].numPlayers; j++)
		{
			std::cout << "Tag: " << pHolder.aPlays[i].pRoles[j].mTag << " Role: " << pHolder.aPlays[i].pRoles[j].mRole << " Detail: " << pHolder.aPlays[i].pRoles[j].mDetail;
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


void CreateAllPlayArt()
{
	for (int i = 0; i < pHolder.numPlays; i++)
	{
		CreateFieldPNG(i);
	}
}

// I know that the play art field is 500x500, although I can do the math.
void CreateFieldPNG(int ind)
{
	ScaleApplyImage(&FINAL_PLAY, &gField.img, 0, 0, FINAL_PLAY.wd);

	// let's see if we can place all the players in the right spot.
	for (int i = 0; i < pHolder.aPlays[ind].numPlayers; i++)
	{
		// Start is relative to snap, which is always 25 yards in, and 40 yards deep.
		Vec2 adjPos = pHolder.aPlays[ind].pRoles[i].mStart;
		adjPos.x = (25 + adjPos.x) * 2;
		adjPos.y = (40 - adjPos.y) * 2;			// y goes top to bottom

		CenteredScaleApplyImage(&FINAL_PLAY, &gPlayer.img, adjPos.x, adjPos.y, 5);

		// Now we also have to render their assignments.
		if (pHolder.aPlays[ind].pRoles[i].mRole == "Route") {
			RenderRoute(pHolder.aPlays[ind].pRoles[i].mDetail, adjPos);
		}
		else if (pHolder.aPlays[ind].pRoles[i].mRole == "Pass Block") {
			CenteredScaleApplyImage(&FINAL_PLAY, &gBlock.img, adjPos.x, adjPos.y + 2, 2);
		}
	}
	
	//ScaleApplyImage(&FINAL_PLAY, &gPlayer.img, 50, 50, 10);
	std::string sName = "PlayArt/" + pHolder.aPlays[ind].mName + ".png";
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

bool LoadPlays(PlayHolder* pPlayHolder, std::string filePath)
{
	std::ifstream file("plays.txt");
	std::string str;
	std::getline(file, str);
	int numPlays = std::stoi(str);
	pPlayHolder->aPlays = new DATA_Off_Play[numPlays];
	pPlayHolder->numPlays = numPlays;
	for (int i = 0; i < numPlays; i++)
	{
		std::getline(file, str);
		pPlayHolder->aPlays[i].mName = str;

		// now here's where we have to format this stuff.
		std::getline(file, str);
		int numJobs = std::stoi(str);
		pPlayHolder->aPlays[i].numPlayers = numJobs;
		pPlayHolder->aPlays[i].pRoles = new DATA_PlayRole[numJobs];

		for (int j = 0; j < numJobs; j++)
		{
			std::getline(file, str);
			pPlayHolder->aPlays[i].pRoles[j].mTag = str;
			std::getline(file, str);
			pPlayHolder->aPlays[i].pRoles[j].mRole = str;
			std::getline(file, str);
			pPlayHolder->aPlays[i].pRoles[j].mDetail = str;
			std::getline(file, str);
			pPlayHolder->aPlays[i].pRoles[j].mStart = GetVecFromString(str);
		}
	}

	return false;
}

