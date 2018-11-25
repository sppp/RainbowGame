#include "RainGame.h"

void CollideGround(Object& fixed, Object& dynamic, int dir) {
	
	if (dir == C_TOP) {
		dynamic.collide_ground = true;
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
	pl.acceleration.y = GRAVITY;
	world.AddObject(pl);
	
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
			int y0 = (map.height * map.blocksize - o.pos.top) * multiplier;
			int x1 = o.pos.right * multiplier;
			int y1 = (map.height * map.blocksize - o.pos.bottom) * multiplier;
			Tile& tile = GetTileSet().tiles[o.tile_id];
			
			Image i = tile.img;
			if (o.IsTileMirror())
				i = MirrorHorz(i);
			
			d.DrawImage(x0, y0, Rescale(i, i.GetSize() * multiplier));
			
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
