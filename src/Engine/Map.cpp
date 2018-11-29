#include "Engine.h"

Map::Map() {
	
	player_start = Point(10 * blocksize, 20 * blocksize);
	
	
	/*Tile& block1 = GetTileSet().tiles.Get("w1_block1");
	Size block1_size = block1.GetSize();
	for(int i = 0; i < 100; i++) {
		int j = Random(((double)width/blocksize)*((double)height/blocksize));
		MapTile& o = maptiles.Add();
		o.pos.left = (j % (width / blocksize)) * blocksize;
		o.pos.top = (j / (width / blocksize)) * blocksize;
		o.pos.right = o.pos.left + block1_size.cx;
		o.pos.bottom = o.pos.top - block1_size.cy;
		o.tile_id = block1.id;
		o.fixed |= 1 << FIXED_TOP;
		o.collide_mask |= 1 << COLLIDE_GROUND;
	}*/
	
	/*WaterSpawn& ws = waterspawns.Add();
	ws.pos.left = 10 * blocksize;
	ws.pos.top = 10 * blocksize;
	ws.pos.right = ws.pos.left + 1;
	ws.pos.bottom = ws.pos.top - 1;*/
}

String Map::Get() {
	String out;
	for(int i = 0; i < objects.GetCount(); i++) {
		Object& t = objects[i];
		out << "o "
			<< t.name << " "
			<< IntStr(t.pos.left) << " "
			<< IntStr(t.pos.top) << " "
			<< IntStr(t.pos.right) << " "
			<< IntStr(t.pos.bottom) << " "
			<< IntStr(t.tile_id) << " "
			<< IntStr(t.fixed) << " "
			<< IntStr(t.collide_mask)
			<< "\n";
	}
	out << "size " << IntStr(width) << " " << IntStr(height) << "\n";
	out << "start " << IntStr(player_start.x) << " " << IntStr(player_start.y) << "\n";
	return out;
}

void Map::Set(const String& s) {
	objects.Clear();
	Vector<String> lines = Split(s, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		Vector<String> line = Split(lines[i], " ");
		if (line.IsEmpty()) continue;
		
		if (line[0] == "o") {
			Object& o = objects.Add();
			o.name = line[1];
			o.pos.left = StrInt(line[2]);
			o.pos.top = StrInt(line[3]);
			o.pos.right = StrInt(line[4]);
			o.pos.bottom = StrInt(line[5]);
			o.tile_id = StrInt(line[6]);
			o.fixed = StrInt(line[7]);
			o.collide_mask = StrInt(line[8]);
		}
		else if (line[0] == "size") {
			width = StrInt(line[1]);
			height = StrInt(line[2]);
		}
		else if (line[0] == "start") {
			player_start.x = StrInt(line[1]);
			player_start.y = StrInt(line[2]);
		}
	}
}

