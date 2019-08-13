#pragma once
class Destructible : public Persistent
{
public:
	/*############################*/
	/*##########RACES#############*/
	enum raceTypes
	{
		NO_RACE,
		HUMAN,
		ELF,
		ORC
	};
	/*##########RACES#############*/
	/*############################*/

	/*#########CLASSES############*/
	/*############################*/
	enum classTypes
	{
		NO_CLASS,
		WARRIOR,
		ARCHER,
		MAGE,
	};
	/*#########CLASSES############*/
	/*############################*/

	float maxHp;            // maximum health points
	float hp;               // current health points

	float baseMaxHp;
	float healthConstitutionModifier;

	float maxMp;            // maximum mana points
	float mp;               // current mana points

	float baseMaxMp;
	float manaIntelligenceModifier;

	float strength;         // characters strength attribute
	float constitution;     // characters constitution attribute
	float dexterity;        // characters dexterity attribute
	float intelligence;
	float armor;            // damage reduction
	int xp;                 // XP gained when killing this monster ( or player xp)
	const char *corpseName; // actor's name once dead/destroyed
	const char *race;
	const char *className;

	const char *sex;

	int turnsUntilRest = 0;
	

	Destructible(float baseMaxHp, float baseMaxMp, float strength, float constitution, float dexterity,
		float intelligence, float armor, const char *corpseName, int xp, const char *sex = NULL, 
		const char *race = NULL, const char *className = NULL);
	Destructible(float baseMaxHp, float baseMaxMp, float armor, const char *corpseName, int xp);
	virtual ~Destructible();

	inline bool isDead() { return hp <= 0; }

	void updateHpModifier();
	void updateMpModifier();

	float takeDamage(Actor *owner, float damage);
	float heal(float amount);

	virtual void die(Actor *owner);

	void load(TCODZip &zip);
	void save(TCODZip &zip);
	static Destructible *create(TCODZip &zip);
protected:
	enum DestructibleType
	{
		MONSTER, PLAYER
	};
	raceTypes raceType;
	classTypes classType;
};

class MonsterDestructible : public Destructible
{
public:
	MonsterDestructible(float baseMaxHp, float baseMaxMp, float armor, const char *corpseName, int xp);
	void save(TCODZip &zip);
	void die(Actor *owner);
};

class PlayerDestructible : public Destructible
{
public:
	PlayerDestructible(float baseMaxHp, float baseMaxMp, float strength, float constitution, float dexterity,
		float intelligence, float armor, const char *corpseName, int xp, const char *sex = NULL, 
		const char *race = NULL, const char *className = NULL);
	void save(TCODZip &zip);
	void die(Actor *owner);
};
