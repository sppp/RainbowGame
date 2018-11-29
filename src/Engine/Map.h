#ifndef _Engine_Map_h_
#define _Engine_Map_h_

struct Map {
	Array<Object> objects;
	int width = 40 * blocksize;
	int height = 30 * blocksize;
	Point player_start;
	
	static const int blocksize = 16;
	
	
	Map();
	String Get();
	void Set(const String& s);
	//void Serialize(Stream& s) {s % maptiles % waterspawns % width % height % player_start;}
};


#endif
