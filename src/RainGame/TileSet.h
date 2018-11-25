#ifndef _RainGame_TileSet_h_
#define _RainGame_TileSet_h_

struct Tile {
	Image img;
	int id = -1;
	
	
	Size GetSize() {return img.GetSize();}
	void Serialize(Stream& s) {s % img % id;}
};

struct TileSet {
	ArrayMap<String, Tile> tiles;
	
	TileSet();
	
	void Load(int world);
	void Load(String key, String file, bool is_solid);
	
	void Serialize(Stream& s) {s % tiles;}
};

inline TileSet& GetTileSet() {return Single<TileSet>();}

#endif
