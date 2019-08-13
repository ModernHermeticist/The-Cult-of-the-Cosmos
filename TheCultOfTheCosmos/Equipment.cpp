#include "pch.h"
#include "main.h"

Equipment::Equipment(Actor *mainHand, Actor *offHand, Actor *chest, Actor *legs,
	Actor *feet, Actor *hands, Actor *head, Actor *shoulders,  Actor *back)
	: mainHand(mainHand), offHand(offHand), chest(chest), legs(legs), feet(feet),
		hands(hands), head(head), shoulders(shoulders), back(back)
{

}

Equipment::~Equipment()
{
	if (mainHand) delete mainHand;
	if (offHand) delete offHand;
	if (chest) delete chest;
	if (legs) delete legs;
	if (feet) delete feet;
	if (hands) delete hands;
	if (head) delete head;
	if (shoulders) delete shoulders;
	if (back) delete back;
}

Equipment *Equipment::create(TCODZip &zip)
{
	Equipment *equipment = new Equipment(NULL);
	equipment->load(zip);
	return equipment;
}

void Equipment::load(TCODZip & zip)
{
	bool hasMainHand = zip.getInt();
	if (hasMainHand)
	{
		mainHand = new Actor(0, 0, 0, "", TCODColor::white);
		mainHand->load(zip);
	}
	bool hasOffHand = zip.getInt();
	if (hasOffHand)
	{
		offHand = new Actor(0, 0, 0, "", TCODColor::white);
		offHand->load(zip);
	}
	bool hasChest = zip.getInt();
	if (hasChest)
	{
		chest = new Actor(0, 0, 0, "", TCODColor::white);
		chest->load(zip);
	}
	bool hasLegs = zip.getInt();
	if (hasLegs)
	{
		legs = new Actor(0, 0, 0, "", TCODColor::white);
		legs->load(zip);
	}
	bool hasFeet = zip.getInt();
	if (hasFeet)
	{
		feet = new Actor(0, 0, 0, "", TCODColor::white);
		feet->load(zip);
	}
	bool hasHands = zip.getInt();
	if (hasHands)
	{
		hands = new Actor(0, 0, 0, "", TCODColor::white);
		hands->load(zip);
	}
	bool hasHead = zip.getInt();
	if (hasHead)
	{
		head = new Actor(0, 0, 0, "", TCODColor::white);
		head->load(zip);
	}
	bool hasShoulders = zip.getInt();
	if (hasShoulders)
	{
		shoulders = new Actor(0, 0, 0, "", TCODColor::white);
		shoulders->load(zip);
	}
	bool hasBack = zip.getInt();
	if (hasBack)
	{
		back = new Actor(0, 0, 0, "", TCODColor::white);
		back->load(zip);
	}
}

void Equipment::save(TCODZip & zip)
{
	if (mainHand)
	{
		zip.putInt(1);
		mainHand->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (offHand)
	{
		zip.putInt(1);
		offHand->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (chest)
	{
		zip.putInt(1);
		chest->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (legs)
	{
		zip.putInt(1);
		legs->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (feet)
	{
		zip.putInt(1);
		feet->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (hands)
	{
		zip.putInt(1);
		hands->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (head)
	{
		zip.putInt(1);
		head->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (shoulders)
	{
		zip.putInt(1);
		shoulders->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
	if (back)
	{
		zip.putInt(1);
		back->save(zip);
	}
	else
	{
		zip.putInt(0);
	}
}
