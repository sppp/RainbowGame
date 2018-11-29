#ifndef _Editor_Editor_h_
#define _Editor_Editor_h_

#include <Engine/Engine.h>

#define IMAGECLASS EditorImg
#define IMAGEFILE <Editor/Editor.iml>
#include <Draw/iml_header.h>


enum {TOOL_PEN, TOOL_ERASE};

class MapEditor;

struct MapRenderer : public Ctrl {
	Map* map;
	MapEditor* editor;
	bool is_drawing = false;
	
	static const int multiplier = 1;
	
	void DoDraw(Point p);
	virtual void Paint(Draw& d);
	virtual void LeftDown(Point p, dword keyflags);
	virtual void LeftUp(Point p, dword keyflags);
	virtual void MouseMove(Point p, dword keyflags);
};

class MapEditor : public TopWindow {
	
protected:
	friend class MapRenderer;
	
	Map map;
	String open_path;
	
	MapRenderer renderer;
	Splitter vsplit, hsplit;
	ArrayCtrl tilelist;
	
	MenuBar menu;
	ToolBar tool;
	
	int tool_id = 0;
	Array<Object> tile_objects;
	
	void AddTileObject(String name, String tile_name, int fixed_id=-1);
	
public:
	typedef MapEditor CLASSNAME;
	MapEditor();
	
	void Data();
	
	void MainMenu(Bar& bar);
	void ToolMenu(Bar& bar);
	void Open();
	void OpenMap(int world, int map);
	void Save();
	void SaveAs();
	void PlayMap();
	void Start();
	void SetTool(int i) {tool_id = i;}
};


#endif
