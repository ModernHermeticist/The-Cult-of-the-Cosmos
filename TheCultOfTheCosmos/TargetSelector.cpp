#include "pch.h"
#include "main.h"

TargetSelector::TargetSelector(SelectorType type, float range)
	: type(type), range(range)
{

}


void TargetSelector::selectTargets(Actor *wearer, TCODList<Actor *> &list)
{
	switch (type)
	{
		case CLOSEST_MONSTER:
		{
			Actor *closestMonster = engine.getClosestMonster(wearer->x, wearer->y, range);
			if (closestMonster)
			{
				list.push(closestMonster);
			}
		}
		break;
		case SELECTED_MONSTER:
		{
			int x, y;
			engine.gui->message(TCODColor::cyan, "Left-click to select an enemy,\nor right-click to cancel.");
			if (engine.pickATile(&x, &y, range))
			{
				Actor *actor = engine.getActor(x, y);
				if (actor)
				{
					list.push(actor);
				}
			}
		}
		break;
		case WEARER_RANGE:
		{
			for (Actor **iterator = engine.actors.begin();
				iterator != engine.actors.end(); iterator++)
			{
				Actor *actor = *iterator;
				if (actor != wearer && actor->destructible && !actor->destructible->isDead()
					&& actor->getDistance(wearer->x, wearer->y) <= range)
				{
					list.push(actor);
				}
			}
		}
		break;
		case SELECTED_RANGE:
		{
			int x, y;
			engine.gui->message(TCODColor::cyan, "Left-click to select a tile,\nor right-click to cancel.");
			if (engine.pickATile(&x, &y))
			{
				for (Actor **iterator = engine.actors.begin();
					iterator != engine.actors.end(); iterator++)
				{
					Actor *actor = *iterator;
					if (actor->destructible && !actor->destructible->isDead()
						&& actor->getDistance(x, y) <= range)
					{
						list.push(actor);
					}
				}
			}
		}
		break;
	}
	if (list.isEmpty())
	{
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enough");
	}
}

TargetSelector *TargetSelector::create(TCODZip & zip)
{
	SelectorType type = (SelectorType)zip.getInt();
	TargetSelector *selector = NULL;
	switch (type)
	{
	case CLOSEST_MONSTER:
	{
		selector = new TargetSelector(CLOSEST_MONSTER, 0);
		selector->load(zip);
	}
	break;
	case SELECTED_MONSTER:
	{
		selector = new TargetSelector(SELECTED_MONSTER, 0);
		selector->load(zip);
	}
	break;
	case WEARER_RANGE:
	{
		selector = new TargetSelector(WEARER_RANGE, 0);
		selector->load(zip);
	}
	break;
	case SELECTED_RANGE:
	{
		selector = new TargetSelector(SELECTED_RANGE, 0);
		selector->load(zip);
	}
	break;
	}
	return selector;
}

void TargetSelector::load(TCODZip & zip)
{
	range = zip.getFloat();
}

void TargetSelector::save(TCODZip & zip)
{
	zip.putInt(type);
	zip.putFloat(range);
}
