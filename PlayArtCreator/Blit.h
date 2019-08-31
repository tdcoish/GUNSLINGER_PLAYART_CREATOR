#pragma once

#include <string>

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