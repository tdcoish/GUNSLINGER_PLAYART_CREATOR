#pragma once

#include "Structs.h"
#include "Blit.h"

Vec2 GetVecFromString(std::string sVec);
std::string ConvertVecToString(Vec2 v);

extern Image FINAL_PLAY;
const int PLAY_SIZE_PIXELS = 256;		//500x500 pixels.

extern Graphic gField;
