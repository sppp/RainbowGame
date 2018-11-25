#ifndef _RainGame_MapEditor_h_
#define _RainGame_MapEditor_h_

enum {COLLIDE_GROUND};

struct MapTile : public Object {
	
	
	void Serialize(Stream& s) {Object::Serialize(s);}
};

struct Map {
	Array<MapTile> maptiles;
	Array<WaterSpawn> waterspawns;
	int width = 40;
	int height = 30;
	Point player_start;
	
	static const int blocksize = 16;
	
	
	Map();
	void Serialize(Stream& s) {s % maptiles % waterspawns % width % height % player_start;}
};

struct MapRenderer : public Ctrl {
	Map* map;
	bool is_drawing = false;
	
	static const int multiplier = 1;
	
	void DoDraw(Point p);
	virtual void Paint(Draw& d);
	virtual void LeftDown(Point p, dword keyflags);
	virtual void LeftUp(Point p, dword keyflags);
	virtual void MouseMove(Point p, dword keyflags);
};

class MapEditor : public TopWindow {
	Map map;
	String open_path;
	
	MapRenderer renderer;
	Splitter vsplit, hsplit;
	ArrayCtrl tilelist;
	
	MenuBar menu;
	
public:
	typedef MapEditor CLASSNAME;
	MapEditor();
	
	void Data();
	
	void MainMenu(Bar& bar);
	void Open();
	void Save();
	void SaveAs();
	void PlayMap();
	void Start();
};

#endif
