#pragma once

#include "Structs.h"
#include "Blit.h"

extern ZoneHolder ZoneList;
extern DefPlayHolder DefPlayList;

extern Graphic gDefender;
extern Graphic gZone;

bool LoadZones(std::string filePath);
bool LoadDefensivePlays(std::string filePath);

void PrintAllZones();
void PrintAllDefensivePlays();

void LoadDefGraphics(std::string filePath);

void CreateAllDefensivePlayArt();
void CreateDefensiveFieldPNG(int ind);



