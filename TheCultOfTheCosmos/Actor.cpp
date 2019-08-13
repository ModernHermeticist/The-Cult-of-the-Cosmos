#include "pch.h"
#include "main.h"


Actor::Actor(int x, int y, int ch, std::string name, const TCODColor &col) :
	x(x), y(y), ch(ch), col(col), name(name),
	blocks(true), fovOnly(true), talkable(false), attacker(NULL), destructible(NULL), ai(NULL),
	pickable(NULL), equipment(NULL), container(NULL)
{
}

Actor::~Actor()
{
	if (attacker) delete attacker;
	if (destructible) delete destructible;
	if (ai) delete ai;
	if (pickable) delete pickable;
	if (equipment) delete equipment;
	if (container) delete container;
}

void Actor::save(TCODZip &zip) 
{
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(ch);
	zip.putColor(&col);
	zip.putString(name.c_str());
	zip.putInt(blocks);
	zip.putInt(fovOnly);
	zip.putInt(talkable);
	zip.putString(talkableName.c_str());
	zip.putString(talkableType.c_str());
	zip.putInt(attacker != NULL);
	zip.putInt(destructible != NULL);
	zip.putInt(ai != NULL);
	zip.putInt(pickable != NULL);
	zip.putInt(equipment != NULL);
	zip.putInt(container != NULL);
	if (attacker) attacker->save(zip);
	if (destructible) destructible->save(zip);
	if (ai) ai->save(zip);
	if (pickable) pickable->save(zip);
	if (equipment) equipment->save(zip);
	if (container) container->save(zip);
}

void Actor::load(TCODZip &zip) 
{
	const char* tempName;
	x = zip.getInt();
	y = zip.getInt();
	ch = zip.getInt();
	col = zip.getColor();
	tempName = zip.getString();
	name = tempName;
	blocks = zip.getInt();
	fovOnly = zip.getInt();
	talkable = zip.getInt();
	tempName = zip.getString();
	talkableName = tempName;
	tempName = zip.getString();
	talkableType = tempName;
	bool hasAttacker = zip.getInt();
	bool hasDestructible = zip.getInt();
	bool hasAi = zip.getInt();
	bool hasPickable = zip.getInt();
	bool hasEquipment = zip.getInt();
	bool hasContainer = zip.getInt();
	if (hasAttacker) 
	{
		attacker = new Attacker(0.0f, 0.0f);
		attacker->load(zip);
	}
	if (hasDestructible) 
	{
		destructible = Destructible::create(zip);
	}
	if (hasAi) 
	{
		ai = Ai::create(zip);
	}
	if (hasPickable) 
	{
		pickable = Pickable::create(zip);
	}
	if (hasEquipment)
	{
		equipment = new Equipment(NULL);
		equipment->load(zip);
	}
	if (hasContainer) 
	{
		container = new Container(0);
		container->load(zip);
	}
}

float Actor::getDistance(int cx, int cy) const
{
	float dx = x - cx;
	float dy = y - cy;
	return sqrtf(dx*dx + dy * dy);
}

void Actor::drawAttackFeedback()
{

}


void Actor::update()
{
	if (ai) ai->update(this);
}


void Actor::render() const
{
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, col);
}