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

void CollideThrowable(Object& fixed, Object& dynamic, int dir) {
	if (fixed.is_thrown) return;
	
	if (dynamic.is_thrown) {
		Enemy* e = dynamic_cast<Enemy*>(&fixed);
		
		if (e) e->TakeHit(dynamic.weight, dir);
		
		GetWorld().RemoveObject(&dynamic);
	}
}


Game::Game() {
	Sizeable().MaximizeBox().MinimizeBox();
	
	worldctrls.SetCount(8);
	worldctrls[0].name = "Castle";
	worldctrls[1].name = "Forest";
	worldctrls[2].name = "Jungle";
	worldctrls[3].name = "Industrial";
	worldctrls[4].name = "Casino";
	worldctrls[5].name = "War";
	worldctrls[6].name = "Toys";
	worldctrls[7].name = "Virtual";
	
	mapctrls.SetCount(8);
	
	PostCallback(THISBACK(Refresh0));
}

void Game::LoadMap(String path) {
	if (FileExists(path)) {
		map.Set(LoadFile(path));
		mode = MODE_MAP;
		InitMap();
	}
}

void Game::LoadMap(int world, int map) {
	String file = AppendFileName(GetDataFile("maps"), "w" + IntStr(world+1) + "_" + IntStr(map+1) + ".map");
	LOG(file);
	LoadMap(file);
}

void Game::InitMap() {
	World& world = GetWorld();
	
	world.Clear();
	
	world.SetCollideFunction(COLLIDE_GROUND, CollideGround);
	world.SetCollideFunction(COLLIDE_WATER, CollideWater);
	world.SetCollideFunction(COLLIDE_BORDER, CollideBorder);
	world.SetCollideFunction(COLLIDE_RUNNINGWATER, CollideRunningWater);
	world.SetCollideFunction(COLLIDE_THROWABLE, CollideThrowable);
	
	for(int i = 0; i < map.objects.GetCount(); i++) {
		Object& o = map.objects[i];
		if (o.name == "waterdrop") {
			WaterSpawn& ws = waterspawns.Add();
			ws.Object::operator=(o);
			world.AddObject(ws);
		}
		else if (o.name == "enemy1") {
			Enemy& e = enemies.Add();
			e.Object::operator=(o);
			e.Init();
			world.AddObject(e);
		}
		else
			world.AddObject(o);
	}
	
	
	pl.tile_id = GetTileSet().tiles.Find("player_idle");
	pl.pos.left = 15 * map.blocksize;
	pl.pos.top = 15 * map.blocksize;
	pl.pos.right = pl.pos.left + 32.0;
	pl.pos.bottom = pl.pos.top - 52.0;
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
	Size sz(GetSize());
	
	
	double pad = sz.cx * 0.05;
	double width = (sz.cx - 2 * pad - 3 * pad) / 4.0;
	double height = (sz.cy - 2 * pad - 1 * pad) / 2.0;
	for(int i = 0; i < 4; i++) {
		WorldCtrl& w0 = worldctrls[0 + i];
		WorldCtrl& w1 = worldctrls[4 + i];
		w0.r.left = pad + i * (width + pad),
		w0.r.right = w0.r.left + width;
		w0.r.top = pad;
		w0.r.bottom = w0.r.top + height;
		w1.r.left = pad + i * (width + pad),
		w1.r.right = w1.r.left + width;
		w1.r.top = pad * 2 + height;
		w1.r.bottom = w1.r.top + height;
	}
	
	for(int i = 0; i < worldctrls.GetCount(); i++) {
		mapctrls[i].r = worldctrls[i].r;
	}
}

void Game::LeftDown(Point p, dword keyflags) {
	if (mode == MODE_WORLDSELECT) {
		for(int i = 0; i < worldctrls.GetCount(); i++) {
			if (worldctrls[i].r.Contains(p)) {
				world_id = i;
				mode = MODE_MAPSELECT;
				Refresh();
			}
		}
	}
	else if (mode == MODE_MAPSELECT) {
		for(int i = 0; i < mapctrls.GetCount(); i++) {
			if (mapctrls[i].r.Contains(p)) {
				map_id = i;
				mode = MODE_MAP;
				LoadMap(world_id, map_id);
				Refresh();
			}
		}
	}
}

void Game::Paint(Draw& d) {
	if (mode == MODE_WORLDSELECT) {
		PaintWorlds(d);
	}
	else if (mode == MODE_MAPSELECT) {
		PaintMaps(d);
	}
	else if (mode == MODE_MAP) {
		PaintGame(d);
	}
}

void Game::PaintWorlds(Draw& d) {
	Size sz(GetSize());
	
	d.DrawRect(sz, White());
	
	
	for(int i = 0; i < worldctrls.GetCount(); i++) {
		WorldCtrl& w = worldctrls[i];
		
		d.DrawRect(w.r, GrayColor(128+64));
		
		Point bct = w.r.BottomCenter();
		
		Font fnt = Arial(15);
		Size txtsize = GetTextSize(w.name, fnt);
		
		Point pt = bct;
		pt.x -= txtsize.cx / 2;
		pt.y -= txtsize.cy + 5;
		
		d.DrawText(pt.x, pt.y, w.name, fnt, Black());
	}
}

void Game::PaintMaps(Draw& d) {
	Size sz(GetSize());
	
	d.DrawRect(sz, White());
	
	for(int i = 0; i < mapctrls.GetCount(); i++) {
		MapCtrl& w = mapctrls[i];
		
		d.DrawRect(w.r, GrayColor(128+64));
		
		Point bct = w.r.BottomCenter();
		
		String text = "Map #" + IntStr(i + 1);
		Font fnt = Arial(15);
		Size txtsize = GetTextSize(text, fnt);
		
		Point pt = bct;
		pt.x -= txtsize.cx / 2;
		pt.y -= txtsize.cy + 5;
		
		d.DrawText(pt.x, pt.y, text, fnt, Black());
	}
}

void Game::PaintGame(Draw& d) {
	World& world = GetWorld();
	
	Tile& bg = GetTileSet().tiles.Get("w1_bg");
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
			int img_x = (x0 + x1 - img.GetWidth()) / 2;
			if (o.rotate == 0)
				d.DrawImage(img_x, img_y, img);
			else
				d.DrawImage(img_x, img_y, Rotate(img, o.rotate * 10));
			
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
