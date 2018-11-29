#include "Engine.h"

TileSet::TileSet() {
	Load();
}

void TileSet::Load() {
	tiles.Clear();
	
	
	Load("player_falling");
	Load("player_fallingumbrella");
	Load("player_idle");
	Load("player_umbrella");
	Load("player_umbrellaup");
	Load("player_running");
	Load("player_runningumbrella");
	Load("player_runningumbrellaup");
	
	Load("w1_block1");
	Load("w1_block1_shadow");
	Load("w1_curtain1");
	Load("w1_curtain2");
	Load("w1_curtain3");
	Load("w1_bg");
	
	Load("water");
	Load("bigwater");
	Load("runningwater");
	Load("waterdrop");
	
	Load("w1_enemy1");
	LoadGreen("w1_enemy1");
}

void TileSet::Load(String key) {
	String path = GetDataFile("assets" DIR_SEPS + key + ".png");
	if (!FileExists(path))
		return;
	
	Tile& tile = tiles.Add(key);
	tile.id = tiles.GetCount() - 1;
	tile.img = StreamRaster::LoadFileAny(path);
}

void TileSet::LoadGreen(String from_key) {
	String to_key = from_key + "_green";
	Tile& from = tiles.Get(from_key);
	Tile& to = tiles.Add(to_key);
	to.id = tiles.GetCount() - 1;
	
	Size sz(from.img.GetSize());
	ImageBuffer ib(sz);
	RGBA* to_it = ib.Begin();
	const RGBA* from_it = from.img.Begin();
	for(int y = 0; y < sz.cy; y++) {
		for (int x = 0; x < sz.cx; x++) {
			to_it->a = from_it->a;
			int gray = (from_it->r + from_it->g + from_it->b) / 3;
			to_it->r = 0;
			to_it->g = gray;
			to_it->b = 0;
			to_it++;
			from_it++;
		}
	}
	to.img = ib;
}
