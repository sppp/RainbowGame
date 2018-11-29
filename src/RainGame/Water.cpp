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
		o->collide_mask |= 1 << COLLIDE_WATER;
		o->collide_mask |= 1 << COLLIDE_BORDER;
		o->acceleration.y = GRAVITY;
	
		GetWorld().AddOwnedObject(o);
		
		ts.Reset();
	}
	
}

void WaterRunning::Tick() {
	World& world = GetWorld();
	
	int steps = ts.Elapsed() / step - wait;
	if (steps < 0) steps = 0;
	int steps_now = steps - prev_steps;
	prev_steps = steps;
	
	for(int i = 0; i < steps_now; i++) {
		Rectf bottom = pos;
		bottom.top -= Map::blocksize;
		bottom.bottom -= Map::blocksize;
		Rectf left = pos;
		left.left -= Map::blocksize;
		left.right -= Map::blocksize;
		Rectf right = pos;
		right.left += Map::blocksize;
		right.right += Map::blocksize;
		
		bool go_bottom = true;
		bool go_left = true;
		bool go_right = true;
		for(int j = 0; j < world.GetObjectCount(); j++) {
			const Object& o = world.GetObject(j);
			if (!o.fixed || !(o.collide_mask & (1 << COLLIDE_GROUND)))
				continue;
			
			if (OverlapFactor(o.pos, bottom) > 0.33)
				go_bottom = false;
			if (OverlapFactor(o.pos, left) > 0.33)
				go_left = false;
			if (OverlapFactor(o.pos, right) > 0.33)
				go_right = false;
		}
		
		if (go_bottom)
			pos = bottom;
		else {
			if (dir) {
				if (go_left)
					pos = left;
				else {
					pos = right;
					dir = false;
				}
			} else {
				if (go_right)
					pos = right;
				else {
					pos = left;
					dir = true;
				}
			}
		}
	}
	
	Point ct = pos.CenterPoint();
	for(int i = 0; i < moved_objects.GetCount(); i++) {
		Object& o = *moved_objects[i];
		double w = o.pos.GetWidth();
		double h = -o.pos.GetHeight();
		o.pos.left = ct.x - w/2;
		o.pos.right = o.pos.left + w;
		o.pos.top = ct.y + h;
		o.pos.bottom = o.pos.top - h;
		ASSERT(o.pos.top >= o.pos.bottom);
	}
	
	Object& bottom = *GetWorld().FindObject("bottom");
	if (pos.top < bottom.pos.top) {
		GetWorld().RemoveObject(this);
		ReleaseObjs();
	}
}

void WaterRunning::ReleaseObjs() {
	Object& bottom = *GetWorld().FindObject("bottom");
	Object& top = *GetWorld().FindObject("top");
	int height = top.pos.top - bottom.pos.top;
	
	for(int i = 0; i < moved_objects.GetCount(); i++) {
		Object& o = *moved_objects[i];
		o.is_frozen = false;
		o.pos.top += height;
		o.pos.bottom += height;
		ASSERT(o.pos.top >= o.pos.bottom);
	}
	moved_objects.Clear();
}
