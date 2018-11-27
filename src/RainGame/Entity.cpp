#include "RainGame.h"


Player::Player() {
	state = "idle";
}

bool Player::Key(dword key) {
	
	if (key == K_LEFT) {
		velocity.x = -80;
		return true;
	}
	if (key == K_RIGHT) {
		velocity.x = +80;
		return true;
	}
	if (key == (K_LEFT|K_KEYUP)) {
		velocity.x = 0;
		return true;
	}
	if (key == (K_RIGHT|K_KEYUP)) {
		velocity.x = 0;
		return true;
	}
	if (key == K_UP) {
		if (collide_ground)
			velocity.y = 160;
		return true;
	}
	if (key == (K_SPACE|K_KEYUP)) {
		ReleaseDrops();
		return true;
	}
	if (key == K_SPACE) {
		is_umbrella = true;
		return true;
	}
	
	
	return false;
}

void Player::Tick() {
	if (velocity.x < 0)
		is_tile_mirror = true;
	if (velocity.x > 0)
		is_tile_mirror = false;
	
	if (is_umbrella) {
		if (velocity.y < -50)
			velocity.y = -50;
	}
	
	if (have_water) {
		for(int i = 0; i < waterdrops.GetCount(); i++) {
			WaterDrop& o = waterdrops[i];
			Tile& tile = GetTileSet().tiles[o.tile_id];
			
			double angle = ts.Elapsed() * 0.007 + i * 2 * M_PI / max_drops;
			double y = sin(angle) * 10;
			double x = cos(angle) * 26;
			o.pos.left = pos.left + pos.Width() / 2 + x - tile.img.GetWidth() / 2;
			o.pos.top = pos.top + y + tile.img.GetHeight();
			
			o.pos.right = o.pos.left + tile.img.GetWidth();
			o.pos.bottom = o.pos.top - tile.img.GetHeight();
			
		}
	}
}


void Player::PostTick() {
	if (!collide_ground) {
		if (!is_umbrella) {
			tile_id = GetTileSet().tiles.Find("player_falling");
			state = "falling";
		} else {
			tile_id = GetTileSet().tiles.Find("player_fallingumbrella");
			state = "fallingumbrella";
		}
	}
	else if (velocity.x != 0) {
		if (!is_umbrella) {
			tile_id = GetTileSet().tiles.Find("player_running");
			state = "running";
		} else {
			if (have_water) {
				tile_id = GetTileSet().tiles.Find("player_runningumbrellaup");
				state = "runningumbrellaup";
			} else {
				tile_id = GetTileSet().tiles.Find("player_runningumbrella");
				state = "runningumbrella";
			}
		}
	}
	else {
		if (!is_umbrella) {
			tile_id = GetTileSet().tiles.Find("player_idle");
			state = "idle";
		} else {
			if (have_water) {
				tile_id = GetTileSet().tiles.Find("player_umbrellaup");
				state = "umbrellaup";
			} else {
				tile_id = GetTileSet().tiles.Find("player_umbrella");
				state = "umbrella";
			}
		}
	}
}

bool Player::AddWaterDrop(int type) {
	if (have_water >= max_drops) return false;
	
	have_water++;
	if (have_water < max_drops) {
		WaterDrop& o = waterdrops.Add();
		o.pos = pos;
		o.tile_id = GetTileSet().tiles.Find("water");
		Tile& tile = GetTileSet().tiles[o.tile_id];
		o.pos.right = o.pos.left + tile.img.GetWidth();
		o.pos.bottom = o.pos.top - tile.img.GetHeight();
		GetWorld().AddObject(o);
	} else {
		for(int i = 0; i < waterdrops.GetCount(); i++) {
			GetWorld().RemoveObject(&waterdrops[i]);
		}
		waterdrops.Clear();
		
		WaterDrop& o = waterdrops.Add();
		o.pos = pos;
		o.tile_id = GetTileSet().tiles.Find("bigwater");
		Tile& tile = GetTileSet().tiles[o.tile_id];
		o.pos.right = o.pos.left + tile.img.GetWidth();
		o.pos.bottom = o.pos.top - tile.img.GetHeight();
		GetWorld().AddObject(o);
	}
	
	return true;
}

void Player::ReleaseDrops() {
	
	if (have_water >= max_drops) {
		
		for(int i = 0; i < 16; i++) {
			WaterRunning* wr = new WaterRunning();
			
			wr->wait = i;
			wr->dir = is_tile_mirror;
			
			Point pt = pos.TopCenter();
			wr->pos.top = pt.y;
			if (is_tile_mirror)
				wr->pos.left = pt.x - pos.GetWidth();
			else
				wr->pos.left = pt.x + pos.GetWidth();
			wr->pos.left -= (int)wr->pos.left % Map::blocksize;
			wr->pos.top -= (int)wr->pos.top % Map::blocksize;
			wr->pos.right = wr->pos.left + Map::blocksize;
			wr->pos.bottom = wr->pos.top - Map::blocksize;
			wr->tile_id = GetTileSet().tiles.Find("runningwater");
			Tile& tile = GetTileSet().tiles[wr->tile_id];
			wr->collide_mask |= 1 << COLLIDE_BORDER;
			wr->collide_mask |= 1 << COLLIDE_RUNNINGWATER;
			
			GetWorld().AddOwnedObject(wr);
		}
		
	}
	
	is_umbrella = false;
	have_water = 0;
	for(int i = 0; i < waterdrops.GetCount(); i++) {
		GetWorld().RemoveObject(&waterdrops[i]);
	}
	waterdrops.Clear();
}
