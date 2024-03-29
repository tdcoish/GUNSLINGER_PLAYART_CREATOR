
#include <iostream>
#include "Blit.h"

// Gotta make sure this is in a .cpp file.

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master\stb_image_write.h"

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


