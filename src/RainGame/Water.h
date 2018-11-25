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




#endif
