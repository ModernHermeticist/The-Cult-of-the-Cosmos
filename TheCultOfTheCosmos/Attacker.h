#pragma once
class Attacker : public Persistent
{
public :
	float baseMaxDamage = 0; // unmodified max damage
	float baseMinDamage = 0; // unmodified min damage

	float maxDamage = 0; // maximum on damage range
	float minDamage = 0; // minimum on damage range

	float damageStrengthModifier = 0; // strength modifier on overall damage

	float minItemDamageModifier = 0;
	float maxItemDamageModifier = 0;

	bool ranged = false;
	int range = 0;

	Attacker(float baseMinDamage, float baseMaxDamage);
	Attacker(float baseMinDamage, float baseMaxDamage, bool ranged, int range);
	void updateDamageModifier();
	void attack(Actor *owner, Actor *target);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};