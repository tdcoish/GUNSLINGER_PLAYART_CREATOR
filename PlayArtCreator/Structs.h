#pragma once

#include <string>

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