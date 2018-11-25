#ifndef _RainGame_Game_h_
#define _RainGame_Game_h_


class Game : public TopWindow {
	Map map;
	Player pl;
	
	double multiplier = 1.5;
	
	TimeCallback tc;
public:
	typedef Game CLASSNAME;
	Game();
	
	virtual void Paint(Draw& d);
	virtual bool Key(dword key, int count);
	virtual void Layout();
	
	void InitMap();
	void LoadMap(String path);
	
	void Refresh0() {GetWorld().Tick(); Refresh(); tc.Set(1000/60, THISBACK(Refresh0));}
};


#endif
