#pragma once
class Equipment : public Persistent
{
public:
	Actor *mainHand = NULL;
	Actor *offHand = NULL;
	Actor *chest = NULL;
	Actor *legs = NULL;
	Actor *feet = NULL;
	Actor *hands = NULL;
	Actor *head = NULL;
	Actor *shoulders = NULL;
	Actor *back = NULL;

	Equipment(Actor *mainHand = NULL, Actor *offHand = NULL, Actor *chest = NULL, Actor *legs = NULL,
				Actor *feet = NULL, Actor *hands = NULL, Actor *head = NULL, Actor *shoulders = NULL, Actor *back = NULL);
	~Equipment();
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	static Equipment *create(TCODZip &zip);
};