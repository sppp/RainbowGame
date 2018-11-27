#include "RainGame.h"

void CollideGround(Object& fixed, Object& dynamic, int dir) {
	
	if (dir == C_TOP) {
		dynamic.collide_ground = true;
	}
	
}

void CollideWater(Object& dynamic0, Object& dynamic1, int dir) {
	Player* pl = dynamic_cast<Player*>(&dynamic0);
	WaterDrop* wd;
	if (!pl) {
		return;
	}
	else {
		wd = dynamic_cast<WaterDrop*>(&dynamic1);
	}
	if (!pl || !wd)
		return;
	
	if (dir == C_LEFT && pl->is_umbrella && pl->is_tile_mirror && pl->have_water == 0) {
		if (pl->AddWaterDrop(0))
			GetWorld().RemoveObject(wd);
	}
	if (dir == C_RIGHT && pl->is_umbrella && !pl->is_tile_mirror && pl->have_water == 0) {
		if (pl->AddWaterDrop(0))
			GetWorld().RemoveObject(wd);
	}
	if (dir == C_TOP && pl->is_umbrella && pl->have_water > 0) {
		if (pl->AddWaterDrop(0))
			GetWorld().RemoveObject(wd);
	}
}

void CollideBorder(Object& fixed, Object& dynamic, int dir) {
	if (fixed.name == "bottom") {
		WaterDrop* wd = dynamic_cast<WaterDrop*>(&dynamic);
		Player* pl = dynamic_cast<Player*>(&dynamic);
		WaterRunning* wr = dynamic_cast<WaterRunning*>(&dynamic);
		
		if (wd) {
			GetWorld().RemoveObject(wd);
		}
		else if (pl) {
			Object& top = *GetWorld().FindObject("top");
			int height = top.pos.top - fixed.pos.top;
			pl->pos.top += height;
			pl->pos.bottom += height;
		}
		else if (wr) {
			GetWorld().RemoveObject(wr);
			wr->ReleaseObjs();
		}
	}
}

void CollideRunningWater(Object& fixed, Object& dynamic, int dir) {
	Player* pl = dynamic_cast<Player*>(&dynamic);
	WaterRunning* wr = dynamic_cast<WaterRunning*>(&fixed);
	if (pl && wr) {
		if (!pl->is_frozen) {
			wr->moved_objects.Add(pl);
			pl->is_frozen = true;
		}
	}
}



Game::Game() {
	Sizeable().MaximizeBox().MinimizeBox();
	
	
	PostCallback(THISBACK(Refresh0));
}

void Game::LoadMap(String path) {
	if (FileExists(path)) {
		FileIn fin(path);
		fin % map;
	}
}

void Game::InitMap() {
	World& world = GetWorld();
	
	world.SetCollideFunction(COLLIDE_GROUND, CollideGround);
	world.SetCollideFunction(COLLIDE_WATER, CollideWater);
	world.SetCollideFunction(COLLIDE_BORDER, CollideBorder);
	world.SetCollideFunction(COLLIDE_RUNNINGWATER, CollideRunningWater);
	
	for(int i = 0; i < map.maptiles.GetCount(); i++)
		world.AddObject(map.maptiles[i]);
	
	for(int i = 0; i < map.waterspawns.GetCount(); i++)
		world.AddObject(map.waterspawns[i]);
	
	
	pl.tile_id = GetTileSet().tiles.Find("player_idle");
	pl.pos.left = 15 * map.blocksize;
	pl.pos.top = 15 * map.blocksize;
	pl.pos.right = pl.pos.left + 52.0;
	pl.pos.bottom = pl.pos.top - 52.0;
	pl.collide_mask |= 1 << COLLIDE_GROUND;
	pl.collide_mask |= 1 << COLLIDE_WATER;
	pl.collide_mask |= 1 << COLLIDE_BORDER;
	pl.collide_mask |= 1 << COLLIDE_RUNNINGWATER;
	pl.acceleration.y = GRAVITY;
	world.AddObject(pl);
	
	
	bottom.name = "bottom";
	bottom.pos.top = -64;
	bottom.pos.left = -map.width;
	bottom.pos.right = map.width * 2;
	bottom.pos.bottom = bottom.pos.top - Map::blocksize;
	bottom.collide_mask |= 1 << COLLIDE_BORDER;
	//bottom.fixed |= 1 << FIXED_TOP;
	world.AddObject(bottom);
	
	top.name = "top";
	top.pos.top = map.height+64;
	top.pos.left = -map.width;
	top.pos.right = map.width * 2;
	top.pos.bottom = top.pos.top - Map::blocksize;
	top.collide_mask |= 1 << COLLIDE_BORDER;
	//top.fixed |= 1 << FIXED_BOTTOM;
	world.AddObject(top);
}

void Game::Layout() {
	
}

void Game::Paint(Draw& d) {
	World& world = GetWorld();
	
	Tile& bg = GetTileSet().tiles.Get("bg");
	d.DrawImage(0, 0, Rescale(bg.img, bg.img.GetSize() * multiplier));
	
	
	
	for(int i = 0; i < world.GetObjectCount(); i++) {
		Object& o = world.GetObject(i);
		
		if (o.tile_id >= 0) {
			int x0 = o.pos.left * multiplier;
			int y0 = (map.height - o.pos.top) * multiplier;
			int x1 = o.pos.right * multiplier;
			int y1 = (map.height - o.pos.bottom) * multiplier;
			Tile& tile = GetTileSet().tiles[o.tile_id];
			
			Image i = tile.img;
			if (o.IsTileMirror())
				i = MirrorHorz(i);
			
			Image img = Rescale(i, i.GetSize() * multiplier);
			int img_y = y0 - img.GetHeight() - (y0 - y1);
			d.DrawImage(x0, img_y, img);
			
			d.DrawLine(x0, y0, x1, y0, 1, Color(255,0,0));
			d.DrawLine(x0, y1, x1, y1, 1, Color(255,0,0));
			d.DrawLine(x0, y0, x0, y1, 1, Color(255,0,0));
			d.DrawLine(x1, y0, x1, y1, 1, Color(255,0,0));
		}
	}
}

bool Game::Key(dword key, int count) {
	if (pl.Key(key))
		return true;
	
	return false;
}
