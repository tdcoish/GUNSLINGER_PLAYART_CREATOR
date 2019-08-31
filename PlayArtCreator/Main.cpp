/***********************************************************************************
Wow, fuck me. Can't read in strings into c++ without first saving the length of the string.

So fucking irritating.

Maybe I'll make a .Net implementation.

Maybe not. The real problem is loading in the data. Maybe I'll... 
Nope, c# strings are collections of System.Char objects. So you can't possibly know the actual
data size of the stored string. Fuck me.

Stop whining like a little bitch. Convert the file to a text file and then load it in you
moron.
***********************************************************************************/


#include <iostream>
#include <fstream>
#include <cstdlib>
#include "time.h"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master\stb_image_write.h"

struct Vec2 {
	int x, y;
	Vec2 operator+(const Vec2& v2) {
		Vec2 v;
		v.x = x + v2.x;
		v.y = y + v2.y;
		return v;
	}
	Vec2 operator-(const Vec2& v2) {
		Vec2 v;
		v.x = x - v2.x;
		v.y = y - v2.y;
		return v;
	}
	Vec2 operator*(const int val) {
		Vec2 v;
		v.x = x * val;
		v.y = y * val;
		return v;
	}
	Vec2 operator/(const int val) {
		Vec2 v;
		v.x = x / val;
		v.y = y / val;
		return v;
	}
};

struct DATA_Route {
	std::string mName;
	int numSpots;
	Vec2* aSpots;
};

struct RouteHolder {
	int numRoutes;
	DATA_Route* aRoutes;
};

struct DATA_PlayRole {
	std::string				mTag;
	std::string				mRole;
	std::string				mDetail;
	Vec2					mStart;
};

struct DATA_Off_Play {
	std::string				mName;
	int						numPlayers;
	DATA_PlayRole*			pRoles;
};

struct PlayHolder {
	int						numPlays;
	DATA_Off_Play*			aPlays;
};

struct Pixel {
	unsigned char r, g, b, a;
};

// Pixel* caused me lots of problems.
struct Image {
	int wd, ht, bpp;
	Pixel* pImg;
};

struct Graphic {
	Image img;
	std::string type;
};

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

///Load in a PNG to our Image struct
bool LoadImage(Image* pImg, std::string filePath);

void CenteredScaleApplyImage(Image* pI1, Image* pI2, int startX, int startY, float size);

//blit image 2 onto a percent of image 1.
void ScaleApplyImage(Image* pI1, Image* pI2, int startX, int startY, float size);

//blit one image onto another.
void ApplyImage(Image* pI1, Image* pI2, int startX, int startY, float size);

//Wrap around stbi_write_png to write our png again.
void WriteImage(Image img, std::string name);

//If we want to manually create the raw pixel data. Must be square
void CreateImage(Image* pImg, int size);

//create a new size using nearest neighbour.
Image ResizeImage(Image old, float size);

//delete allocated pixels inside Image struct.
void DeleteImage(Image img);

void LoadImages(std::string path);

//apparently fMod gives me this functionatlity.
int RoundFloatUpOrDown(float num) {
	int temp = (int)num;
	float temp2 = (float)temp;
	if (num - temp2>0.5) {
		return (int)(num + 1);
	}
	else {
		return (int)num;
	}
}

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

	//ScaleApplyImage(&FINAL_PLAY, &gBlock.img, 0, 0, 20);
	//ScaleApplyImage(&FINAL_PLAY, &gField.img, 0, 0, FINAL_PLAY.wd);
	//ScaleApplyImage(&FINAL_PLAY, &gBlock.img, 0, 0, 10);
	//WriteImage(FINAL_PLAY, "test.png");

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

bool LoadImage(Image* pImg, std::string filePath) {
	unsigned char* pRaw = NULL;
	pRaw = stbi_load(filePath.c_str(), &pImg->wd, &pImg->ht, &pImg->bpp, 0);
	if (pRaw == NULL) {
		std::cout << "Error loading PNG\n";
		std::cout << filePath << std::endl;
		return false;
	}

	//allocate memory for pixels
	int size = pImg->wd*pImg->ht;
	pImg->pImg = new Pixel[size]();
	//if image has no alpha channel, give it full opaqueness which == 255.
	for (int i = 0; i<size*pImg->bpp; i += pImg->bpp) {
		pImg->pImg[i / pImg->bpp].r = pRaw[i];
		pImg->pImg[i / pImg->bpp].g = pRaw[i + 1];
		pImg->pImg[i / pImg->bpp].b = pRaw[i + 2];
		if (pImg->bpp == 4) {
			pImg->pImg[i / pImg->bpp].a = pRaw[i + 3];

			//an ugly hack because paint turned all my alpha into white. This will ruin white images.
			if (pImg->pImg[i / pImg->bpp].r == 255 && pImg->pImg[i / pImg->bpp].g == 255 && pImg->pImg[i / pImg->bpp].r == 255) {
				pImg->pImg[i / pImg->bpp].a = 0;		//make all white pixels transparent.
			}
		}
		else {
			pImg->pImg[i / pImg->bpp].a = 255;		//fully opaque.
		}
	}
	pImg->bpp = 4;		//since we've added the alpha channel.

	delete pRaw;
	return true;
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

//using simple nearest neighbour.
//assumes square images.
Image ResizeImage(Image old, float size)
{
	//if the new size is the same as the old size.
	if (old.wd == size)
		return old;

	int count = 0;
	Image temp;
	CreateImage(&temp, (int)size);

	//can't go past with the X value.

	//nearest neighbour algorithm.
	float increase = (float)old.wd / size;
	float oldIndice;
	int spotX, spotY;
	float oldSpotX, oldSpotY;
	int testDif = 0;
	int numDif = 0;
	for (int i = 0; i<size*size; i++) {
		spotX = i % (int)size;		//so if picture == 20x20 and i=42. x=2.
		spotY = i / size;			//so if picture==20x20 and i=42. y=2
		oldSpotX = (float)spotX*increase;
		//can't go past the edge.
		oldSpotX = (int)oldSpotX;
		oldSpotY = (float)spotY*increase;
		//always start at the beginning of the line
		oldSpotY = (int)oldSpotY;
		oldIndice = RoundFloatUpOrDown(oldSpotY*(float)old.wd + (oldSpotX));


		temp.pImg[i] = old.pImg[(int)oldIndice];

	}
	return temp;
}

// Maybe not quite yet.
void StretchScaleApplyImage(Image* pI1, Image* pI2, int startX, int startY, int finishX, int finishY)
{

}

void CenteredScaleApplyImage(Image* pI1, Image* pI2, int startX, int startY, float size)
{
	ScaleApplyImage(pI1, pI2, startX - size / 2, startY - size / 2, size);
}

void ScaleApplyImage(Image* pI1, Image* pI2, int startX, int startY, float size) {
	float percent = (float)pI1->wd / 100;
	startX *= percent;
	startY *= percent;
	size *= percent;
	ApplyImage(pI1, pI2, startX, startY, size);
}

//software blit one image onto another. Second image is being blitted into the first
void ApplyImage(Image* pI1, Image* pI2, int startX, int startY, float size) {
	size = (int)size;
	Image temp;
	//resize the image here.
	temp = ResizeImage(*pI2, size);

	int startLoc = pI1->wd*startY + startX;


	//keep blitting in bounds
	int blitStartY = 0;
	int blitStartX = 0;
	if (startX<0)
		blitStartX -= startX;
	if (startY<0)
		blitStartY -= startY;

	int blitLengthX = size;
	if ((startX + size)>pI1->wd) {
		blitLengthX -= (startX + size) - pI1->wd;	//don't go past right edge
		blitLengthX -= blitStartX;		//don't go past left edge
	}
	int blitLengthY = size;
	if (startY + size>pI1->ht) {
		blitLengthY -= (startY + size) - pI1->ht;
		blitLengthY -= blitStartY;
	}

	//blit all valid pixels.
	int im1pxl, im2pxl;
	for (int line = blitStartY; line<blitLengthY; line++) {
		for (int i = blitStartX; i<blitLengthX; i++) {
			im1pxl = pI1->wd*line + i + startLoc;
			im2pxl = temp.wd*line + i;

			//if fully opaque, just write over.
			int alphaVal = (int)temp.pImg[im2pxl].a;
			if (alphaVal == 255) {
				pI1->pImg[im1pxl] = temp.pImg[im2pxl];
			}
			//if partially opaque, blend appropriately
			else if (alphaVal != 0) {
				float newPercent = temp.pImg[im2pxl].a / 255.0;
				float oldPercent = 1.0 - newPercent;
				Pixel newPixel;
				//newPixel.a=pI1->pImg[im1px].
				newPixel.r = pI1->pImg[im1pxl].r*oldPercent + temp.pImg[im2pxl].r*newPercent;
				newPixel.g = pI1->pImg[im1pxl].g*oldPercent + temp.pImg[im2pxl].g*newPercent;
				newPixel.b = pI1->pImg[im1pxl].b*oldPercent + temp.pImg[im2pxl].b*newPercent;
				//what should the alpha be?????? 
				newPixel.a = 255;		//end result should always be opaque, since I assume the background is opaque anyway.

				pI1->pImg[im1pxl] = newPixel;
			}
			//pixel is translucent
			else {
				//just do nothing, since we don't blend.
			}
		}
	}
}

//Wrap around stbi_write_png to write our png again.
void WriteImage(Image img, std::string name) {
	int size = img.ht*img.wd*img.bpp;
	unsigned char* pRaw = new unsigned char[size]();
	for (int i = 0; i<size; i += img.bpp) {
		pRaw[i] = img.pImg[i / img.bpp].r;
		pRaw[i + 1] = img.pImg[i / img.bpp].g;
		pRaw[i + 2] = img.pImg[i / img.bpp].b;
		pRaw[i + 3] = img.pImg[i / img.bpp].a;
	}
	stbi_write_png(name.c_str(), img.wd, img.ht, img.bpp, pRaw, 0);
	delete pRaw;		//for some reason this is giving me an error.
}

//If we want to manually create the raw pixel data.
void CreateImage(Image* pImg, int size) {
	pImg->ht = size;
	pImg->wd = size;
	pImg->bpp = 4;			//all images have alpha channel.
	pImg->pImg = NULL;
	int memSize = size * size;
	pImg->pImg = new Pixel[memSize]();
}

//delete allocated pixels inside Image struct.
void DeleteImage(Image img) {
	delete img.pImg;
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
	std::string sName = pHolder.aPlays[ind].mName + ".png";
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

