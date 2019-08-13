#include "pch.h"
#include "main.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback
{
private:
	Map &map; // a map to dig
	int roomNum; // room number
	int lastx, lasty; // center of the last room

public:
	BspListener(Map &map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void *userData)
	{
		if (node->isLeaf())
		{
			int x, y, w, h;
			bool withActors = (bool)userData;
			// dig a room
			TCODRandom *rng = TCODRandom::getInstance();
			w = map.rng->getInt(ROOM_MIN_SIZE, node->w - 2);
			h = map.rng->getInt(ROOM_MIN_SIZE, node->h - 2);
			x = map.rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = map.rng->getInt(node->y + 1, node->y + node->h - h - 1);
			map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1, withActors);
			if (roomNum != 0)
			{
				//dig a corridor from last room
				map.dig(lastx, lasty, x + w / 2, lasty);
				map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
			}
			lastx = x + w / 2;
			lasty = y + h / 2;
			roomNum++;
		}
		return true;
	}
};

Map::Map()
{

}

Map::Map(int width, int height ) : width(width), height(height)
{
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}


Map::~Map()
{
	delete[] tiles;
	delete map;
}

void Map::init(bool withActors)
{
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);
	tiles = new Tile[width * height];
	map = new TCODMap(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MIN_SIZE,
		1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, (void*)withActors);
}

void Map::initFromFile(bool withActors, int width, int height, rapidjson::Value& a)
{
	tiles = new Tile[width * height];
	map = new TCODMap(width, height);
	int i = 0;
	for (int x = 0; x >= width; x++)
	{
		for (int y = 0; y >= height; y++)
		{
			map->setProperties(x, y, true, true);
		}
	}
}

void Map::save(TCODZip &zip) 
{
	zip.putInt(seed);
	for (int i = 0; i < width*height; i++) 
	{
		zip.putInt(tiles[i].explored);
	}
}

void Map::load(TCODZip &zip) 
{
	seed = zip.getInt();
	init(false);
	for (int i = 0; i < width*height; i++) 
	{
		tiles[i].explored = zip.getInt();
	}
}

bool Map::isWall(int x, int y) const
{
	return !map->isWalkable(x, y);
}

bool Map::isExplored(int x, int y) const
{
	return tiles[x + y * width].explored;
}

bool Map::isInFov(int x, int y) const
{
	if (x < 0 || x >= width || y < 0 || y >= height)
	{
		return false;
	}
	if (map->isInFov(x, y))
	{
		tiles[x + y * width].explored = true;
		return true;
	}
	return false;
}

bool Map::canWalk(int x, int y) const
{
	if (isWall(x, y))
	{
		// this is a wall
		return false;
	}
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if (actor->blocks && actor->x == x && actor->y == y)
		{
			// there is a blocking actor there. cannot walk
			return false;
		}
	}
	return true;
}

void Map::computeFov()
{
	map->computeFov(engine.player->x, engine.player->y,
		engine.fovRadius);
}

void Map::dig(int x1, int y1, int x2, int y2)
{
	if (x2 < x1)
	{
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y2 < y1)
	{
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}
	for (int tilex = x1; tilex <= x2; tilex++)
	{
		for (int tiley = y1; tiley <= y2; tiley++)
		{
			map->setProperties(tilex, tiley, true, true);
		}
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors)
{
	dig(x1, y1, x2, y2);
	if (!withActors)
	{
		return;
	}
	if (first)
	{
		// put the player in the first room
		engine.player->x = (x1 + x2) / 2;
		engine.player->y = (y1 + y2) / 2;
		if (engine.floor == 1)
		{
			itemListOne roomOne;
			engine.actors.push(roomOne.create(itemListOne::ROOM_ONE_FLOOR_ONE_SCROLL_OF_USELESS_GOODIES,
				engine.player->x - 1, engine.player->y - 1));
		}
		if (engine.floor == 1)
		{
			Actor *oldMan = new Actor(engine.player->x+2, engine.player->y-1, 'K', "Dying Knight-Errant",
				TCODColor::white);

			oldMan->blocks = true;
			oldMan->talkable = true;
			oldMan->talkableName = "Dying Knight-Errant";
			oldMan->talkableType = "Characters";
			engine.actors.push(oldMan);

			Actor *statue = new Actor(engine.player->x + 3, engine.player->y + 2, 'Y', "Statue of Lady Liana",
				TCODColor::orange);

			statue->blocks = true;
			statue->talkable = true;
			statue->talkableName = "Statue of Lady Liana";
			statue->talkableType = "Statues";
			engine.actors.push(statue);
		}
		// place stairs up if not on first level
		if (engine.floor > 1)
		{
			engine.stairsUp->x = (x1 + x2) / 2;
			engine.stairsUp->y = (y1 + y2) / 2;
		}
	}
	else
	{
		TCODRandom *rng = TCODRandom::getInstance();
		// add monsters
		int nbMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
		//int nbMonsters = rng->getInt(0, 1);
		if (nbMonsters > 0)
		{
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y))
			{
				addMonster(x, y);
			}
			nbMonsters--;
		}

		// add items
		int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
		if (nbItems > 0)
		{
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y) && engine.getActor(x, y) == NULL)
			{
				addItem(x, y);
			}
			nbItems--;
		}
		
		// set the stairs position
		// the stairs will be moved through every room
		// until the last, ensuring that the stairs
		// are as far from the player as possible
		engine.stairsDown->x = (x1 + x2) / 2;
		engine.stairsDown->y = (y1 + y2) / 2;
	}
}

void Map::addMonster(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	if (rng->getInt(0, 100) < randomChanceByFloor(40, engine.floor, 1, 10))
	{
		// create a thug
		Actor *gunner= new Actor(x, y, 'g', "Gunner",
			TCODColor::white);
		gunner->destructible = new MonsterDestructible(20 + (2 * engine.floor - 1), 0, 0,
			"A Gunner's Corpse", 35 + (5 * (engine.floor - 1)));
		gunner->attacker = new Attacker(0, 3 + (engine.floor - 1), true, 5);
		gunner->ai = new MonsterAi();
		gunner->attacker->updateDamageModifier();
		engine.actors.push(gunner);
	}
	if (rng->getInt(0, 100) < randomChanceByFloor(60, engine.floor, 1, 10))
	{
		// create an orc
		Actor *thug = new Actor(x, y, 't', "Thug",
			TCODColor::white);
		thug->destructible = new MonsterDestructible(20 + (2 * engine.floor - 1), 0, 0,
			"A Thug's Corpse", 35 + (5 * (engine.floor - 1)));
		thug->attacker = new Attacker(0, 3 + (engine.floor - 1));
		thug->ai = new MonsterAi();
		thug->attacker->updateDamageModifier();
		engine.actors.push(thug);
	}
	else if (rng->getInt(0, 100) < randomChanceByFloor(80, engine.floor, 1, 10)) 
	{
		// create an orc
		Actor *tweaker = new Actor(x, y, 'w', "Tweaker",
			TCODColor::green);
		tweaker->destructible = new MonsterDestructible(20+(engine.floor), 0, 0, 
			"A Tweaker's Corpse", 20 + (2 * (engine.floor - 1)));
		tweaker->attacker = new Attacker(0, 2 + (engine.floor - 1));
		tweaker->ai = new MonsterAi();
		tweaker->attacker->updateDamageModifier();
		engine.actors.push(tweaker);
	}
	else if (rng->getInt(0, 100) < randomChanceByFloor(20, engine.floor, 3, 10, true))
	{
		// create a troll
		Actor *cyborg = new Actor(x, y, 'C', "Cyborg",
			TCODColor::darkerYellow);
		cyborg->destructible = new MonsterDestructible(30 + (2 * engine.floor - 1), 0, 1, 
			"A Cyborg's Corpse", 100 + (5 * (engine.floor - 1)));
		cyborg->attacker = new Attacker(0, 4 + (engine.floor - 1));
		cyborg->ai = new MonsterAi();
		cyborg->attacker->updateDamageModifier();
		engine.actors.push(cyborg);
	}
}

void Map::addItem(int x, int y) 
{
	itemListOne items;
	TCODRandom *rng = TCODRandom::getInstance();
	int rngNum = rng->getInt(0, 100);
	if (rngNum < randomChanceByFloor(15, engine.floor, 3, 1))
	{
		TCODRandom *temprng = TCODRandom::getInstance();
		if (temprng->getInt(0, 100) < 50)
		{
			engine.actors.push(items.create(itemListOne::SCROLL_OF_LIGHTNING_BOLT, x, y));
			return;
		}
		else
		{
			engine.actors.push(items.create(itemListOne::SCROLL_OF_FIREBALL, x, y));
			return;
		}
	}

	else if (rngNum < randomChanceByFloor(10, engine.floor, 2, 1))
	{
		engine.actors.push(items.create(itemListOne::SCROLL_OF_CONFUSION, x, y));
		return;
	}

	else if (rngNum < randomChanceByFloor(15, engine.floor, 2, 1))
	{
		TCODRandom *temprng = TCODRandom::getInstance();
		if (temprng->getInt(0, 100) < 50)
		{
			engine.actors.push(items.create(itemListOne::RUSTY_IRON_SWORD, x, y));
			return;
		}

		else
		{
			engine.actors.push(items.create(itemListOne::CRACKED_WOODEN_SHIELD, x, y));
			return;
		}
	}

	else if (rngNum < randomChanceByFloor(15, engine.floor, 1, 1))
	{
		engine.actors.push(items.create(itemListOne::RIPPED_LEATHER_TUNIC, x, y));
		return;
	}

	else if (rngNum < randomChanceByFloor(80, engine.floor, 1, 1))
	{
		engine.actors.push(items.create(itemListOne::SMALL_HEALING_POTION, x, y));
		return;
	}
}

void Map::render() const
{
	
	static const TCODColor darkWall(69, 69, 69);
	static const TCODColor darkGround(30, 30, 30);
	static const TCODColor lightWall(130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);
	
	//static const int darkWall = 258;
	//static const int darkGround = 256;
	//static const int lightWall = 259;
	//static const int lightGround = 257;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (isInFov(x, y))
			{
				if (isWall(x, y))
				{
					TCODConsole::root->setCharBackground(x, y, lightWall);
				}
				else
				{
					TCODConsole::root->setCharBackground(x, y, TCODColor::black);
					TCODConsole::root->setCharForeground(x, y, lightGround);
					TCODConsole::root->setChar(x, y, '.');
				}
			}
			else if (isExplored(x,y))
			{
				if (isWall(x, y))
				{
					TCODConsole::root->setCharBackground(x, y, darkWall);
				}
				else
				{
					TCODConsole::root->setCharBackground(x, y, TCODColor::black);
					TCODConsole::root->setCharForeground(x, y, darkGround);
					TCODConsole::root->setChar(x, y, '.');
				}
			}
		}
	}
}


