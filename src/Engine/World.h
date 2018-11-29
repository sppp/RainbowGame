#ifndef _Engine_World_h_
#define _Engine_World_h_



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
	void Clear();
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
