#include "RainGame.h"

#define IMAGECLASS RainGameImg
#define IMAGEFILE <RainGame/RainGame.iml>
#include <Draw/iml_source.h>




GUI_APP_MAIN
{
	bool run_editor = false;
	String map_path;
	
	auto& args = CommandLine();
	for(int i = 0; i < args.GetCount(); i++) {
		String arg = args[i];
		if (arg == "-e")
			run_editor = true;
		else
			map_path = arg;
	}
	if (run_editor)
		MapEditor().Run();
	else {
		Game g;
		if (!map_path.IsEmpty())
			g.LoadMap(map_path);
		g.Run();
	}
}
