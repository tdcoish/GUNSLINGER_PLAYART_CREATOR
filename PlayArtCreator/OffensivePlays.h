#pragma once

#include "Structs.h"
#include "Blit.h"

//extern OffPlayHolder OffensivePlays;
extern RouteHolder rHolder;

extern Graphic gPlayer;
extern Graphic gRouteBase;
extern Graphic gRouteArrow;
extern Graphic gBlock;

OffFormationHolder LoadOffensiveFormations(std::string directoryPath);
DATA_Off_Formation LoadFormationFromFile(std::string filePath);

Vec2 GetStartingPositionFromFormation(std::string tag, std::string formation, OffFormationHolder formHolder);

OffPlayHolder LoadOffensivePlays(std::string directoryPath);
DATA_Off_Play LoadOffensivePlayFromFile(std::string filePath);
DATA_Off_Route GetRouteFromPlayTag(std::string owner, std::string playName, OffPlayHolder plays);

void LoadImages(std::string path);

void CreateAllOffPlayArt(OffPlayHolder plays, OffFormationHolder formations, std::string dirPath);

void PrintAllRoutes();

void PrintAllFormations(OffFormationHolder holder);
void PrintAllOffensivePlays(OffPlayHolder holder);

void RenderRoute(Vec2 vStart, DATA_Off_Route route);

//bool LoadOffensivePlays(std::string filePath);