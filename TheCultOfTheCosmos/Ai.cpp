#include "pch.h"
#include "main.h"


// how many turns the monster chases the player
// after losing his sight
static const int TRACKING_TURNS = 3;


Ai *Ai::create(TCODZip &zip) 
{
	AiType type = (AiType)zip.getInt();
	Ai *a = NULL;
	switch (type)
	{
		case (PLAYER): 
		{
			a = new PlayerAi();
			a->load(zip);
		}
		break;
		case (MONSTER):
		{
			a = new MonsterAi();
			a->load(zip);
		}
		break;
	}
	return a;
}


TemporaryAi::TemporaryAi(int nbTurns) : nbTurns(nbTurns)
{
}

void TemporaryAi::update(Actor *owner, bool playerMoving)
{
	nbTurns--;
	if (nbTurns == 0)
	{
		owner->ai = oldAi;
		delete this;
	}
}

void TemporaryAi::applyTo(Actor *actor)
{
	oldAi = actor->ai;
	actor->ai = this;
}

TemporaryAi * TemporaryAi::create(TCODZip & zip)
{
	TemporaryAi *a = new TemporaryAi(0);
	a->load(zip);
	return a;
}

void TemporaryAi::load(TCODZip & zip)
{
	nbTurns = zip.getInt();
}

void TemporaryAi::save(TCODZip & zip)
{
	zip.putInt(nbTurns);
}




PlayerAi::PlayerAi() : xpLevel(1)
{
}

const int LEVEL_UP_BASE = 200;
const int LEVEL_UP_FACTOR = 150;

int PlayerAi::getNextLevelXp()
{
	return LEVEL_UP_BASE + (xpLevel-1) * LEVEL_UP_FACTOR;
}

void PlayerAi::load(TCODZip &zip) 
{
	xpLevel = zip.getInt();
}

void PlayerAi::save(TCODZip &zip) 
{
	zip.putInt(PLAYER);
	zip.putInt(xpLevel);
}

void PlayerAi::update(Actor *owner, bool playerMoving)
{
	int levelUpXp = getNextLevelXp();
	if (owner->destructible->xp >= levelUpXp)
	{
		xpLevel++;
		owner->destructible->xp -= levelUpXp;
		engine.gui->message(TCODColor::yellow, "You feel that your skills have grown.\nYou have attained level %d!", xpLevel);
		engine.gui->menu.clear();
		engine.gui->menu.addItem(Menu::CONSTITUTION, "Constitution (+1)");
		engine.gui->menu.addItem(Menu::STRENGTH, "Strength (+1)");
		engine.gui->menu.addItem(Menu::DEXTERITY, "Dexterity (+1)");
		Menu::MenuItemCode menuItem = engine.gui->menu.pick("Level Up", Menu::PAUSE);

		switch (menuItem)
		{
			case Menu::CONSTITUTION :
			{
				owner->destructible->constitution += 1;
				break;
			}
			case Menu::STRENGTH :
			{
				owner->destructible->strength += 1;
				break;
			}
			case Menu::DEXTERITY :
			{
				owner->destructible->dexterity += 1;
				break;
			}
			default: break;
		}
		owner->attacker->updateDamageModifier();
		owner->destructible->updateHpModifier();
		owner->destructible->updateMpModifier();
	}
	if (owner->destructible && owner->destructible->isDead())
	{
		return;
	}
	int dx = 0, dy = 0;
	switch (engine.lastKey.vk)
	{
	case TCODK_UP: dy = -1; break;
	case TCODK_DOWN: dy = 1; break;
	case TCODK_LEFT: dx = -1; break;
	case TCODK_RIGHT: dx = 1; break;
	case TCODK_CHAR: 
	{
		switch (engine.lastKey.c)
		{
		case 'w': dy = -1; break;
		case 'x': dy = 1; break;
		case 'a': dx = -1; break;
		case 'd': dx = 1; break;
		case 'q':
		{
			dy = -1;
			dx = -1;
			break;
		}
		case 'e':
		{
			dy = -1;
			dx = 1;
			break;
		}
		case 'z':
		{
			dy = 1;
			dx = -1;
			break;
		}
		case 'c':
		{
			dy = 1;
			dx = 1;
			break;
		}
		}
		handleActionKey(owner, engine.lastKey.c); break;
	}
	default: break;
	}
	if (dx != 0 || dy != 0)
	{
		engine.gameStatus = engine.NEW_TURN;
		if (moveOrAttack(owner, owner->x+dx, owner->y+dy))
		{
			if (engine.player->destructible->turnsUntilRest > 0)
			{
				engine.player->destructible->turnsUntilRest -= 1;
			}
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	if (engine.map->isWall(targetx, targety)) return false;
	// look for living actors to attack
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead()
			&& actor->x == targetx && actor->y == targety) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// look for talkable actors
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if (actor->talkable && actor->x == targetx && actor->y == targety)
		{
			return false;
		}
	}

	// look for corpses or items
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead())
			|| actor->pickable;
		if (corpseOrItem && actor->x == targetx && actor->y == targety) 
		{
			engine.gui->message(TCODColor::lightGrey, "There's a(n) %s here.", actor->name.c_str());
		}
	}
	owner->x = targetx;
	owner->y = targety;
	return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii)
{
	switch (ascii)
	{
	case 's': // wait
	{
		engine.gameStatus = Engine::NEW_TURN;
	}
	break;

	case 'r': // rest
	{
		bool enemyInSight = false;
		for (Actor **iterator = engine.actors.begin();
			iterator != engine.actors.end(); iterator++)
		{
			Actor *actor = *iterator;
			if (actor != engine.player && actor->destructible && !actor->destructible->isDead())
			{
				if (engine.map->isInFov(actor->x, actor->y))
				{
					engine.gui->message(TCODColor::lightOrange, "You cannot rest with enemies near.");
					enemyInSight = true;
					break;
				}
				actor->ai->update(actor, true);
			}
		}
		if (enemyInSight)
		{
			break;
		}
		if (engine.player->destructible->turnsUntilRest != 0)
		{
			engine.gui->message(TCODColor::lightOrange, "The stress of recent battle prevents");
			engine.gui->message(TCODColor::lightOrange, "you from adequately resting.");
		}
		else
		{
			if (engine.player->destructible->hp < 0.6f * engine.player->destructible->maxHp)
			{
				TCODRandom *rngHp = TCODRandom::getInstance();
				int numHp = rngHp->getInt(0, 10);
				engine.player->destructible->hp += numHp;
				if (engine.player->destructible->hp > 0.6f * engine.player->destructible->maxHp)
				{
					engine.player->destructible->hp = 0.6f * engine.player->destructible->maxHp;
				}
				engine.gui->message(TCODColor::lightGreen, "You take a moment to rest.");
			}
			else
			{
				engine.gui->message(TCODColor::lightOrange, "Even resting can only help you so much.");
			}
		}
		engine.gameStatus = Engine::NEW_TURN;
	}
	break;

	case 'g': // pickup item
	{
		bool found = false;
		for (Actor **iterator = engine.actors.begin();
			iterator != engine.actors.end(); iterator++)
		{
			Actor *actor = *iterator;
			if (actor->pickable && actor->x == owner->x && actor->y == owner->y)
			{
				if (actor->pickable->pick(actor, owner))
				{
					found = true;
					engine.gui->message(TCODColor::lightGrey, "You pick up the %s.",
						actor->name.c_str());
					break;
				}
				else if (!found)
				{
					found = true;
					engine.gui->message(TCODColor::red, "Your inventory is full.");
				}
			}
		}
		if (!found)
		{
			engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick up.");
		}
		engine.gameStatus = Engine::NEW_TURN;
	}
	break;
	case 'i': // display inventory
	{
		while (true)
		{
			Actor *actor = choseFromInventory(owner);
			if (actor == NULL)
			{
				break;
			}
			if (actor)
			{
				if (actor->pickable->equippable && !actor->pickable->equipped)
				{
					actor->pickable->equipEquippable(actor, owner);
					engine.gameStatus = Engine::NEW_TURN;
					break;
				}
				else if (actor->pickable->equippable && actor->pickable->equipped)
				{
					actor->pickable->unEquipEquippable(actor, owner);
					engine.gameStatus = Engine::NEW_TURN;
					break;
				}
				else if (actor->pickable->usable)
				{
					actor->pickable->use(actor, owner);
					engine.gameStatus = Engine::NEW_TURN;
					break;
				}
				else
				{
					TCOD_key_t key;
					engine.gui->drawGenericSizedWindow(actor->pickable->itemDescription, actor->name.c_str());
					while (true)
					{
						key = TCODConsole::waitForKeypress(true);
						if (key.vk == TCODK_SPACE)
						{
							break;
						}
					}
				}
			}
			engine.render();
		}
	}
	break;
	case 'o': // display character information
	{
		displayCharacterInformation();
		engine.gameStatus = Engine::NEW_TURN;
	}
	break;
	case 't': // drop item 
	{
		Actor *actor = choseFromInventory(owner);
		if (actor) 
		{
			if (actor->pickable->equippable && actor->pickable->equipped)
			{
				actor->pickable->unEquipEquippable(actor, owner);
			}
			actor->pickable->drop(actor, owner);
			engine.gameStatus = Engine::NEW_TURN;
		}
	}
	break;
	case 'v':
	{
		if (engine.stairsDown->x == owner->x && engine.stairsDown->y == owner->y)
		{
			engine.nextFloor();
		}
		else if (engine.stairsUp->x == owner->x && engine.stairsUp->y == owner->y)
		{
			engine.previousFloor();
		}
		else
		{
			engine.gui->message(TCODColor::lightGrey, "There are no stairs here.");
		}
	}
	break;
	case 'f':
	{
		Actor *closestTalkable = engine.getClosestTalkable(owner->x, owner->y, 1.5f);
		if (closestTalkable)
		{
			engine.gui->drawGenericSizeWindowWithTalkOptions(closestTalkable);
			engine.gameStatus = Engine::NEW_TURN;
		}
	}
	break;
	}
}

void PlayerAi::displayCharacterInformation()
{
	static const int CHARACTER_SHEET_WIDTH = 50;
	static const int CHARACTER_SHEET_HEIGHT = 28;
	static TCODConsole con(CHARACTER_SHEET_WIDTH, CHARACTER_SHEET_HEIGHT);

	// display the character sheet frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, CHARACTER_SHEET_WIDTH, CHARACTER_SHEET_HEIGHT, true,
		TCOD_BKGND_DEFAULT, "Character Sheet");

	// display character stats
	con.setDefaultForeground(TCODColor::white);
	con.printf(2, 1, "Level: %d", int(xpLevel));
	con.printf(2, 2, "Name: %s", engine.player->name.c_str());
	con.printf(2, 3, "Race: %s", engine.player->destructible->race);
	con.printf(2, 4, "Class: %s", engine.player->destructible->className);
	con.printf(2, 5, "HP: %d/%d", int(engine.player->destructible->hp), int(engine.player->destructible->maxHp));
	con.printf(2, 6, "MP: %d/%d");
	con.printf(2, 7, "XP: %d/%d", engine.player->destructible->xp, getNextLevelXp());
	con.printf(2, 8, "Damage: %d/%d", int(engine.player->attacker->minDamage), int(engine.player->attacker->maxDamage));
	con.printf(2, 9, "Armor: %d", int(engine.player->destructible->armor));
	con.printf(2, 10, "Strength: %d", int(engine.player->destructible->strength));
	con.printf(2, 11, "Dexterity: %d", int(engine.player->destructible->dexterity));
	con.printf(2, 12, "Constitution: %d", int(engine.player->destructible->constitution));
	con.printf(2, 13, "Intelligence: %d", int(engine.player->destructible->intelligence));

	if (engine.player->equipment->mainHand) con.printf(20, 1, "Main Hand: %s", engine.player->equipment->mainHand->name.c_str());
	else con.printf(20, 1, "Main Hand: Empty");
	if (engine.player->equipment->offHand) con.printf(20, 2, "Off Hand: %s", engine.player->equipment->offHand->name.c_str());
	else con.printf(20, 2, "Off Hand: Empty");
	if (engine.player->equipment->head) con.printf(20, 3, "Head: %s", engine.player->equipment->head->name.c_str());
	else con.printf(20, 3, "Head: Empty");
	if (engine.player->equipment->shoulders) con.printf(20, 4, "Shoulders: %s", engine.player->equipment->shoulders->name.c_str());
	else con.printf(20, 4, "Shoulders: Empty");
	if (engine.player->equipment->chest) con.printf(20, 5, "Chest: %s", engine.player->equipment->chest->name.c_str());
	else con.printf(20, 5, "Chest: Empty");
	if (engine.player->equipment->hands) con.printf(20, 6, "Hands: %s", engine.player->equipment->hands->name.c_str());
	else con.printf(20, 6, "Hands: Empty");
	if (engine.player->equipment->legs) con.printf(20, 7, "Legs: %s", engine.player->equipment->legs->name.c_str());
	else con.printf(20, 7, "Legs: Empty");
	if (engine.player->equipment->feet) con.printf(20, 8, "Feet: %s", engine.player->equipment->feet->name.c_str());
	else con.printf(20, 8, "Feet: Empty");
	if (engine.player->equipment->back) con.printf(20, 9, "Back: %s", engine.player->equipment->back->name.c_str());
	else con.printf(20, 9, "Back: Empty");

	// blit the character sheet console on the root console
	TCODConsole::blit(&con, 0, 0, CHARACTER_SHEET_WIDTH, CHARACTER_SHEET_HEIGHT,
		TCODConsole::root, engine.screenWidth / 2 - CHARACTER_SHEET_WIDTH / 2,
		engine.screenHeight / 2 - CHARACTER_SHEET_HEIGHT / 2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_ESCAPE)
	{
		return;
	}
}

Actor *PlayerAi::choseFromInventory(Actor *owner)
{
	static const int INVENTORY_WIDTH = 55;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	// display the inventory frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true,
		TCOD_BKGND_DEFAULT, "Inventory");

	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor **it = owner->container->inventory.begin();
		it != owner->container->inventory.end(); it++)
	{
		Actor *actor = *it;
		if (actor->pickable->equipped)
		{
			con.printf(2, y, "(%c) %s (e)", shortcut, actor->name.c_str());
		}
		else
		{
			con.printf(2, y, "(%c) %s", shortcut, actor->name.c_str());
		}
		y++;
		shortcut++;
	}
	con.printf(1, INVENTORY_HEIGHT - 3, "Press (escape) to close - (letter) to select item");
	con.printf(1, INVENTORY_HEIGHT - 2, "Shift-(letter) for item description");

	// wait for a key press
	while (true)
	{
		// blit the inventory console on the root console
		TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
			TCODConsole::root, engine.screenWidth / 2 - INVENTORY_WIDTH / 2,
			engine.screenHeight / 2 - INVENTORY_HEIGHT / 2);
		TCODConsole::flush();
		TCOD_key_t key = TCODConsole::waitForKeypress(true);
		if (key.vk == TCODK_CHAR && !key.shift)
		{
			int actorIndex = key.c - 'a';

			if (actorIndex >= 0 && actorIndex < owner->container->inventory.size())
			{
				return owner->container->inventory.get(actorIndex);
			}
		}
		else if (key.vk == TCODK_CHAR && key.shift)
		{
			int actorIndex = key.c - 'a';

			if (actorIndex >= 0 && actorIndex < owner->container->inventory.size())
			{
				Actor *actor = owner->container->inventory.get(actorIndex);
				engine.gui->drawGenericSizedWindow(actor->pickable->itemDescription, actor->name.c_str());
				while (true)
				{
					key = TCODConsole::waitForKeypress(true);
					if (key.vk == TCODK_SPACE)
					{
						break;
					}
				}
			}
		}
		else if (key.vk == TCODK_ESCAPE)
		{
			return NULL;
		}
	}
	return NULL;
}

void MonsterAi::load(TCODZip &zip) 
{
	moveCount = zip.getInt();
}

void MonsterAi::save(TCODZip &zip) 
{
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
}

void MonsterAi::update(Actor *owner, bool playerResting) 
{
	if (playerResting)
	{
		approachPlayerOnRest(owner, engine.player);
		return;
	}
	if (owner->destructible && owner->destructible->isDead()) 
	{
		return;
	}
	if (engine.map->isInFov(owner->x, owner->y)) 
	{
		// we can see the player. move towards him
		moveCount = TRACKING_TURNS;
	}
	else {
		moveCount--;
	}
	if (moveCount > 0) {
		moveOrAttack(owner, engine.player->x, engine.player->y);
	}
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety)
{
	float dx = targetx - owner->x;
	float dy = targety - owner->y;
	float stepdx = (dx > 0 ? 1 : -1);
	float stepdy = (dy > 0 ? 1 : -1);
	float distance = sqrtf(dx*dx + dy * dy);
	if (!owner->attacker->ranged && distance >= 2 || owner->attacker->ranged && distance > owner->attacker->range
		&& engine.map->isInFov(engine.player->x, engine.player->y))
	{
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));
		if (engine.map->canWalk(owner->x + dx, owner->y + dy)) {
			owner->x += dx;
			owner->y += dy;
		}
		else if (engine.map->canWalk(owner->x + stepdx, owner->y)) {
			owner->x += stepdx;
		}
		else if (engine.map->canWalk(owner->x, owner->y + stepdy)) {
			owner->y += stepdy;
		}
	}
	else if (owner->attacker) 
	{
		owner->attacker->attack(owner, engine.player);
	}
}

void MonsterAi::approachPlayerOnRest(Actor *owner, Actor * player)
{
	int distance = owner->getDistance(player->x, player->y);
	if (distance >= 2 && distance <= 8)
	{
		moveOrAttack(owner, player->x, player->y);
	}
}

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns)
	: TemporaryAi(nbTurns)
{

}

void ConfusedMonsterAi::load(TCODZip &zip) 
{
	nbTurns = zip.getInt();
	bool hasOldAi = zip.getInt();
	if (hasOldAi) 
	{
		oldAi = ConfusedMonsterAi::create(zip);
	}
}

void ConfusedMonsterAi::save(TCODZip &zip) 
{
	zip.putInt(CONFUSED_MONSTER);
	zip.putInt(nbTurns);
	zip.putInt(oldAi != NULL);
	if (oldAi) oldAi->save(zip);
}

void ConfusedMonsterAi::update(Actor *owner) 
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dx = rng->getInt(-1, 1);
	int dy = rng->getInt(-1, 1);
	if (dx != 0 || dy != 0) 
	{
		int destx = owner->x + dx;
		int desty = owner->y + dy;
		if (engine.map->canWalk(destx, desty)) 
		{
			owner->x = destx;
			owner->y = desty;
		}
		else 
		{
			Actor *actor = engine.getActor(destx, desty);
			if (actor) 
			{
				owner->attacker->attack(owner, actor);
			}
		}
	}
	TemporaryAi::update(owner);
}