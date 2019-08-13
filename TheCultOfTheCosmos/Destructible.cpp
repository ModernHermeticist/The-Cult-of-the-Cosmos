#include "pch.h"
#include "main.h"


Destructible::Destructible(float baseMaxHp, float baseMaxMp, float strength, float constitution, float dexterity,
	float intelligence, float armor, const char *corpseName, int xp, const char *sex, const char *race, const char *className) :
	baseMaxHp(baseMaxHp), hp(baseMaxHp), baseMaxMp(baseMaxMp), mp(baseMaxMp), 
	strength(strength), constitution(constitution), dexterity(dexterity), intelligence(intelligence), 
	armor(armor), corpseName(corpseName), xp(xp), sex(sex), race(race), className(className)
{
	this->corpseName = _strdup(corpseName);
	this->sex = _strdup(sex);
	this->race = _strdup(race);
	this->className = _strdup(className);
}

Destructible::Destructible(float baseMaxHp, float baseMaxMp, float armor, const char * corpseName, int xp) :
	baseMaxHp(baseMaxHp), hp(baseMaxHp), baseMaxMp(baseMaxMp), mp(baseMaxMp), corpseName(corpseName), xp(xp)
{
	this->corpseName = _strdup(corpseName);
	this->sex = _strdup("None");
	this->race = _strdup("None");
	this->className = _strdup("None");
	maxHp = 0;
	healthConstitutionModifier = 0;
	maxMp = 0;
	manaIntelligenceModifier = 0;
	strength = 0;
	constitution = 0;
	dexterity = 0;
	intelligence = 0;
	armor = 0;
	raceType = raceTypes::NO_RACE;
	classType = classTypes::NO_CLASS;
}

Destructible::~Destructible()
{
	free((char*)corpseName);
	free((char*)sex);
	free((char*)race);
	free((char*)className);
}

void Destructible::load(TCODZip &zip) 
{
	baseMaxHp = zip.getFloat();
	healthConstitutionModifier = zip.getFloat();
	maxHp = zip.getFloat();
	hp = zip.getFloat();
	baseMaxMp = zip.getFloat();
	maxMp = zip.getFloat();
	mp = zip.getFloat();
	strength = zip.getFloat();
	constitution = zip.getFloat();
	dexterity = zip.getFloat();
	intelligence = zip.getFloat();
	armor = zip.getFloat();
	corpseName = _strdup(zip.getString());
	xp = zip.getInt();
	race = _strdup(zip.getString());
	className = _strdup(zip.getString());
}

void Destructible::save(TCODZip &zip) 
{
	zip.putFloat(baseMaxHp);
	zip.putFloat(healthConstitutionModifier);
	zip.putFloat(maxHp);
	zip.putFloat(hp);
	zip.putFloat(baseMaxMp);
	zip.putFloat(maxMp);
	zip.putFloat(mp);
	zip.putFloat(strength);
	zip.putFloat(constitution);
	zip.putFloat(dexterity);
	zip.putFloat(intelligence);
	zip.putFloat(armor);
	zip.putString(corpseName);
	zip.putInt(xp);
	zip.putString(race);
	zip.putString(className);
}

Destructible *Destructible::create(TCODZip &zip) {
	DestructibleType type = (DestructibleType)zip.getInt();
	Destructible *destructible = NULL;
	switch (type) 
	{
	case MONSTER: destructible = new MonsterDestructible(0, 0, 0, NULL, 0); break;
	case PLAYER: destructible = new PlayerDestructible(0, 0, 0, 0, 0, 0, 0, NULL, 0); break;
	}
	destructible->load(zip);
	return destructible;
}


void Destructible::updateHpModifier()
{
	healthConstitutionModifier = int(constitution / 3 * 10);
	maxHp = baseMaxHp + healthConstitutionModifier;
}

void Destructible::updateMpModifier()
{
	manaIntelligenceModifier = int(intelligence / 3 * 10);
	maxMp = baseMaxMp + manaIntelligenceModifier;
}

float Destructible::takeDamage(Actor *owner, float damage)
{
	damage -= armor;
	if (damage > 0)
	{
		turnsUntilRest = 5;
		hp -= damage;
		if (hp <= 0)
		{
			die(owner);
		}
	}
	else
	{
		damage = 0;
	}
	return damage;
}

float Destructible::heal(float amount)
{
	hp += amount;
	if (hp > maxHp)
	{
		amount -= hp - maxHp;
		hp = maxHp;
	}
	return amount;
}

void Destructible::die(Actor *owner)
{
	// transform the actor into a corpse!
	owner->ch = '%';
	owner->col = TCODColor::red;
	owner->blocks = false;
	// make sure corpses are drawn before living actors
	engine.sendToBack(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float maxMp, float strength, float constitution, float dexterity,
	float intelligence, float armor, const char *corpseName, int xp, const char *sex, const char *race, const char *className) :
	Destructible(maxHp, maxMp, strength, constitution, dexterity, intelligence, armor, corpseName, xp, sex, race, className)
{}


void PlayerDestructible::save(TCODZip &zip)
{
	zip.putInt(PLAYER);
	Destructible::save(zip);
}

void PlayerDestructible::die(Actor *owner)
{
	engine.gui->message(TCODColor::red, "You died!");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}

MonsterDestructible::MonsterDestructible(float maxHp, float maxMp, float armor, const char *corpseName, int xp) :
	Destructible(maxHp, maxMp, armor, corpseName, xp)
{}


void MonsterDestructible::save(TCODZip &zip)
{
	zip.putInt(MONSTER);
	Destructible::save(zip);
}

void MonsterDestructible::die(Actor *owner)
{
	// transform it into a nasty corpse! it doesn't block, can't be attacked
	// and doesn't move
	engine.gui->message(TCODColor::lightGrey, "%s is dead. You gain %d xp.",
		owner->name.c_str(), xp);
	owner->name = corpseName;
	engine.player->destructible->xp += xp;
	Destructible::die(owner);
}
