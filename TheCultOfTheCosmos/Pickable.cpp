#include "pch.h"
#include "main.h"

Pickable::Pickable(ItemName name, std::vector<std::string> itemDescription, TargetSelector *selector,
	Effect *effect, EquipmentSlot slot, bool equippable, bool usable, int minDamage, int maxDamage, int armor)
	: name(name), selector(selector), itemDescription(itemDescription), effect(effect), 
	  slot(slot), equippable(equippable), usable(usable), minDamage(minDamage), maxDamage(maxDamage), armor(armor)
{

}

Pickable::~Pickable()
{
	if (selector) delete selector;
	if (effect) delete effect;
}




bool Pickable::pick(Actor *owner, Actor *wearer)
{
	if (wearer->container && wearer->container->add(owner))
	{
		engine.actors.remove(owner);
		return true;
	}
	return false;
}

void Pickable::drop(Actor *owner, Actor *wearer)
{
	if (wearer->container)
	{
		wearer->container->remove(owner);
		engine.actors.push(owner);
		owner->x = wearer->x;
		owner->y = wearer->y;
		engine.gui->message(TCODColor::lightGrey, "%s drops a %s.",
			wearer->name.c_str(), owner->name.c_str());
	}
}

bool Pickable::use(Actor *owner, Actor *wearer)
{
	TCODList<Actor *> list;
	if (selector)
	{
		selector->selectTargets(wearer, list);
	}
	else
	{
		list.push(wearer);
	}
	bool succeed = false;
	for (Actor **it = list.begin(); it != list.end(); it++)
	{
		if (effect->applyTo(*it))
		{
			succeed = true;
		}
	}
	if (succeed)
	{
		if (wearer->container)
		{
			wearer->container->remove(owner);
			delete owner;
		}
	}
	return succeed;
}


bool Pickable::equipEquippable(Actor *owner, Actor *wearer)
{
	EquipmentSlot slot = owner->pickable->slot;
	switch (slot)
	{
	case MAIN_HAND:
	{
		if (wearer->equipment->mainHand != NULL)
		{
			unEquipEquippable(wearer->equipment->mainHand, wearer);
		}
		if (wearer->equipment->mainHand == NULL)
		{
			wearer->equipment->mainHand = owner;
			updateStats(wearer, slot, true);
			owner->pickable->equipped = true;
		}
	}
	break;
	case OFF_HAND:
	{
		if (wearer->equipment->offHand != NULL)
		{
			unEquipEquippable(wearer->equipment->offHand, wearer);
		}
		if (wearer->equipment->offHand == NULL)
		{
			wearer->equipment->offHand = owner;
			updateStats(wearer, slot, true);
			owner->pickable->equipped = true;
		}
	}
	break;
	case CHEST:
	{
		if (wearer->equipment->chest != NULL)
		{
			unEquipEquippable(wearer->equipment->chest, wearer);
		}
		if (wearer->equipment->chest == NULL)
		{
			wearer->equipment->chest = owner;
			updateStats(wearer, slot, true);
			owner->pickable->equipped = true;
		}
	}
	break;
	default: return false;
	}
	engine.gui->message(TCODColor::lightGrey, "You equip the %s.", owner->name.c_str());
	return true;
}

bool Pickable::unEquipEquippable(Actor *owner, Actor *wearer)
{
	EquipmentSlot slot = owner->pickable->slot;
	switch (slot)
	{
	case MAIN_HAND:
	{
		if (wearer->equipment->mainHand != NULL)
		{
			updateStats(wearer, slot, false);
			wearer->equipment->mainHand = NULL;
			owner->pickable->equipped = false;
		}
	}
	break;
	case OFF_HAND:
	{
		if (wearer->equipment->offHand != NULL)
		{
			updateStats(wearer, slot, false);
			wearer->equipment->offHand = NULL;
			owner->pickable->equipped = false;
		}
	}
	break;
	case CHEST:
	{
		if (wearer->equipment->chest != NULL)
		{
			updateStats(wearer, slot, false);
			wearer->equipment->chest = NULL;
			owner->pickable->equipped = false;
		}
	}
	break;
	default: return false;
	}
	engine.gui->message(TCODColor::lightGrey, "You unequip the %s.", owner->name.c_str());
	return true;
}

void Pickable::updateStats(Actor *wearer, Pickable::EquipmentSlot slot, bool additive)
{
	switch (slot)
	{
	case(MAIN_HAND):
	{
		if (additive)
		{
			if (this->minDamage >= 0)
			{
				wearer->attacker->minItemDamageModifier += this->minDamage;
				wearer->attacker->maxItemDamageModifier += this->maxDamage;
			}
			if (this->armor != NULL)
			{
				wearer->destructible->armor += this->armor;
			}
		}
		else
		{
			if (this->minDamage >= 0)
			{
				wearer->attacker->minItemDamageModifier -= this->minDamage;
				wearer->attacker->maxItemDamageModifier -= this->maxDamage;
			}
			if (this->armor != NULL)
			{
				wearer->destructible->armor -= this->armor;
			}
		}
		engine.player->attacker->updateDamageModifier();
	}
	break;
	case(OFF_HAND):
	{
		if (additive)
		{
			if (this->minDamage >= 0)
			{
				wearer->attacker->minItemDamageModifier += this->minDamage;
				wearer->attacker->maxItemDamageModifier += this->maxDamage;
			}
			if (this->armor != NULL)
			{
				wearer->destructible->armor += this->armor;
			}
		}
		else
		{
			if (this->minDamage >= 0)
			{
				wearer->attacker->minItemDamageModifier -= this->minDamage;
				wearer->attacker->maxItemDamageModifier -= this->maxDamage;
			}
			if (this->armor != NULL)
			{
				wearer->destructible->armor -= this->armor;
			}
		}
		engine.player->attacker->updateDamageModifier();
	}
	break;
	case(CHEST):
	{
		if (additive)
		{
			if (this->minDamage >= 0)
			{
				wearer->attacker->minItemDamageModifier -= this->minDamage;
				wearer->attacker->maxItemDamageModifier -= this->maxDamage;
			}
			if (this->armor != NULL)
			{
				wearer->destructible->armor += this->armor;
			}
		}
		else
		{
			if (this->minDamage >= 0)
			{
				wearer->attacker->minItemDamageModifier -= this->minDamage;
				wearer->attacker->maxItemDamageModifier -= this->maxDamage;
			}
			if (this->armor != NULL)
			{
				wearer->destructible->armor -= this->armor;
			}
		}
		engine.player->attacker->updateDamageModifier();
	}
	break;
	}
}

Pickable *Pickable::create(TCODZip &zip) 
{
	Pickable *pickable = NULL;
	itemListOne items;
	std::vector<std::string> description;
	ItemName name = (ItemName)zip.getInt();
	switch (name)
	{
		case ROOM_ONE_FLOOR_ONE_SCROLL_OF_USELESS_GOODIES:
		{
			description.push_back("I knew you'd come. Find me several floors below.");
			description.push_back("                                           -F.B.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(ROOM_ONE_FLOOR_ONE_SCROLL_OF_USELESS_GOODIES, description, NULL, 
				NULL, Pickable::INVENTORY, false, false);
		}break;
		case SMALL_HEALING_POTION:
		{
			description.push_back("A small potion of healing.");
			description.push_back("");
			description.push_back("The alchemists of Kear'il have long been renowned");
			description.push_back("for their talents in brewing foul tasting drinks");
			description.push_back("capable of rapidly restoring the vitality of the");
			description.push_back("embiber.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(SMALL_HEALING_POTION, description, NULL, new HealthEffect(40, NULL), Pickable::INVENTORY, false, true);
		}break;
		case SCROLL_OF_FIREBALL:
		{
			description.push_back("A scorched sheet of paper inscribed with runes.");
			description.push_back("");
			description.push_back("Many of the mages from the eastern land of");
			description.push_back("Az'ral Irabbi favor fire based magic.");
			description.push_back("This scroll is a testament to their desire");
			description.push_back("to spread their knowledge to even the lay-man.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(SCROLL_OF_FIREBALL, description,
				new TargetSelector(TargetSelector::SELECTED_RANGE, 1),
				new HealthEffect(-25, "The %s gets burned for %g damage!"), INVENTORY, false, true);
		}break;
		case SCROLL_OF_LIGHTNING_BOLT:
		{
			description.push_back("A sheet of paper that crackles softly when tocuhed.");
			description.push_back("");
			description.push_back("The harnessing of electricity has long been sought");
			description.push_back("by the gnomish engineers of Ulon. This scroll is");
			description.push_back("merely an imitation created by some of the more");
			description.push_back("magical of them.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(SCROLL_OF_LIGHTNING_BOLT, description,
				new TargetSelector(TargetSelector::CLOSEST_MONSTER, 5),
				new HealthEffect(-40, "A lightning bolt strikes the %s with a loud crack!\n"
					"It takes %g damage."), Pickable::INVENTORY, false, true);
		}break;
		case SCROLL_OF_CONFUSION:
		{
			description.push_back("Your eyes glaze over when you look upon this scroll.");
			description.push_back("");
			description.push_back("GHZA MAGI BADGINZAA REL UOCK PLA MAGI.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(SCROLL_OF_CONFUSION, description,
				new TargetSelector(TargetSelector::SELECTED_MONSTER, 5),
				new AiChangeEffect(new ConfusedMonsterAi(10),
					"The eyes of the %s look vacant."), INVENTORY, false, true);
		}break;
		/*###########################################################################*/
		/*###########################################################################*/
		/*##################                       WEAPONS            ###############*/
		/*###########################################################################*/
		/*###########################################################################*/
		case RUSTY_IRON_SWORD:
		{
			description.push_back("A rusted and cracked iron sword.");
			description.push_back("");
			description.push_back("Long ago a knight stood atop the battlements of");
			description.push_back("lost Fort Greymount.");
			description.push_back("He looked wistfully to the fields in the distance.");
			description.push_back("");
			description.push_back("\"Such a waste...\", he thought, as he thre his");
			description.push_back("sword to the ground.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(RUSTY_IRON_SWORD, description, NULL, NULL, MAIN_HAND, true, false);
			pickable->minDamage = 0;
			pickable->maxDamage = 2;
		}break;
		/*###########################################################################*/
		/*###########################################################################*/
		/*##################                       SHIELDS            ###############*/
		/*###########################################################################*/
		/*###########################################################################*/
		case CRACKED_WOODEN_SHIELD:
		{
			description.push_back("A shattered board that could be called a shield.");
			description.push_back("");
			description.push_back("In moments of crises even this might be useful.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(CRACKED_WOODEN_SHIELD, description, NULL, NULL, OFF_HAND, true, false);
			pickable->armor = 1;
		}break;
		/*###########################################################################*/
		/*###########################################################################*/
		/*##################                       CHEST            #################*/
		/*###########################################################################*/
		/*###########################################################################*/
		case RIPPED_LEATHER_TUNIC:
		{
			description.push_back("A ripped leather tunic.");
			description.push_back("");
			description.push_back("It smells awful and provides next to no protection.");
			description.push_back("Press (space) to return.");
			pickable = new Pickable(RIPPED_LEATHER_TUNIC, description, NULL, NULL, CHEST, true, false);
			pickable->armor = 1;
		}
		default: break;
	}
	pickable->load(zip);
	return pickable;
}

void Pickable::save(TCODZip & zip)
{
	zip.putInt(name);
	zip.putInt(equipped);
}

void Pickable::load(TCODZip & zip)
{
	equipped = zip.getInt();
}