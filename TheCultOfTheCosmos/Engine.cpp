#include "pch.h"
#include "main.h"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10),
screenWidth(screenWidth), screenHeight(screenHeight), floor(1)
{
	TCODConsole::initRoot(screenWidth, screenHeight, "The Cult of the Cosmos", false, TCOD_RENDERER_OPENGL2);
	//TCODConsole::setCustomFont("terminal.png", TCOD_FONT_LAYOUT_ASCII_INROW, 0, 0);
	TCODConsole::setCustomFont("tileset1.png", TCOD_FONT_LAYOUT_ASCII_INROW | TCOD_FONT_TYPE_GREYSCALE, 16, 18);
	gui = new Gui();
}

Engine::~Engine()
{
	term();
	delete gui;
}

void Engine::nextFloor()
{
	save();
	floor++;
	if (deepestFloor < floor)
	{
		deepestFloor = floor;
		gui->message(TCODColor::lightViolet, "You take a moment to rest.");
		player->destructible->heal(player->destructible->maxHp / 2);
		gui->message(TCODColor::red, "After a rare moment of peace you descend\ndeeper into the dungeon...");
		delete map;
		// delete all actors but player and stairs
		for (Actor **it = actors.begin(); it != actors.end(); it++)
		{
			if (*it != player && *it != stairsDown && *it != stairsUp)
			{
				delete *it;
				it = actors.remove(it);
			}
		}
		// create a new map
		map = new Map(80, 45);
		map->init(true);
		gameStatus = STARTUP;
	}
	else
	{
		delete map;
		// delete all actors but player and stairs
		for (Actor **it = actors.begin(); it != actors.end(); it++)
		{
			delete *it;
			it = actors.remove(it);
		}
		prepareFloorLoading();
	}
}

void Engine::previousFloor()
{
	save();
	floor--;
	delete map;
	// delete all actors but player and stairs
	for (Actor **it = actors.begin(); it != actors.end(); it++)
	{
		delete *it;
		it = actors.remove(it);
	}
	prepareFloorLoading();
}

void Engine::term()
{
	actors.clear();
	if (map) delete map;
	gui->clear();
}

void Engine::init()
{
	player = new Actor(5, 5, '@', "Player", TCODColor::white);
	player->destructible = new PlayerDestructible(97, 10, 1, 1, 1, 1, 0, "Your Corpse", 0);
	characterGeneration();
	player->destructible->updateHpModifier();
	player->destructible->hp = player->destructible->maxHp;
	player->destructible->updateMpModifier();
	player->destructible->mp = player->destructible->maxMp;
	player->attacker = new Attacker(0, 4);
	player->attacker->updateDamageModifier();
	player->ai = new PlayerAi();
	player->container = new Container(26);
	player->equipment = new Equipment();
	actors.push(player);
	stairsDown = new Actor(0, 0, 261, "Stairs Down", TCODColor::white);
	stairsDown->blocks = false;
	stairsDown->fovOnly = false;
	actors.push(stairsDown);
	stairsUp = new Actor(89, 59, 260, "Stairs Up", TCODColor::white);
	stairsUp->blocks = false;
	stairsUp->fovOnly = false;
	actors.push(stairsUp);
	map = new Map(80, 45);
	//map = new Map();
	map->init(true);
	//loadMapFile();
	gui->message(TCODColor::green,
		"Welcome adventurer!\nThe Cult of the Cosmos!");
	gameStatus = STARTUP;
}

void Engine::save()
{
	std::string fileName = "save\\floor" + std::to_string(floor) + ".sav";
	if (player->destructible->isDead())
	{
		clearSavedFloors();
	}
	else
	{
		TCODZip zip;
		// save the map first
		zip.putInt(map->width);
		zip.putInt(map->height);
		map->save(zip);
		// save the player
		player->save(zip);
		// save the stairs
		stairsDown->save(zip);
		stairsUp->save(zip);
		// save all other actors
		zip.putInt(actors.size() - 3);
		for (Actor **it = actors.begin(); it != actors.end(); it++)
		{
			if (*it != player && *it != stairsDown && *it != stairsUp)
			{
				(*it)->save(zip);
			}
		}
		// save the message log
		gui->save(zip);
		
		zip.saveToFile(fileName.c_str());
	}
}

void Engine::characterGeneration()
{
	bool confirmed = false;
	TCOD_key_t key;
	TCODConsole::root->clear();

	std::vector<std::string> dialogueSet;
	std::string name = "";
	std::string line = "\"Oh my! Another one...\"";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);

	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();

	line = "\"Well, let me get a good look at you.\"";
	dialogueSet.push_back(line);
	line = " ";
	dialogueSet.push_back(line);
	line = "Your entire body aches.";
	dialogueSet.push_back(line);
	line = "You feel cold stone beneath you body.";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);

	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();

	line = "\"Well go on then!\"";
	dialogueSet.push_back(line);
	line = "\"Up on your feet.\"";
	dialogueSet.push_back(line);
	line = " ";
	dialogueSet.push_back(line);
	line = "You see an ancient looking old man before you.";
	dialogueSet.push_back(line);
	line = "He has snow white hair and a flowing beard.";
	dialogueSet.push_back(line);
	line = "He wears a bright red robe and large pointed hat.";
	dialogueSet.push_back(line);
	line = " ";
	dialogueSet.push_back(line);
	line = "He points to a chair.";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);


	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();
	line = "\"Sit and we can get started!\"";
	dialogueSet.push_back(line);
	line = " ";
	dialogueSet.push_back(line);
	line = "He hobbles over to another chair and snaps his fingers.";
	dialogueSet.push_back(line);
	line = "A table appears between the chairs.";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);


	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();
	line = "As you sit down the old man clatters into his own chair.";
	dialogueSet.push_back(line);
	line = "He snaps his fingers again and an immense tome appears on the table.";
	dialogueSet.push_back(line);
	line = " ";
	dialogueSet.push_back(line);
	line = "\"So then my friend, time for the paperwork!\"";
	dialogueSet.push_back(line);
	line = " ";
	dialogueSet.push_back(line);
	line = "Confused you nod at him to continue.";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);



	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}

	// wait for a key press
	while (true)
	{
		TCODConsole::root->clear();
		line = "\"So...\", he thumbs his way through the book.";
		dialogueSet.push_back(line);
		line = "\"Tell me, would you prefer to be known as (M)ale or (F)emale?\"";
		dialogueSet.push_back(line);
		line = " ";
		dialogueSet.push_back(line);
		line = "Enter (m) for Male or (f) for Female.";
		dialogueSet.push_back(line);



		gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
		dialogueSet.clear();
		dialogueSet.resize(0);

		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.c == 'm' or key.c == 'M')
		{
			TCODConsole::root->clear();
			line = "\"You have chosen to be known as a Male.\"";
			dialogueSet.push_back(line);
			line = "Are you sure?";
			dialogueSet.push_back(line);
			line = "Press (y) for yes or (n) for no.";
			dialogueSet.push_back(line);


			gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
			dialogueSet.clear();
			dialogueSet.resize(0);

			// wait for a key press
			while (true)
			{
				TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
				if (key.c == 'y' || key.c == 'Y')
				{
					player->destructible->sex = "Male";
					confirmed = true;
					break;
				}
				else if(key.c == 'n' || key.c == 'N')
				{
					break;
				}
			}
		}
		else if (key.c == 'f' or key.c == 'F')
		{
			TCODConsole::root->clear();
			line = "\"You have chosen to be known as a Female.\"";
			dialogueSet.push_back(line);
			line = "Are you sure?";
			dialogueSet.push_back(line);
			line = "Press (y) for yes or (n) for no.";
			dialogueSet.push_back(line);


			gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
			dialogueSet.clear();
			dialogueSet.resize(0);

			// wait for a key press
			while (true)
			{
				TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
				if (key.c == 'y' || key.c == 'Y')
				{
					player->destructible->sex = "Female";
					confirmed = true;
					break;
				}
				else if (key.c == 'n' || key.c == 'N')
				{
					break;
				}
			}
		}
		if (confirmed)
		{
			confirmed = false;
			break;
		}
	}

	while (true)
	{
		TCODConsole::root->clear();
		line = "\"Very good.\", he marks your sex and turns the page.";
		dialogueSet.push_back(line);
		line = "\"Now for your race. Would you describe yourself as a (H)uman or\"";
		dialogueSet.push_back(line);
		line = "\"as an (E)lf?\"";
		dialogueSet.push_back(line);
		line = " ";
		dialogueSet.push_back(line);
		line = "Enter (h) for Human or (e) for Elf.";
		dialogueSet.push_back(line);


		gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
		dialogueSet.clear();
		dialogueSet.resize(0);

		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.c == 'h' or key.c == 'H')
		{
			TCODConsole::root->clear();
			line = "\"You have chosen to be known as a Human.\"";
			dialogueSet.push_back(line);
			line = "Are you sure?";
			dialogueSet.push_back(line);
			line = "Press (y) for yes or (n) for no.";
			dialogueSet.push_back(line);

			gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
			dialogueSet.clear();
			dialogueSet.resize(0);

			// wait for a key press
			while (true)
			{
				TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
				if (key.c == 'y' || key.c == 'Y')
				{
					player->destructible->race = "Human";
					confirmed = true;
					break;
				}
				else if (key.c == 'n' || key.c == 'N')
				{
					break;
				}
			}
		}
		else if (key.c == 'e' or key.c == 'E')
		{
			TCODConsole::root->clear();
			line = "\"You have chosen to be known as an Elf.\"";
			dialogueSet.push_back(line);
			line = "Are you sure?";
			dialogueSet.push_back(line);
			line = "Press (y) for yes or (n) for no.";
			dialogueSet.push_back(line);


			gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
			dialogueSet.clear();
			dialogueSet.resize(0);

			// wait for a key press
			while (true)
			{
				TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
				if (key.c == 'y' || key.c == 'Y')
				{
					player->destructible->race = "Elf";
					confirmed = true;
					break;
				}
				else if (key.c == 'n' || key.c == 'N')
				{
					break;
				}
			}
		}
		if (confirmed)
		{
			confirmed = false;
			break;
		}
	}

	while (true)
	{
		TCODConsole::root->clear();
		line = "\"Interesting...\", he marks your race and turns the page.";
		dialogueSet.push_back(line);
		line = "\"Now for your class. Do you prefer the savage thrill of melee combat like a (W)arrior?\"";
		dialogueSet.push_back(line);
		line = " ";
		dialogueSet.push_back(line);
		line = "\"Or perhaps you prefer to go at it from afar like an (A)rcher would?\"";
		dialogueSet.push_back(line);
		line = " ";
		dialogueSet.push_back(line);
		line = "Enter (w) for Warrior or (a) for Archer.";
		dialogueSet.push_back(line);


		gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
		dialogueSet.clear();
		dialogueSet.resize(0);

		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.c == 'w' or key.c == 'W')
		{
			TCODConsole::root->clear();
			line = "\"You have chosen the fighting style of the warrior.\"";
			dialogueSet.push_back(line);
			line = "Are you sure?";
			dialogueSet.push_back(line);
			line = "Press (y) for yes or (n) for no.";
			dialogueSet.push_back(line);

			gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
			dialogueSet.clear();
			dialogueSet.resize(0);

			// wait for a key press
			while (true)
			{
				TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
				if (key.c == 'y' || key.c == 'Y')
				{
					player->destructible->className = "Warrior";
					confirmed = true;
					break;
				}
				else if (key.c == 'n' || key.c == 'N')
				{
					break;
				}
			}
		}
		else if (key.c == 'a' or key.c == 'A')
		{
			TCODConsole::root->clear();
			line = "\"You have chosen the fighting style of the archer.\"";
			dialogueSet.push_back(line);
			line = "Are you sure?";
			dialogueSet.push_back(line);
			line = "Press (y) for yes or (n) for no.";
			dialogueSet.push_back(line);


			gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
			dialogueSet.clear();
			dialogueSet.resize(0);

			// wait for a key press
			while (true)
			{
				TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
				if (key.c == 'y' || key.c == 'Y')
				{
					player->destructible->className = "Archer";
					confirmed = true;
					break;
				}
				else if (key.c == 'n' || key.c == 'N')
				{
					break;
				}
			}
		}
		if (confirmed)
		{
			confirmed = false;
			break;
		}
	}
	while (true)
	{
		TCODConsole::root->clear();
		line = "\"Not what I would have chosen...\", he marks your class and turns the page.";
		dialogueSet.push_back(line);
		line = "\"Finally, what is your name child?\"";
		dialogueSet.push_back(line);
		line = "Press (enter) to continue.";
		dialogueSet.push_back(line);


		name = gui->getCharacterName(dialogueSet, "Mysterious Old Man");
		dialogueSet.clear();
		dialogueSet.resize(0);

		TCODConsole::root->clear();
		line = "\"Your name is " + name + ".\"";
		dialogueSet.push_back(line);
		line = "Is this correct?";
		dialogueSet.push_back(line);
		line = "Press (y) for yes or (n) for no.";
		dialogueSet.push_back(line);

		gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
		dialogueSet.clear();
		dialogueSet.resize(0);

		// wait for a key press
		while (true)
		{
			TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
			if (key.c == 'y' || key.c == 'Y')
			{
				player->name = name;
				confirmed = true;
				break;
			}
			else if (key.c == 'n' || key.c == 'N')
			{
				break;
			}
		}
		if (confirmed)
		{
			confirmed = false;
			break;
		}
	}
	TCODConsole::root->clear();

	line = "\"Very well.\" He writes your name down in the tome.";
	dialogueSet.push_back(line);
	line = "";
	dialogueSet.push_back(line);
	line = "As he finishes writing you feel an odd";
	dialogueSet.push_back(line);
	line = "tightening settle over your skin.";
	dialogueSet.push_back(line);
	line = "";
	dialogueSet.push_back(line);
	line = "\"The agreement is made my young friend.\"";
	dialogueSet.push_back(line);
	line = "\"I wish you luck on your journey.\"";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);


	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();

	line = "\"Many before you have tried.\"";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);

	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();


	line = "\"Many before you have failed.\"";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);

	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();


	line = "The old man draws an unintelligible symbol";
	dialogueSet.push_back(line);
	line = "before your eyes.";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);


	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
	TCODConsole::root->clear();


	line = "Light consumes you.";
	dialogueSet.push_back(line);
	line = "Press (space) to continue.";
	dialogueSet.push_back(line);


	gui->drawGenericSizedWindow(dialogueSet, "Mysterious Old Man");
	dialogueSet.clear();
	dialogueSet.resize(0);

	// wait for a key press
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_SPACE)
		{
			break;
		}
	}
}

void Engine::showControls()
{
	TCOD_key_t key;
	TCODConsole::root->clear();
	std::vector<std::string> controls;
	controls.push_back("w: Move North");
	controls.push_back("x: Move South");
	controls.push_back("a: Move West");
	controls.push_back("d: Move East");
	controls.push_back("q: Move Northwest");
	controls.push_back("e: Move Northeast");
	controls.push_back("z: Move Southwest");
	controls.push_back("c: Move Southeast");
	controls.push_back("g: Pick up item");
	controls.push_back("r: Rest");
	controls.push_back("s: Wait");
	controls.push_back("f: Interact with NPC");
	controls.push_back("v: Use Stairs");
	controls.push_back("i: Open Inventory");
	controls.push_back("o: Open Character Sheet");
	controls.push_back("t: Drop item from inventory");
	controls.push_back("Press (escape) to return to Main Menu");
	gui->drawGenericSizedWindow(controls, "Controls");
	while (true)
	{
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		if (key.vk == TCODK_ESCAPE)
		{
			break;
		}
	}
	controls.clear();
	controls.resize(0);
	TCODConsole::root->clear();
}

void Engine::clearSavedFloors()
{
	std::string fileName;
	for (int i = 1; i <= 0xFFFF; i++)
	{
		fileName = "save\\floor" + std::to_string(i) + ".sav";

		if (TCODSystem::fileExists(fileName.c_str()))
		{
			TCODSystem::deleteFile(fileName.c_str());
		}
		else
		{
			return;
		}
	}
}

void Engine::getLowestFloor()
{
	std::string fileName;
	while (true)
	{
		fileName = "save\\floor" + std::to_string(floor) + ".sav";

		if (TCODSystem::fileExists(fileName.c_str()))
		{
			floor += 1;
		}
		else
		{
			floor -= 1;
			deepestFloor = floor;
			break;
		}
	}
}

void Engine::prepareFloorLoading()
{
	TCODZip zip;
	std::string fileName;
	fileName = "save\\floor" + std::to_string(floor) + ".sav";
	// continue a saved game
	//engine.term();
	zip.loadFromFile(fileName.c_str());
	// load the map
	int width = zip.getInt();
	int height = zip.getInt();
	map = new Map(width, height);
	map->load(zip);

	// load the player
	player = new Actor(0, 0, 0, "", TCODColor::white);
	player->load(zip);
	actors.push(player);
	// load the stairs
	stairsDown = new Actor(0, 0, 0, "", TCODColor::white);
	stairsDown->load(zip);
	actors.push(stairsDown);
	stairsUp = new Actor(0, 0, 0, "", TCODColor::white);
	stairsUp->load(zip);
	actors.push(stairsUp);

	// then all other actors
	int nbActors = zip.getInt();
	while (nbActors > 0)
	{
		Actor *actor = new Actor(0, 0, 0, "", TCODColor::white);
		actor->load(zip);
		actors.push(actor);
		nbActors--;
	}

	// finally the message log
	gui->load(zip);
	// to force FOV recomputation
	gameStatus = STARTUP;
}

void Engine::load()
{
	while (true)
	{
		gui->menu.clear();
		TCODConsole::root->setDefaultBackground(TCODColor::black);
		TCODConsole::root->setDefaultForeground(TCODColor::gold);
		TCODConsole::root->printf(screenWidth / 2 - 8, screenHeight / 5, "The Cult of the Cosmos");
		TCODConsole::root->printf(20, screenHeight / 5 + 3, "This game is UNBALANCED, UNSTABLE, and UNFORGIVING.");
		TCODConsole::root->printf(36, screenHeight / 5 + 4, "Play at your own peril.");
		TCODConsole::root->printf(TCOD_CENTER + screenWidth / 3 - 2, screenHeight - 2, "Author: Jake Queen   Pre-Alpha v0.1");
		gui->menu.addItem(Menu::NEW_GAME, "New Game");
		if (TCODSystem::fileExists("save\\floor1.sav"))
		{
			gui->menu.addItem(Menu::CONTINUE, "Continue");
		}
		gui->menu.addItem(Menu::CONTROLS, "Controls");
		gui->menu.addItem(Menu::EXIT, "Exit");
		Menu::MenuItemCode menuItem = gui->menu.pick();
		if (menuItem == Menu::EXIT || menuItem == Menu::NONE)
		{
			// Exit or window closed
			exit(0);
		}
		else if (menuItem == Menu::NEW_GAME)
		{
			// New game
			clearSavedFloors();
			term();
			init();
			break;
		}
		else if (menuItem == Menu::CONTROLS)
		{
			showControls();
			continue;
		}
		else
		{
			term();
			getLowestFloor();
			prepareFloorLoading();
			break;
		}
	}
}

Actor *Engine::getClosestMonster(int x, int y, float range) const
{
	Actor *closest = NULL;
	float bestDistance = 1E6F;

	for (Actor **iterator = actors.begin();
		iterator != actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if (actor != player && actor->destructible
			&& !actor->destructible->isDead())
		{
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range || range == 0.0f))
			{
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

Actor *Engine::getClosestTalkable(int x, int y, float range) const
{
	Actor *closest = NULL;
	float bestDistance = 1E6F;

	for (Actor **it = actors.begin();
		it != actors.end(); it++)
	{
		Actor *actor = *it;
		if (actor != player && actor->talkable)
		{
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range || range == 0.0f))
			{
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

Actor *Engine::getActor(int x, int y) const
{
	for (Actor **iterator = actors.begin();
		iterator != actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if (actor->x == x && actor->y == y && actor->destructible
			&& !actor->destructible->isDead())
		{
			return actor;
		}
	}
	return NULL;
}

bool Engine::pickATile(int *x, int *y, float maxRange, int sizeOfEffect)
{
	while (!TCODConsole::isWindowClosed())
	{
		render();
		// highlight the possible range
		for (int cx = 0; cx < map->width; cx++)
		{
			for (int cy = 0; cy < map->height; cy++)
			{
				if (map->isInFov(cx, cy)
					&& (maxRange == 0 || player->getDistance(cx, cy) <= maxRange))
				{
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.4f;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
		if (map->isInFov(mouse.cx, mouse.cy)
			&& (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange))
		{
			for (int i = -sizeOfEffect; i < sizeOfEffect+1; i++)
			{
				for (int j = -sizeOfEffect; j < sizeOfEffect+1; j++)
				{
					TCODConsole::root->setCharBackground(mouse.cx+i, mouse.cy+j, TCODColor::lightestRed);
				}
			}
			if (mouse.lbutton_pressed)
			{
				*x = mouse.cx;
				*y = mouse.cy;
				return true;
			}
		}
		if (mouse.rbutton_pressed)
		{
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}

void Engine::update() 
{
	if (gameStatus == STARTUP) map->computeFov();
	gameStatus = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
	if (lastKey.vk == TCODK_ESCAPE)
	{
		TCODConsole::root->clear();
		TCODConsole::flush();
		save();
		load();
	}
	player->update();
	if (gameStatus == NEW_TURN) 
	{
		for (Actor **iterator = actors.begin(); iterator != actors.end();
			iterator++) 
		{
			Actor *actor = *iterator;
			if (actor != player) 
			{
				actor->update();
			}
		}
	}
}

void Engine::sendToBack(Actor *actor)
{
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}

void Engine::render()
{
	TCODConsole::root->clear();
	// draw the map
	map->render();
	// draw the actors
	for (Actor **iterator = actors.begin();
		iterator != actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if (actor != player && (((actor == stairsDown || actor == stairsUp) && map->isExplored(actor->x, actor->y)) 
			|| map->isInFov(actor->x, actor->y)))
		{
			actor->render();
		}
	}
	player->render();
	gui->render();
}