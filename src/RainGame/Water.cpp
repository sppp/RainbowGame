#include "RainGame.h"


void WaterDrop::Tick() {
	
}

void WaterDrop::PostTick() {
	
	if (collide_ground) {
		velocity.x = dir ? -50 : +50;
		velocity.y = 0;
	}
}



void WaterSpawn::Tick() {
	
	if (ts.Elapsed() > 1*1000) {
		WaterDrop* o = new WaterDrop();
		o->pos = pos;
		o->tile_id = GetTileSet().tiles.Find("water");
		Tile& tile = GetTileSet().tiles[o->tile_id];
		o->pos.right = o->pos.left + tile.img.GetWidth();
		o->pos.bottom = o->pos.top - tile.img.GetHeight();
		o->collide_mask |= 1 << COLLIDE_GROUND;
		o->acceleration.y = GRAVITY;
	
		GetWorld().AddOwnedObject(o);
		
		ts.Reset();
	}
	
}
