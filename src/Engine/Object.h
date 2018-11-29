#ifndef _Engine_Object_h_
#define _Engine_Object_h_


enum {FIXED_TOP, FIXED_BOTTOM, FIXED_SIDES};


struct Object {
	String name;
	Rectf pos;
	int tile_id = -1;
	byte fixed = 0;
	dword collide_mask = 0;
	
	Rectf prev_pos;
	Pointf velocity, acceleration;
	int weight = 1;
	int rotate = 0;
	bool collide_ground = false;
	bool is_tile_mirror = false;
	bool is_frozen = false;
	bool is_thrown = false;
	
	Object() {
		pos = RectC(0,0,0,0);
		prev_pos = RectC(0,0,0,0);
		velocity = Pointf(0,0);
		acceleration = Pointf(0,0);
	}
	
	bool IsTileMirror() {return is_tile_mirror;}
	
	void Serialize(Stream& s) {
		s % name % prev_pos % pos % velocity % acceleration % fixed % collide_mask
		  % tile_id % collide_ground % is_tile_mirror % is_frozen % is_thrown;
	}
	void operator=(const Object& s) {
		name = s.name;
		prev_pos = s.prev_pos;
		pos = s.pos;
		velocity = s.velocity;
		acceleration = s.acceleration;
		fixed = s.fixed;
		collide_mask = s.collide_mask;
		tile_id = s.tile_id;
		collide_ground = s.collide_ground;
		is_tile_mirror = s.is_tile_mirror;
		is_frozen = s.is_frozen;
		is_thrown = s.is_thrown;
	}
	virtual void Tick() {}
	virtual void PostTick() {}
};

#endif
