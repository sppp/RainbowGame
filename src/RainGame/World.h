#ifndef _RainGame_World_h_
#define _RainGame_World_h_

enum {FIXED_TOP, FIXED_BOTTOM, FIXED_SIDES};


struct Object {
	String name;
	Rectf prev_pos, pos;
	Pointf velocity, acceleration;
	byte fixed = 0;
	dword collide_mask = 0;
	int tile_id = -1;
	bool collide_ground = false;
	bool is_tile_mirror = false;
	bool is_frozen = false;
	
	Object() {
		pos = RectC(0,0,0,0);
		prev_pos = RectC(0,0,0,0);
		velocity = Pointf(0,0);
		acceleration = Pointf(0,0);
	}
	
	bool IsTileMirror() {return is_tile_mirror;}
	
	void Serialize(Stream& s) {
		s % prev_pos % pos % velocity % acceleration % fixed % collide_mask
		  % tile_id % collide_ground % is_tile_mirror;
	}
	
	virtual void Tick() {}
	virtual void PostTick() {}
};


typedef void (*CollideFn)(Object& a, Object& b, int);

enum {C_LEFT, C_RIGHT, C_TOP, C_BOTTOM};


struct World {
	TimeStop ts;
	Vector<Object*> obj;
	Vector<Object*> owned_obj;
	Vector<CollideFn> collide_fns;
	
	
public:
	typedef World CLASSNAME;
	World();
	~World();
	void AddObject(Object& o) {obj.Add(&o); ASSERT(o.pos.left <= o.pos.right && o.pos.top >= o.pos.bottom);}
	void AddOwnedObject(Object* o) {obj.Add(o); owned_obj.Add(o);}
	void Tick();
	void FixedCollide(Object& fixed, Object& dynamic, CollideFn fn);
	void DynamicCollide(Object& fixed, Object& dynamic, CollideFn fn);
	void SetCollideFunction(int i, CollideFn fn) {collide_fns[i] = fn;}
	void RemoveObject(Object* o);
	Object* FindObject(String name);
	
	int GetObjectCount() {return obj.GetCount();}
	Object& GetObject(int i) {return *obj[i];}
	
};

inline World& GetWorld() {return Single<World>();}


#endif
