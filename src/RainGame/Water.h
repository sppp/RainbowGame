#ifndef _RainGame_Water_h_
#define _RainGame_Water_h_

class WaterDrop : public Object {
	bool dir = false;
	
	virtual void PostTick();
	virtual void Tick();
};

struct WaterSpawn : public Object {
	TimeStop ts;
	
	virtual void Tick();
};

struct WaterRunning : public Object {
	Vector<Object*> moved_objects;
	TimeStop ts;
	int wait = 0;
	int prev_steps = 0;
	bool dir = false;
	
	static const int step = 100;
	
	virtual void Tick();
	void ReleaseObjs();
};


#endif
