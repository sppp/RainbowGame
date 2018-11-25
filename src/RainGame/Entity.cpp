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
		is_umbrella = false;
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