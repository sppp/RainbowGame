#ifndef _RainGame_RainGame_h
#define _RainGame_RainGame_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define IMAGEFILE <RainGame/RainGame.iml>
#include <Draw/iml_header.h>

inline bool Intersects(const Rectf& r0, const Rectf& r1) {
	bool up_col = r1.top > r0.top && r1.bottom < r0.top;
	bool bot_col = r1.bottom < r0.bottom && r1.top > r0.bottom;
	bool vinside = r1.top <= r0.top && r1.bottom >= r0.bottom;
	bool voutside = r1.top >= r0.top && r1.bottom <= r0.bottom;
	bool left_col = r1.left < r0.left && r1.right > r0.left;
	bool right_col = r1.right > r0.right && r1.left < r0.right;
	bool hinside = r1.left >= r0.left && r1.right <= r0.right;
	bool houtside = r1.left <= r0.left && r1.right >= r0.right;
	
	return (up_col || bot_col || vinside || voutside) && (left_col || right_col || hinside || houtside);
}

inline double OverlapFactor(Rectf r0, Rectf r1) {
	Swap(r0.top, r0.bottom);
	Swap(r1.top, r1.bottom);
	Rectf r2 = r0;
	r2.Intersect(r1);
	double r0_area = r0.GetHeight() * r0.GetWidth();
	double r2_area = r2.GetHeight() * r2.GetWidth();
	ASSERT(r0_area >= 0.0);
	double f = r2_area / r0_area;
	return f;
}

#include "TileSet.h"
#include "World.h"
#include "Water.h"
#include "Entity.h"
#include "MapEditor.h"
#include "Game.h"

#define GRAVITY -200


#endif
