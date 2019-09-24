#pragma once

#include "Structs.h"
#include "Blit.h"

//extern OffPlayHolder OffensivePlays;
extern RouteHolder rHolder;

extern Graphic gPlayer;
extern Graphic gRouteBase;
extern Graphic gRouteArrow;
extern Graphic gBlock;

bool LoadRoutes(RouteHolder* pRouteHolder, std::string filePath);
OffFormationHolder LoadOffensiveFormations(std::string directoryPath);
DATA_Off_Formation LoadFormationFromFile(std::string filePath);

Vec2 GetStartingPositionFromFormation(std::string tag, std::string formation, OffFormationHolder formHolder);

OffPlayHolder LoadOffensivePlays(std::string directoryPath);
DATA_Off_Play LoadOffensivePlayFromFile(std::string filePath);

void LoadImages(std::string path);

void CreateAllOffPlayArt(OffPlayHolder plays, OffFormationHolder formations);
void CreateOffensiveFieldPNG(int ind);

void PrintAllRoutes();

void PrintAllFormations(OffFormationHolder holder);
void PrintAllOffensivePlays(OffPlayHolder holder);

void RenderRoute(std::string name, Vec2 vStart);

//bool LoadOffensivePlays(std::string filePath);