#pragma once

#include <string>
#include <list>

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

struct DATA_Zone {
	std::string mName;
	Vec2 mSpot;
};

struct ZoneHolder{
	int numZones;
	DATA_Zone* aZones;
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

struct DATA_Off_Route {
	std::string				mOwner;
	int						mNumSpots;
	Vec2*					mSpots;
};

struct DATA_Off_Play {
	std::string				mName;
	std::string				mFormation;
	int						mNumReceivers;
	int						mNumPlayers;
	std::string*			aTags;
	std::string*			aRoles;
	DATA_Off_Route*			aRoutes;
};

struct DATA_Off_Formation {
	int						mNumPlayers;
	std::string				mName;
	std::string*			aTags;
	Vec2*					aSpots;
};

struct DATA_Def_Play {
	std::string				mName;
	int						numPlayers;
	DATA_PlayRole*			pRoles;
};

struct OffPlayHolder {
	int						mNumPlays;
	DATA_Off_Play*			aPlays;
};

struct OffFormationHolder {
	int						numFormations;
	DATA_Off_Formation*		aFormations;
};

struct DefPlayHolder {
	int						numPlays;
	DATA_Def_Play*			aPlays;
};