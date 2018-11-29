#ifndef _RainGame_Game_h_
#define _RainGame_Game_h_

struct MapCtrl : Moveable<MapCtrl> {
	Rect r;
};

struct WorldCtrl : Moveable<WorldCtrl> {
	Rect r;
	String name;
};

class Game : public TopWindow {
	Map map;
	Array<WaterSpawn> waterspawns;
	Array<Enemy> enemies;
	Player pl;
	Object top, bottom;
	
	Vector<WorldCtrl> worldctrls;
	Vector<MapCtrl> mapctrls;
	int world_id = -1;
	int map_id = -1;
	
	double multiplier = 1.5;
	int mode = MODE_WORLDSELECT;
	
	TimeCallback tc;
	
	enum {MODE_WORLDSELECT, MODE_MAPSELECT, MODE_MAP};
	
	static const int map_count = 8;
	static const int map_cols = 4;
	static const int map_rows = 2;
	
public:
	typedef Game CLASSNAME;
	Game();
	
	virtual void Paint(Draw& d);
	virtual bool Key(dword key, int count);
	virtual void Layout();
	virtual void LeftDown(Point p, dword keyflags);
	
	void InitMap();
	void LoadMap(String path);
	void LoadMap(int world, int map);
	void PaintWorlds(Draw& d);
	void PaintMaps(Draw& d);
	void PaintGame(Draw& d);
	
	void Refresh0() {GetWorld().Tick(); Refresh(); tc.Set(1000/60, THISBACK(Refresh0));}
};


#endif
