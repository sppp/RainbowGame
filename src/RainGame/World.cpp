#include "RainGame.h"

World::World() {
	collide_fns.SetCount(32, NULL);
}

void World::Tick() {
	
	dword elapsed = ts.Elapsed();
	ts.Reset();
	
	for(int i = 0; i < obj.GetCount(); i++) {
		Object& o = *obj[i];
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
		for(int j = i+1; j < obj.GetCount(); j++) {
			Object& o1 = *obj[j];
			
			dword mask = o0.collide_mask & o1.collide_mask;
			if (mask == 0)
				continue;
			
			if (o1.fixed && o0.fixed)
				continue;
			
			for(int k = 0; k < 32; k++) {
				if (mask & (1 << k)) {
					if (o0.fixed)
						FixedCollide(o0, o1, collide_fns[k]);
					else if (o1.fixed)
						FixedCollide(o1, o0, collide_fns[k]);
					else
						;//Panic("TODO");
				}
			}
		}
	}
	
	for(int i = 0; i < obj.GetCount(); i++) {
		obj[i]->PostTick();
	}
}

void World::FixedCollide(Object& fixed, Object& dynamic, CollideFn fn) {
	
	if (fixed.fixed & (1 << FIXED_SIDES)) {
		if (dynamic.prev_pos.right <= fixed.pos.left && dynamic.pos.right > fixed.pos.left) {
			double diff = dynamic.pos.right - fixed.pos.left;
			dynamic.pos.left -= diff;
			dynamic.pos.right -= diff;
			dynamic.velocity.x = 0;
			if (fn) fn(fixed, dynamic, C_LEFT);
		}
		if (dynamic.prev_pos.left >= fixed.pos.right && dynamic.pos.left < fixed.pos.right) {
			double diff = fixed.pos.right - dynamic.pos.left;
			dynamic.pos.left += diff;
			dynamic.pos.right += diff;
			dynamic.velocity.x = 0;
			if (fn) fn(fixed, dynamic, C_RIGHT);
		}
	}
	
	if (
		(dynamic.pos.left >= fixed.pos.left && dynamic.pos.left <= fixed.pos.right) ||
		(dynamic.pos.right >= fixed.pos.left && dynamic.pos.right <= fixed.pos.right) ||
		(dynamic.pos.left <= fixed.pos.left && dynamic.pos.right >= fixed.pos.right) ||
		(dynamic.pos.left >= fixed.pos.left && dynamic.pos.right <= fixed.pos.right)
		
		) {
		if (fixed.fixed & (1 << FIXED_TOP)) {
			if (dynamic.prev_pos.bottom >= fixed.pos.top && dynamic.pos.bottom < fixed.pos.top) {
				double diff = fixed.pos.top - dynamic.pos.bottom;
				dynamic.pos.top += diff;
				dynamic.pos.bottom += diff;
				dynamic.velocity.y = 0;
				if (fn)
					fn(fixed, dynamic, C_TOP);
			}
		}
		
		if (fixed.fixed & (1 << FIXED_BOTTOM)) {
			if (dynamic.prev_pos.top <= fixed.pos.bottom && dynamic.pos.top > fixed.pos.bottom) {
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
	
	if (fixed.fixed & (1 << FIXED_SIDES)) {
		if (dynamic.prev_pos.right <= fixed.pos.left && dynamic.pos.right > fixed.pos.left) {
			double diff = dynamic.pos.right - fixed.pos.left;
			dynamic.pos.left -= diff;
			dynamic.pos.right -= diff;
			dynamic.velocity.x = 0;
		}
		if (dynamic.prev_pos.left >= fixed.pos.right && dynamic.pos.left < fixed.pos.right) {
			double diff = fixed.pos.right - dynamic.pos.left;
			dynamic.pos.left += diff;
			dynamic.pos.right += diff;
			dynamic.velocity.x = 0;
		}
	}
	
	if (
		(dynamic.pos.left >= fixed.pos.left && dynamic.pos.left <= fixed.pos.right) ||
		(dynamic.pos.right >= fixed.pos.left && dynamic.pos.right <= fixed.pos.right) ||
		(dynamic.pos.left <= fixed.pos.left && dynamic.pos.right >= fixed.pos.right) ||
		(dynamic.pos.left >= fixed.pos.left && dynamic.pos.right <= fixed.pos.right)
		
		) {
		if (fixed.fixed & (1 << FIXED_TOP)) {
			if (dynamic.prev_pos.bottom >= fixed.pos.top && dynamic.pos.bottom < fixed.pos.top) {
				double diff = fixed.pos.top - dynamic.pos.bottom;
				dynamic.pos.top += diff;
				dynamic.pos.bottom += diff;
				dynamic.velocity.y = 0;
			}
		}
		
		if (fixed.fixed & (1 << FIXED_BOTTOM)) {
			if (dynamic.prev_pos.top <= fixed.pos.bottom && dynamic.pos.top > fixed.pos.bottom) {
				double diff = dynamic.pos.top - fixed.pos.bottom;
				dynamic.pos.top -= diff;
				dynamic.pos.bottom -= diff;
				dynamic.velocity.y = 0;
			}
		}
	}
	
}
