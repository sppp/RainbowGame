#include "RainGame.h"


Player::Player() {
	acceleration.y = GRAVITY;
	collide_mask |= 1 << COLLIDE_GROUND;
	collide_mask |= 1 << COLLIDE_WATER;
	collide_mask |= 1 << COLLIDE_BORDER;
	collide_mask |= 1 << COLLIDE_RUNNINGWATER;
	collide_mask |= 1 << COLLIDE_THROWABLE;
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
			velocity.y = 200;
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
		} else {
			tile_id = GetTileSet().tiles.Find("player_fallingumbrella");
		}
	}
	else if (velocity.x != 0) {
		if (!is_umbrella) {
			tile_id = GetTileSet().tiles.Find("player_running");
		} else {
			if (have_water) {
				tile_id = GetTileSet().tiles.Find("player_runningumbrellaup");
			} else {
				tile_id = GetTileSet().tiles.Find("player_runningumbrella");
			}
		}
	}
	else {
		if (!is_umbrella) {
			tile_id = GetTileSet().tiles.Find("player_idle");
		} else {
			if (have_water) {
				tile_id = GetTileSet().tiles.Find("player_umbrellaup");
			} else {
				tile_id = GetTileSet().tiles.Find("player_umbrella");
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
		if (water_type == WATER_NORMAL) {
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
	}
	else if (have_water > 0) {
		WaterDrop* wd = new WaterDrop();
		wd->collide_mask |= 1 << COLLIDE_THROWABLE;
		wd->is_thrown = true;
		wd->weight = have_water;
		Point pt = pos.TopCenter();
		
		if (is_tile_mirror) {
			pt.x -= pos.GetWidth();
			wd->velocity.x = -700;
		}
		else {
			pt.x += pos.GetWidth();
			wd->velocity.x = +700;
		}
		wd->tile_id = GetTileSet().tiles.Find("water");
		Tile& tile = GetTileSet().tiles[wd->tile_id];
		wd->pos.left = pt.x - tile.img.GetWidth() / 2;
		wd->pos.top = pt.y - tile.img.GetHeight() / 2;
		wd->pos.right = wd->pos.left + tile.img.GetWidth();
		wd->pos.bottom = wd->pos.top - tile.img.GetHeight();
		
		
		GetWorld().AddOwnedObject(wd);
	}
	
	is_umbrella = false;
	have_water = 0;
	for(int i = 0; i < waterdrops.GetCount(); i++) {
		GetWorld().RemoveObject(&waterdrops[i]);
	}
	waterdrops.Clear();
}

















Enemy::Enemy() {
	Init();
}

void Enemy::Init() {
	acceleration.y = GRAVITY;
	collide_mask |= 1 << COLLIDE_GROUND;
	collide_mask |= 1 << COLLIDE_BORDER;
	collide_mask |= 1 << COLLIDE_RUNNINGWATER;
	collide_mask |= 1 << COLLIDE_THROWABLE;
	
	dir = Random(3) - 1;
}

void Enemy::Tick() {
	
}

void Enemy::PostTick() {
	
	if (weight > 0 ) {
		// Jump
		if (ts.Elapsed() > 1000 && collide_ground) {
			act = Random(ACT_COUNT);
			
			switch (act) {
				case ACT_LEFT:
					velocity.x = -60;
					is_tile_mirror = true;
					break;
				case ACT_RIGHT:
					velocity.x = +60;
					is_tile_mirror = false;
					break;
				case ACT_IDLE:
					is_tile_mirror = !is_tile_mirror;
					velocity.x = 0;
					break;
				case ACT_JUMP:
					velocity.y = 200;
					velocity.x = 0;
					break;
				case ACT_JUMPLEFT:
					velocity.y = 200;
					velocity.x = -60;
					is_tile_mirror = true;
					break;
				case ACT_JUMPRIGHT:
					velocity.y = 200;
					velocity.x = +60;
					is_tile_mirror = false;
					break;
				
			}
			
			
			ts.Reset();
		}
	} else {
		if (ts.Elapsed() > 600) {
			rotate += 90;
			while (rotate >= 360) rotate -= 360;
			
			ts.Reset();
		}
	}
}

void Enemy::TakeHit(int damage, int dir) {
	if (damage <= 0) return;
	
	bool was_ok = weight > 0;
	
	weight -= damage;
	
	if (was_ok && weight <= 0) {
		tile_id++; // change overlay color
		velocity.x = 0;
	}
	
	if (weight < 0) {
		if (dir == C_LEFT)
			velocity.x = +200;
		else if (dir == C_RIGHT)
			velocity.x = -200;
	}
}
