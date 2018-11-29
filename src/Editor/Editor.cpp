#include "Editor.h"

#define IMAGECLASS EditorImg
#define IMAGEFILE <Editor/Editor.iml>
#include <Draw/iml_source.h>




void MapRenderer::Paint(Draw& d) {
	Map& map = *this->map;
	
	Tile& tl = GetTileSet().tiles.Get("w1_bg");
	
	d.DrawImage(0, 0, Rescale(tl.img, tl.img.GetSize() * multiplier));
	
	for(int i = 0; i < map.objects.GetCount(); i++) {
		Object& o = map.objects[i];
		
		if (o.tile_id >= 0 && o.tile_id < GetTileSet().tiles.GetCount()) {
			int x0 = o.pos.left * multiplier;
			int y0 = (map.height - o.pos.top) * multiplier;
			int x1 = o.pos.right * multiplier;
			int y1 = (map.height - o.pos.bottom) * multiplier;
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
	int yi = map->height / Map::blocksize - p.y / (map->blocksize * multiplier);
	
	if (editor->tool_id == TOOL_PEN) {
		
		if (xi >= 0 && xi < map->width / map->blocksize && yi >= 0 && yi < map->height / map->blocksize){
			Object& o = map->objects.Add();
			Object& from = editor->tile_objects[editor->tilelist.GetCursor()];
			o.Object::operator=(from);
			double w = o.pos.GetWidth();
			double h = -o.pos.GetHeight();
			o.pos.left = xi * map->blocksize;
			o.pos.top = yi * map->blocksize;
			o.pos.right = o.pos.left + w;
			o.pos.bottom = o.pos.top - h;
		}
		
	}
	else if (editor->tool_id == TOOL_ERASE) {
		int l = xi * map->blocksize;
		int t = yi * map->blocksize;
		for(int i = 0; i < map->objects.GetCount(); i++) {
			Object& o = map->objects[i];
			if (o.pos.left == l && o.pos.top == t) {
				map->objects.Remove(i);
			}
		}
	}
	
	Refresh();
}




MapEditor::MapEditor() {
	Sizeable().MaximizeBox().MinimizeBox();
	Add(hsplit.SizePos());
	AddFrame(menu);
	AddFrame(tool);
	
	hsplit << vsplit << renderer;
	hsplit.SetPos(1000);
	hsplit.Horz();
	
	vsplit.Vert();
	vsplit << tilelist;
	
	tilelist.AddColumn("Title");
	tilelist.AddColumn("Image");
	
	renderer.map = &map;
	renderer.editor = this;
	
	menu.Set(THISBACK(MainMenu));
	
	tool.Set(THISBACK(ToolMenu));
	
	AddTileObject("block1", "w1_block1", FIXED_TOP);
	AddTileObject("sidewall", "w1_block1", FIXED_SIDES);
	AddTileObject("block1_shadow", "w1_block1_shadow");
	AddTileObject("curtain1", "w1_curtain1");
	AddTileObject("curtain2", "w1_curtain2");
	AddTileObject("curtain3", "w1_curtain3");
	AddTileObject("waterdrop", "waterdrop");
	AddTileObject("enemy1", "w1_enemy1");
	
	PostCallback(THISBACK(Data));
}

void MapEditor::MapEditor::Data() {
	for(int i = 0; i < tile_objects.GetCount(); i++) {
		Object& o = tile_objects[i];
		tilelist.Set(i, 0, o.name);
		tilelist.Set(i, 1, GetTileSet().tiles[o.tile_id].img);
		tilelist.SetDisplay(i, 1, ImageDisplay());
	}
	
	Refresh();
}

void MapEditor::AddTileObject(String name, String tile_name, int fixed_id) {
	Object& o = tile_objects.Add();
	o.name = name;
	o.tile_id = GetTileSet().tiles.Find(tile_name);
	ASSERT(o.tile_id != -1);
	Tile& tile = GetTileSet().tiles[o.tile_id];
	o.pos.top = 0;
	o.pos.left = 0;
	o.pos.bottom = -tile.img.GetHeight();
	o.pos.right = tile.img.GetWidth();
	if (fixed_id >= 0) {
		o.fixed |= 1 << fixed_id;
		o.collide_mask |= 1 << COLLIDE_GROUND;
		o.collide_mask |= 1 << COLLIDE_THROWABLE;
	}
}

void MapEditor::MainMenu(Bar& bar) {
	bar.Sub("File", [&](Bar& bar) {
		bar.Sub("Open map", [=](Bar& bar) {
			for(int i = 0; i < 8; i++) {
				bar.Sub("World " + IntStr(i), [=](Bar& bar) {
					for(int j = 0; j < 8; j++) {
						bar.Add("Map " + IntStr(j), THISBACK2(OpenMap, i, j));
					}
				});
			}
		});
		
		bar.Add("Open", THISBACK(Open));
		bar.Add("Save", THISBACK(Save)).Key(K_CTRL_S);
		bar.Add("Save As", THISBACK(SaveAs));
		
	});
	bar.Sub("Test", [&](Bar& bar) {
		bar.Add("Play map", THISBACK(PlayMap)).Key(K_F5);
		
	});
	
	
}

void MapEditor::ToolMenu(Bar& bar) {
	bar.Add("Pen", EditorImg::PenTool, THISBACK1(SetTool, TOOL_PEN)).Key(K_1);
	bar.Add("Erase", EditorImg::EraseTool, THISBACK1(SetTool, TOOL_ERASE)).Key(K_2);
}

void MapEditor::Open() {
	String path = SelectFileOpen("*.map");
	if (FileExists(path)) {
		open_path = path;
		map.Set(LoadFile(path));
		Data();
	}
}

void MapEditor::OpenMap(int world, int map) {
	String path = AppendFileName(GetDataFile("maps"), "w" + IntStr(world+1) + "_" + IntStr(map+1) + ".map");
	open_path = path;
	if (FileExists(path)) {
		this->map.Set(LoadFile(path));
		Data();
	}
}

void MapEditor::Save() {
	if (open_path.IsEmpty())
		SaveAs();
	else {
		FileOut fout(open_path);
		fout << map.Get();
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

