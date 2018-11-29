#ifndef _RainGame_Entity_h_
#define _RainGame_Entity_h_

struct Player : public Entity {
	bool is_umbrella = false;
	int have_water = 0;
	int water_type = WATER_NORMAL;
	TimeStop ts;
	
	Array<WaterDrop> waterdrops;
	
	static const int max_drops = 5;
	
	Player();
	bool Key(dword key);
	virtual void Tick();
	virtual void PostTick();
	bool AddWaterDrop(int type);
	void ReleaseDrops();
};


struct Enemy : public Entity {
	TimeStop ts;
	int dir = 0;
	int act = 0;
	
	enum {ACT_LEFT, ACT_RIGHT, ACT_IDLE, ACT_JUMP, ACT_JUMPLEFT, ACT_JUMPRIGHT, ACT_COUNT};
	
	Enemy();
	void Init();
	virtual void Tick();
	virtual void PostTick();
	void SwitchDirection();
	void TakeHit(int damage, int dir);
};

#endif
