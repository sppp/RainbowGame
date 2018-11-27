#include "RainGame.h"

World::World() {
	collide_fns.SetCount(32, NULL);
}

World::~World() {
	for(int i = 0; i < owned_obj.GetCount(); i++)
		delete owned_obj[i];
	owned_obj.Clear();
}

void World::RemoveObject(Object* o) {
	for(int i = 0; i < obj.GetCount(); i++) {
		if (obj[i] == o) {
			obj.Remove(i);
			i--;
		}
	}
}

Object* World::FindObject(String name) {
	for(int i = 0; i < obj.GetCount(); i++) {
		Object* o = obj[i];
		if (o->name.GetCount() == 0) continue;
		if (o->name == name)
			return o;
	}
	return NULL;
}

void World::Tick() {
	
	dword elapsed = ts.Elapsed();
	ts.Reset();
	
	for(int i = 0; i < obj.GetCount(); i++) {
		Object& o = *obj[i];
		if (o.is_frozen)
			continue;
		
		o.velocity += elapsed * 0.001 * o.acceleration;
		Pointf movement = o.velocity * elapsed * 0.001;
		o.prev_pos = o.pos;
		o.pos.left += movement.x;
		o.pos.right += movement.x;
		o.pos.top += movement.y;
		o.pos.bottom += movement.y;
		o.collide_ground = false;
		
		o.Tick();
	}
	
	
	for(int i = 0; i < obj.GetCount(); i++) {
		Object& o0 = *obj[i];
		if (o0.is_frozen)
			continue;
		
		for(int j = i+1; j < obj.GetCount(); j++) {
			Object& o1 = *obj[j];
			if (o1.is_frozen)
				continue;
		
			dword mask = o0.collide_mask & o1.collide_mask;
			if (mask == 0)
				continue;
			
			if (o1.fixed && o0.fixed)
				continue;
			
			for(int k = 0; k < COLLIDE_COUNT; k++) {
				if (mask & (1 << k)) {
					if (k == COLLIDE_BORDER) {
						k = COLLIDE_BORDER;
					}
					if (o0.fixed)
						FixedCollide(o0, o1, collide_fns[k]);
					else if (o1.fixed)
						FixedCollide(o1, o0, collide_fns[k]);
					else {
						DynamicCollide(o1, o0, collide_fns[k]);
						DynamicCollide(o0, o1, collide_fns[k]);
					}
				}
			}
		}
	}
	
	for(int i = 0; i < obj.GetCount(); i++) {
		Object& o = *obj[i];
		if (o.is_frozen)
			continue;
		o.PostTick();
	}
}

void World::FixedCollide(Object& fixed, Object& dynamic, CollideFn fn) {
	bool up_col = dynamic.pos.top > fixed.pos.top && dynamic.pos.bottom <= fixed.pos.top;
	bool bot_col = dynamic.pos.bottom < fixed.pos.bottom && dynamic.pos.top >= fixed.pos.bottom;
	bool vinside = dynamic.pos.top <= fixed.pos.top && dynamic.pos.bottom >= fixed.pos.bottom;
	bool voutside = dynamic.pos.top >= fixed.pos.top && dynamic.pos.bottom <= fixed.pos.bottom;
	bool left_col = dynamic.pos.left < fixed.pos.left && dynamic.pos.right >= fixed.pos.left;
	bool right_col = dynamic.pos.right > fixed.pos.right && dynamic.pos.left <= fixed.pos.right;
	bool hinside = dynamic.pos.left >= fixed.pos.left && dynamic.pos.right <= fixed.pos.right;
	bool houtside = dynamic.pos.left <= fixed.pos.left && dynamic.pos.right >= fixed.pos.right;
	bool prevtop_col = dynamic.prev_pos.bottom >= fixed.pos.top && dynamic.pos.bottom < fixed.pos.top;
	bool prevbot_col = dynamic.prev_pos.top <= fixed.pos.bottom && dynamic.pos.top > fixed.pos.bottom;
	bool prevleft_col = dynamic.prev_pos.right <= fixed.pos.left && dynamic.pos.right > fixed.pos.left;
	bool prevright_col = dynamic.prev_pos.left >= fixed.pos.right && dynamic.pos.left < fixed.pos.right;
	
	if (fixed.fixed & (1 << FIXED_SIDES)) {
		if (up_col || bot_col || vinside || voutside) {
			if (prevleft_col) {
				double diff = dynamic.pos.right - fixed.pos.left;
				dynamic.pos.left -= diff;
				dynamic.pos.right -= diff;
				dynamic.velocity.x = 0;
				if (fn) fn(fixed, dynamic, C_LEFT);
			}
			if (prevright_col) {
				double diff = fixed.pos.right - dynamic.pos.left;
				dynamic.pos.left += diff;
				dynamic.pos.right += diff;
				dynamic.velocity.x = 0;
				if (fn) fn(fixed, dynamic, C_RIGHT);
			}
		}
	}
	
	if (left_col || right_col || hinside || houtside) {
		if (fixed.fixed & (1 << FIXED_TOP)) {
			if (prevtop_col) {
				double diff = fixed.pos.top - dynamic.pos.bottom;
				dynamic.pos.top += diff;
				dynamic.pos.bottom += diff;
				dynamic.velocity.y = 0;
				if (fn) fn(fixed, dynamic, C_TOP);
			}
		}
		
		if (fixed.fixed & (1 << FIXED_BOTTOM)) {
			if (prevbot_col) {
				double diff = dynamic.pos.top - fixed.pos.bottom;
				dynamic.pos.top -= diff;
				dynamic.pos.bottom -= diff;
				dynamic.velocity.y = 0;
				if (fn) fn(fixed, dynamic, C_BOTTOM);
			}
		}
	}
	
}

void World::DynamicCollide(Object& fixed, Object& dynamic, CollideFn fn) {
	Size fixed_size = fixed.pos.Size();
	Size dynamic_size = dynamic.pos.Size();
	bool up_col = dynamic.pos.top > fixed.pos.top && dynamic.pos.bottom < fixed.pos.top;
	bool bot_col = dynamic.pos.bottom < fixed.pos.bottom && dynamic.pos.top > fixed.pos.bottom;
	bool vinside = dynamic.pos.top <= fixed.pos.top && dynamic.pos.bottom >= fixed.pos.bottom;
	bool voutside = dynamic.pos.top >= fixed.pos.top && dynamic.pos.bottom <= fixed.pos.bottom;
	bool left_col = dynamic.pos.left < fixed.pos.left && dynamic.pos.right > fixed.pos.left;
	bool right_col = dynamic.pos.right > fixed.pos.right && dynamic.pos.left < fixed.pos.right;
	bool hinside = dynamic.pos.left >= fixed.pos.left && dynamic.pos.right <= fixed.pos.right;
	bool houtside = dynamic.pos.left <= fixed.pos.left && dynamic.pos.right >= fixed.pos.right;
	
	if (up_col || bot_col || vinside || voutside) {
		if (left_col) {
			if (fn) fn(fixed, dynamic, C_LEFT);
		}
		if (right_col) {
			if (fn) fn(fixed, dynamic, C_RIGHT);
		}
	}
	
	if (left_col || right_col || hinside || houtside) {
		if (up_col) {
			if (fn) fn(fixed, dynamic, C_TOP);
		}
		
		if (bot_col) {
			if (fn) fn(fixed, dynamic, C_BOTTOM);
		}
	}
	
}
