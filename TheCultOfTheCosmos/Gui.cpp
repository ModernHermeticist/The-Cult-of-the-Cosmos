#include "pch.h"
#include "main.h"

static const int PANEL_HEIGHT = 7;
static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = PANEL_HEIGHT - 1;

Gui::Gui() 
{
	con = new TCODConsole(engine.screenWidth, PANEL_HEIGHT);
}

Gui::~Gui() 
{
	delete con;
	clear();
}

void Gui::clear()
{
	log.clearAndDelete();
}

void Gui::save(TCODZip &zip)
{
	zip.putInt(log.size());
	for (Message **it = log.begin(); it != log.end(); it++)
	{
		zip.putString((*it)->text);
		zip.putColor(&(*it)->col);
	}
}

std::string Gui::getCharacterName(std::vector<std::string> dialogueSet, const char *title)
{
	static const char letters[] = { 'a','b','c','d','e','f','g','h','i',
					'j','k','l','m','n','o','p','q','r',
					's','t','u','v','w','x','y','z' };

	std::string name = "";
	int x = 0;
	int y = 0;
	bool confirmed = false;
	y = dialogueSet.size() + 6;
	for (auto it = dialogueSet.begin(); it != dialogueSet.end(); it++)
	{
		std::string a = *it;
		if (a.size() > x)
		{
			x = a.size() + 2;
		}
	}

	const int CHARACTER_CREATION_WIDTH = x;
	const int CHARACTER_CREATION_HEIGHT = y;
	TCODConsole con(CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT);
	TCOD_key_t key;

	while (true)
	{
		con.clear();
		// display the character creation frame
		con.setDefaultForeground(TCODColor(200, 180, 50));
		con.printFrame(0, 0, CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT, true,
			TCOD_BKGND_DEFAULT, title);
		con.setDefaultForeground(TCODColor::white);
		int i = 2;
		for (auto it = dialogueSet.begin(); it != dialogueSet.end(); it++)
		{
			std::string a = *it;
			const char *c = a.c_str();
			if (it == dialogueSet.end() - 1)
			{
				con.printf(1, y - 4, "Name: %s", name.c_str());
				con.printf(1, y - 2, c);
			}
			else
			{
				con.printf(1, i, c);
			}
			i++;
		}

		// blit the character sheet console on the root console
		TCODConsole::blit(&con, 0, 0, CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT,
			TCODConsole::root, engine.screenWidth / 2 - CHARACTER_CREATION_WIDTH / 2,
			engine.screenHeight / 2 - CHARACTER_CREATION_HEIGHT / 2);
		TCODConsole::flush();
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
		for (int i = 0; i < 26; i++)
		{
			if (key.c == letters[i])
			{
				name += key.c;
				if (name.size() == 1)
				{
					std::transform(name.begin(), name.end(), name.begin(), ::toupper);
				}
			}
		}
		if (key.vk == TCODK_ENTER && !name.empty())
		{
			con.clear();
			break;
		}
		else if (key.vk == TCODK_BACKSPACE)
		{
			if (!name.empty())
			{
				name.pop_back();
			}
		}
	}
	return name;
}

void Gui::drawGenericSizedWindow(std::vector<std::string> dialogueSet, const char *title)
{
	int x = 0;
	int y = 0;
	y = dialogueSet.size() + 6;
	for (auto it = dialogueSet.begin(); it != dialogueSet.end(); it++)
	{
		std::string a = *it;
		if (a.size() > x)
		{
			x = a.size() + 2;
		}
	}

	const int CHARACTER_CREATION_WIDTH = x;
	const int CHARACTER_CREATION_HEIGHT = y;
	TCODConsole con(CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT);

	// display the character sheet frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT, true,
		TCOD_BKGND_DEFAULT, title);
	con.setDefaultForeground(TCODColor::white);
	int i = 2;
	for (auto it = dialogueSet.begin(); it != dialogueSet.end(); it++)
	{
		std::string a = *it;
		const char *c = a.c_str();
		if (it == dialogueSet.end() - 1)
		{
			con.printf(1, y - 2, c);
		}
		else
		{
			con.printf(1, i, c);
		}
		i++;
	}

	// blit the character sheet console on the root console
	TCODConsole::blit(&con, 0, 0, CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT,
		TCODConsole::root, engine.screenWidth / 2 - CHARACTER_CREATION_WIDTH / 2,
		engine.screenHeight / 2 - CHARACTER_CREATION_HEIGHT / 2);
	TCODConsole::flush();
}

void Gui::drawGenericSizeWindowWithTalkOptions(Actor *talker)
{
	TCOD_key_t key;
	bool leaveConversation = false;
	std::string curNode = "NodeOne";
	while (true)
	{
		rapidjson::Document d = openActorDialogueJSON();
		loadActorDialogue(d, curNode, talker);
		loadActorDialogueResponses(d, curNode, talker);
		menu.clear();
		int x = talker->name.size() + 4;
		int y = 0;
		std::vector<std::string> curDialogueSet = talker->dialogueSet;
		std::vector<std::string> curTalkOptions = talker->talkOptions;
		y = curDialogueSet.size() +  curTalkOptions.size() + 8;
		for (auto it = curDialogueSet.begin(); it != curDialogueSet.end(); it++)
		{
			std::string a = *it;
			if (a.size() > x)
			{
				x = a.size() + 4;
			}
		}

		const int CHARACTER_CREATION_WIDTH = x;
		const int CHARACTER_CREATION_HEIGHT = y;
		TCODConsole con(CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT);

		// display the character sheet frame
		con.setDefaultForeground(TCODColor(200, 180, 50));
		con.printFrame(0, 0, CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT, true,
			TCOD_BKGND_DEFAULT, talker->name.c_str());
		con.setDefaultForeground(TCODColor::white);
		int i = 2;
		for (auto it = curDialogueSet.begin(); it != curDialogueSet.end(); it++)
		{
			std::string a = *it;
			const char *c = a.c_str();

			con.printf(1, i, c);

			i++;
		}
		i += 2;
		
		int l = 1;
		for (auto it = curTalkOptions.begin(); it != curTalkOptions.end(); it++)
		{
			std::string a = *it;
			a = std::to_string(l) + ": " + a;
			const char *c = a.c_str();

			if (it == curTalkOptions.end() - 1)
			{
				con.printf(1, y - 2, "Press (esc) to stop talking.");
			}

			con.printf(1, i, c);


			l++;
			i++;
		}

		TCODConsole::blit(&con, 0, 0, CHARACTER_CREATION_WIDTH, CHARACTER_CREATION_HEIGHT,
			TCODConsole::root, engine.screenWidth / 2 - CHARACTER_CREATION_WIDTH / 2,
			engine.screenHeight / 2 - CHARACTER_CREATION_HEIGHT / 2);
		TCODConsole::flush();

		while (true)
		{
			TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
			if (key.vk == TCODK_ESCAPE)
			{
				leaveConversation = true;
				break;
			}
			else
			{
				curNode = interpretDialogueResponse(d, talker, curNode, key);
				if (curNode == "ExitNode")
				{
					leaveConversation = true;
				}
				break;
			}
		}
		if (leaveConversation)
		{
			break;
		}

		engine.render();
	}
}

void Gui::load(TCODZip &zip) 
{
	int nbMessages = zip.getInt();
	while (nbMessages > 0) 
	{
		const char *text = zip.getString();
		TCODColor col = zip.getColor();
		message(col, text);
		nbMessages--;
	}
}

void Gui::render() 
{
	// clear the GUI console
	con->setDefaultBackground(TCODColor::black);
	con->clear();

	// draw the health bar
	renderBar(1, 1, BAR_WIDTH, "HP", engine.player->destructible->hp,
		engine.player->destructible->maxHp,
		TCODColor::lightRed, TCODColor::darkerRed);

	// draw the mana bar
	renderBar(1, 2, BAR_WIDTH, "MP", engine.player->destructible->mp,
		engine.player->destructible->maxMp,
		TCODColor::lightBlue, TCODColor::darkerBlue);

	// draw the XP bar
	PlayerAi *ai = (PlayerAi *)engine.player->ai;
	char xpTxt[128];
	sprintf(xpTxt, "XP");
	renderBar(1, 3, BAR_WIDTH, xpTxt, engine.player->destructible->xp,
		ai->getNextLevelXp(), TCODColor::yellow, TCODColor::darkYellow, TCODColor::black);

	// draw the message log
	int y = 1;
	float colorCoef = 0.4f;
	for (Message **it = log.begin(); it != log.end(); it++) 
	{
		Message *message = *it;
		con->setDefaultForeground(message->col * colorCoef);
		con->printf(MSG_X, y, message->text);
		y++;
		if (colorCoef < 1.0f) {
			colorCoef += 0.3f;
		}
	}

	// mouse look
	renderMouseLook();

	// dungeon level
	con->setDefaultForeground(TCODColor::white);
	con->printf(1, 4, "Dungeon Floor %d", engine.floor);

	// blit the GUI console on the root console
	TCODConsole::blit(con, 0, 0, engine.screenWidth, PANEL_HEIGHT,
		TCODConsole::root, 0, engine.screenHeight - PANEL_HEIGHT);
}

void Gui::renderBar(int x, int y, int width, const char *name,
	float value, float maxValue, const TCODColor &barColor,
	const TCODColor &backColor, const TCODColor &textColor) 
{
	// fill the background
	con->setDefaultBackground(backColor);
	con->rect(x, y, width, 1, false, TCOD_BKGND_SET);

	int barWidth = (int)(value / maxValue * width);
	if (barWidth > 0) 
	{
		// draw the bar
		con->setDefaultBackground(barColor);
		con->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}
	// print text on top of the bar
	con->setDefaultForeground(textColor);
	con->printf(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER,
		"%s : %g/%g", name, value, maxValue);
}

Gui::Message::Message(const char *text, const TCODColor &col) :
	text(_strdup(text)), col(col) 
{
}

Gui::Message::~Message() 
{
	free(text);
}

void Gui::renderMouseLook() 
{
	if (!engine.map->isInFov(engine.mouse.cx, engine.mouse.cy)) 
	{
		// if mouse is out of fov, nothing to render
		return;
	}
	char buf[128] = "";
	bool first = true;
	for (Actor **it = engine.actors.begin(); it != engine.actors.end(); it++) 
	{
		Actor *actor = *it;
		// find actors under the mouse cursor
		if (actor->x == engine.mouse.cx && actor->y == engine.mouse.cy) 
		{
			if (!first) {
				strcat(buf, ", ");
			}
			else 
			{
				first = false;
			}
			strcat(buf, actor->name.c_str());
		}
	}
	// display the list of actors under the mouse cursor
	con->setDefaultForeground(TCODColor::lightGrey);
	con->printf(1, 0, buf);
}

void Gui::message(const TCODColor &col, const char *text, ...) 
{
	// build the text
	va_list ap;
	char buf[128];
	va_start(ap, text);
	vsprintf(buf, text, ap);
	va_end(ap);

	char *lineBegin = buf;
	char *lineEnd;
	do {
		// make room for the new message
		if (log.size() == MSG_HEIGHT) 
		{
			Message *toRemove = log.get(0);
			log.remove(toRemove);
			delete toRemove;
		}

		// detect end of the line
		lineEnd = strchr(lineBegin, '\n');
		if (lineEnd) 
		{
			*lineEnd = '\0';
		}

		// add a new message to the log
		Message *msg = new Message(lineBegin, col);
		log.push(msg);

		// go to next line
		lineBegin = lineEnd + 1;
	} while (lineEnd);
}


Menu::~Menu()
{
	clear();
}

void Menu::clear()
{
	items.clearAndDelete();
}

void Menu::addItem(MenuItemCode code, const char *label)
{
	MenuItem *item = new MenuItem();
	item->code = code;
	item->label = label;
	items.push(item);
}

const int PAUSE_MENU_WIDTH = 60;
const int PAUSE_MENU_HEIGHT = 15;
Menu::MenuItemCode Menu::pick(const char* title, DisplayMode mode)
{
	int selectedItem = 0;
	int menux, menuy;
	if (mode == PAUSE)
	{
		menux = engine.screenWidth / 2 - PAUSE_MENU_WIDTH / 2;
		menuy = engine.screenHeight / 2 - PAUSE_MENU_HEIGHT / 2;
		TCODConsole::root->setDefaultForeground(TCODColor(200, 180, 50));
		TCODConsole::root->printFrame(menux, menuy, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, true,
			TCOD_BKGND_ALPHA(70), title);
		menux += 2;
		menuy += 3;
	}
	else 
	{
		menux = engine.screenWidth / 2 - 7;
		menuy = TCODConsole::root->getHeight() / 3;
	}
	while (!TCODConsole::isWindowClosed()) 
	{
		int currentItem = 0;
		for (MenuItem **it = items.begin(); it != items.end(); it++) 
		{
			if (currentItem == selectedItem) 
			{
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			}
			else 
			{
				TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->printf(menux, menuy + currentItem * 3, (*it)->label);
			currentItem++;
		}
		TCODConsole::flush();

		// check key presses
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
		switch (key.vk) 
		{
		case TCODK_UP:
			selectedItem--;
			if (selectedItem < 0) 
			{
				selectedItem = items.size() - 1;
			}
			break;
		case TCODK_DOWN:
			selectedItem = (selectedItem + 1) % items.size();
			break;
		case TCODK_ENTER: return items.get(selectedItem)->code;
		default: break;
		}
	}
	return NONE;
}