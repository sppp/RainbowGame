#include "RainGame.h"

Map::Map() {
	
	player_start = Point(10 * blocksize, 20 * blocksize);
	
	
	Tile& block1 = GetTileSet().tiles.Get("block1");
	Size block1_size = block1.GetSize();
	for(int i = 0; i < 100; i++) {
		int j = Random(width*height);
		MapTile& o = maptiles.Add();
		o.pos.left = (j % width) * blocksize;
		o.pos.top = (j / width) * blocksize;
		o.pos.right = o.pos.left + block1_size.cx;
		o.pos.bottom = o.pos.top - block1_size.cy;
		o.tile_id = block1.id;
		o.fixed |= 1 << FIXED_TOP;
		o.collide_mask |= 1 << COLLIDE_GROUND;
	}
	
	WaterSpawn& ws = waterspawns.Add();
	ws.pos.left = 10 * blocksize;
	ws.pos.top = 10 * blocksize;
	ws.pos.right = ws.pos.left + 1;
	ws.pos.bottom = ws.pos.top + 1;
}






void MapRenderer::Paint(Draw& d) {
	Map& map = *this->map;
	
	Tile& tl = GetTileSet().tiles.Get("bg");
	
	d.DrawImage(0, 0, Rescale(tl.img, tl.img.GetSize() * multiplier));
	
	for(int i = 0; i < map.maptiles.GetCount(); i++) {
		Object& o = map.maptiles[i];
		
		if (o.tile_id >= 0) {
			int x0 = o.pos.left * multiplier;
			int y0 = (map.height * map.blocksize - o.pos.top) * multiplier;
			int x1 = o.pos.right * multiplier;
			int y1 = (map.height * map.blocksize - o.pos.bottom) * multiplier;
			Tile& tile = GetTileSet().tiles[o.tile_id];
			
			Image i = tile.img;
			if (o.IsTileMirror())
				i = MirrorHorz(i);
			
			d.DrawImage(x0, y0, Rescale(i, i.GetSize() * multiplier));
			
			d.DrawLine(x0, y0, x1, y0, 1, Color(255,0,0));
			d.DrawLine(x0, y1, x1, y1, 1, Color(255,0,0));
			d.DrawLine(x0, y0, x0, y1, 1, Color(255,0,0));
			d.DrawLine(x1, y0, x1, y1, 1, Color(255,0,0));
		}
	}
	
	d.DrawRect(
		map.player_start.x - 3,
		map.player_start.y - 3,
		6, 6, Color(255, 42, 200));
	
}

void MapRenderer::LeftDown(Point p, dword keyflags) {
	DoDraw(p);
	SetCapture();
	is_drawing = true;
}

void MapRenderer::LeftUp(Point p, dword keyflags) {
	
	ReleaseCapture();
	is_drawing = false;
}

void MapRenderer::MouseMove(Point p, dword keyflags) {
	
	if (is_drawing) {
		DoDraw(p);
	}
	
}

void MapRenderer::DoDraw(Point p) {
	int xi = p.x / (map->blocksize * multiplier);
	int yi = map->height - p.y / (map->blocksize * multiplier);
	
	Tile& block1 = GetTileSet().tiles.Get("block1");
	Size block1_size = block1.GetSize();
	
	if (xi >= 0 && xi < map->width && yi >= 0 && yi < map->height) {
		MapTile& o = map->maptiles.Add();
		o.tile_id = block1.id;
		o.pos.left = xi * map->blocksize;
		o.pos.top = yi * map->blocksize;
		o.pos.right = o.pos.left + block1_size.cx;
		o.pos.bottom = o.pos.top - block1_size.cy;
		o.fixed |= 1 << FIXED_TOP;
		o.collide_mask |= 1 << COLLIDE_GROUND;
	}
	
	Refresh();
}




MapEditor::MapEditor() {
	Sizeable().MaximizeBox().MinimizeBox();
	Add(hsplit.SizePos());
	AddFrame(menu);
	
	hsplit << vsplit << renderer;
	hsplit.SetPos(1000);
	hsplit.Horz();
	
	vsplit.Vert();
	vsplit << tilelist;
	
	renderer.map = &map;
	
	menu.Set(THISBACK(MainMenu));
	
}

void MapEditor::Data() {
	
}

void MapEditor::MainMenu(Bar& bar) {
	bar.Sub("File", [&](Bar& bar) {
		bar.Add("Open", THISBACK(Open));
		bar.Add("Save", THISBACK(Save)).Key(K_CTRL_S);
		bar.Add("Save As", THISBACK(SaveAs));
		
	});
	bar.Sub("Test", [&](Bar& bar) {
		bar.Add("Play map", THISBACK(PlayMap)).Key(K_F5);
		
	});
	
	
}

void MapEditor::Open() {
	String path = SelectFileOpen("*.map");
	if (FileExists(path)) {
		open_path = path;
		FileIn fin(path);
		fin % map;
		Data();
	}
}

void MapEditor::Save() {
	if (open_path.IsEmpty())
		SaveAs();
	else {
		FileOut fout(open_path);
		fout % map;
	}
}

void MapEditor::SaveAs() {
	open_path = SelectFileSaveAs("*.map");
	if (!open_path.IsEmpty())
		Save();
}

void MapEditor::PlayMap() {
	if (open_path.IsEmpty())
		SaveAs();
	else
		Save();
	if (open_path.IsEmpty())
		return;
	Thread::Start(THISBACK(Start));
}

void MapEditor::Start() {
	String out;
	LocalProcess lp;
	String path = GetExeFilePath() + " " + open_path;
	lp.Start(path);
	lp.Finish(out);
}
