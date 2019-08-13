#include "pch.h"
#include "main.h"

Engine engine(90, 60);

int main()
{
	engine.load();
	while (!TCODConsole::isWindowClosed())
	{
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	engine.save();
	return 0;
}