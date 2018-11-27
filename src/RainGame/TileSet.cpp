#include "RainGame.h"

TileSet::TileSet() {
	Load(1);
}

void TileSet::Load(int world) {
	tiles.Clear();
	
	
	Load("player_falling", "player_falling.png", false);
	Load("player_fallingumbrella", "player_fallingumbrella.png", false);
	Load("player_idle", "player_idle.png", false);
	Load("player_umbrella", "player_umbrella.png", false);
	Load("player_umbrellaup", "player_umbrellaup.png", false);
	Load("player_running", "player_running.png", false);
	Load("player_runningumbrella", "player_runningumbrella.png", false);
	Load("player_runningumbrellaup", "player_runningumbrellaup.png", false);
	
	Load("block1", "w" + IntStr(world) + "_block1.png", true);
	Load("block1_shadow", "w" + IntStr(world) + "_block1_shadow.png", true);
	Load("curtain1", "w" + IntStr(world) + "_curtain1.png", false);
	Load("curtain2", "w" + IntStr(world) + "_curtain2.png", false);
	Load("curtain3", "w" + IntStr(world) + "_curtain3.png", false);
	Load("bg", "w" + IntStr(world) + "_bg.png", false);
	
	Load("water", "water.png", false);
	Load("bigwater", "bigwater.png", false);
	Load("runningwater", "runningwater.png", false);
	
}

void TileSet::Load(String key, String file, bool is_solid) {
	String path = GetDataFile("assets" DIR_SEPS + file);
	if (!FileExists(path))
		return;
	
	Tile& tile = tiles.Add(key);
	tile.id = tiles.GetCount() - 1;
	tile.img = StreamRaster::LoadFileAny(path);
}
