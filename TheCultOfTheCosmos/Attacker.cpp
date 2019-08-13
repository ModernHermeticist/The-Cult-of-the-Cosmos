#include "pch.h"
#include "main.h"

Attacker::Attacker(float baseMinDamage, float baseMaxDamage) : baseMinDamage(baseMinDamage), baseMaxDamage(baseMaxDamage)
{}

Attacker::Attacker(float baseMinDamage, float baseMaxDamage, bool ranged, int range)
	: baseMinDamage(baseMinDamage), baseMaxDamage(baseMaxDamage), ranged(ranged), range(range)
{
}

void Attacker::load(TCODZip &zip) 
{
	baseMinDamage = zip.getFloat();
	baseMaxDamage = zip.getFloat();
	damageStrengthModifier = zip.getFloat();
	minDamage = zip.getFloat();
	maxDamage = zip.getFloat();
	ranged = zip.getInt();
	range = zip.getInt();
}

void Attacker::save(TCODZip &zip) 
{
	zip.putFloat(baseMinDamage);
	zip.putFloat(baseMaxDamage);
	zip.putFloat(damageStrengthModifier);
	zip.putFloat(minDamage);
	zip.putFloat(maxDamage);
	zip.putInt(ranged);
	zip.putInt(range);
}

void Attacker::updateDamageModifier()
{
	damageStrengthModifier = int(engine.player->destructible->strength / 3);
	minDamage = baseMinDamage + damageStrengthModifier + minItemDamageModifier;
	maxDamage = baseMaxDamage + damageStrengthModifier + maxItemDamageModifier;
}

void Attacker::attack(Actor *owner, Actor *target)
{
	if (target->destructible && !target->destructible->isDead())
	{
		TCODRandom *rng = TCODRandom::getInstance();
		float damage = rng->getInt(minDamage, maxDamage);
		if (damage - target->destructible->armor > 0)
		{
			engine.gui->message(owner==engine.player ? TCODColor::red : TCODColor::lightGrey,
				"%s attacks %s for %g damage.", owner->name.c_str(), target->name.c_str(),
				damage - target->destructible->armor);
		}
		else
		{
			engine.gui->message(TCODColor::lightGrey,
				"%s attacks %s but it has no effect!", owner->name.c_str(), target->name.c_str());
		}
		target->destructible->takeDamage(target, damage);
	}
	else
	{
		engine.gui->message(TCODColor::lightGrey,
			"%s attacks %s in vain.", owner->name, target->name);
	}
}