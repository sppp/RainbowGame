#ifndef _RainGame_Entity_h_
#define _RainGame_Entity_h_


struct Entity : public Object {
	
};


struct Player : public Entity {
	String state;
	bool is_umbrella = false;
	int have_water = 0;
	
	Player();
	String GetState() {return state;}
	bool Key(dword key);
	virtual void Tick();
	virtual void PostTick();
};



#endif
