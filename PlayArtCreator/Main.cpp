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

#include "Globals.h"
#include "Blit.h"
#include "Structs.h"
#include "OffensivePlays.h"
#include "DefensivePlays.h"


std::string bsPth = "C:/Users/Timothy/Documents/Visual Studio 2017/Projects/PlayArtCreator/GUNSLINGER_PLAYART_CREATOR/PlayArtCreator";
std::string imgPath(bsPth + "/Graphics");
std::string formPth(bsPth + "/OffensivePlays");

Image FINAL_PLAY;
Graphic gField;


// I just had no idea where to shove this guy.
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

std::string ConvertVecToString(Vec2 v)
{
	std::string sVec = "(" + std::to_string(v.x) + "," + std::to_string(v.y) + ")";
	std::cout << sVec;
	return sVec;
}

int main(int args, char** argc)
{
	std::cout << "Base Path: " << bsPth << std::endl;

	CreateImage(&FINAL_PLAY, PLAY_SIZE_PIXELS);

	std::string sSavePath = bsPth + "New Plays/";
	std::cout << sSavePath;

	LoadImages(imgPath);
	LoadDefGraphics(imgPath);

	LoadOffensiveFormations(formPth);

	LoadRoutes(&rHolder, bsPth+"/routes.txt");
	LoadOffensivePlays(bsPth + "/plays.txt");

	LoadZones(bsPth + "/zones.txt");
	LoadDefensivePlays(bsPth + "/defPlays.txt");


	//PrintAllRoutes();
	//PrintAllPlays();
	//PrintAllZones();
	//PrintAllDefensivePlays();

	// Finally, the time has come to actually print out the play art.
	//CreateAllOffPlayArt();
	CreateAllDefensivePlayArt();


	std::getchar();

	return 0;
}



