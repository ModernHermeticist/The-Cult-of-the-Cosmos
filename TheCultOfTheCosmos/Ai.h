#pragma once
class Ai : public Persistent
{
public:
	virtual ~Ai() {};
	virtual void update(Actor *owner, bool playerMoving = false) = 0;
	static Ai *create(TCODZip &zip);
protected:
	enum AiType
	{
		MONSTER, CONFUSED_MONSTER, PLAYER
	};
};

class TemporaryAi : public Ai
{
public:
	TemporaryAi(int nbTurns);
	void update(Actor *owner, bool playerMoving = false);
	void applyTo(Actor *actor);
	static TemporaryAi *create(TCODZip &zip);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected:
	int nbTurns;
	Ai *oldAi;
};

class PlayerAi : public Ai
{
public:
	int xpLevel;
	PlayerAi();
	int getNextLevelXp();
	void update(Actor *owner, bool playerMoving = false);
	void load(TCODZip &zip);
	void save(TCODZip &zip);

protected:
	void handleActionKey(Actor *owner, int ascii);
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	Actor *choseFromInventory(Actor *owner);
	void displayCharacterInformation();
};

class MonsterAi : public Ai {
public:
	void update(Actor *owner, bool playerResting = false);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	void approachPlayerOnRest(Actor *owner, Actor *player);

protected:
	int moveCount;
	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedMonsterAi : public TemporaryAi
{
public:
	ConfusedMonsterAi(int nbTurns);
	void update(Actor *owner);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};