#ifndef _Engine_TileSet_h_
#define _Engine_TileSet_h_

struct Tile {
	Image img;
	int id = -1;
	
	
	Size GetSize() {return img.GetSize();}
	void Serialize(Stream& s) {s % img % id;}
};

struct TileSet {
	ArrayMap<String, Tile> tiles;
	
	TileSet();
	
	void Load();
	void Load(String key);
	void LoadGreen(String from_key);
	
	void Serialize(Stream& s) {s % tiles;}
};

inline TileSet& GetTileSet() {return Single<TileSet>();}


#endif
