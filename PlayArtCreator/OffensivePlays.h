#pragma once

#include "Structs.h"
#include "Blit.h"

extern OffPlayHolder OffensivePlays;
extern RouteHolder rHolder;

extern Graphic gPlayer;
extern Graphic gRouteBase;
extern Graphic gRouteArrow;
extern Graphic gBlock;

bool LoadRoutes(RouteHolder* pRouteHolder, std::string filePath);
OffFormationHolder LoadOffensiveFormations(std::string filePath);
DATA_Off_Formation LoadFormationFromFile(std::string filePath);

bool LoadOffensivePlays(std::string filePath);

void LoadImages(std::string path);

void CreateAllOffPlayArt();
void CreateOffensiveFieldPNG(int ind);

void PrintAllRoutes();
void PrintAllOffensivePlays();

void RenderRoute(std::string name, Vec2 vStart);

//bool LoadOffensivePlays(std::string filePath);