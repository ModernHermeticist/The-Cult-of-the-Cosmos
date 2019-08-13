#pragma once
class Pickable : public Persistent
{
public:
	enum EquipmentSlot
	{
		INVENTORY,
		MAIN_HAND,
		OFF_HAND,
		CHEST,
		LEGS
	};
	enum ItemName
	{
		NONE,
		ROOM_ONE_FLOOR_ONE_SCROLL_OF_USELESS_GOODIES,
		SMALL_HEALING_POTION,
		SCROLL_OF_FIREBALL,
		SCROLL_OF_LIGHTNING_BOLT,
		SCROLL_OF_CONFUSION,
		RUSTY_IRON_SWORD,
		CRACKED_WOODEN_SHIELD,
		RIPPED_LEATHER_TUNIC
	};

	bool equippable = false;
	bool equipped = false;
	bool usable = false;

	std::vector<std::string> itemDescription;


	Pickable(ItemName name, std::vector<std::string> itemDescription, TargetSelector *selector, 
		Effect *effect, EquipmentSlot slot, bool equippable, bool usable, int minDamage = NULL, int maxDamage = NULL, 
		int armor = NULL);
	~Pickable();
	bool pick(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
	bool equipEquippable(Actor *owner, Actor *wearer);
	bool unEquipEquippable(Actor *owner, Actor *wearer);
	void updateStats(Actor *wearer, Pickable::EquipmentSlot slot, bool additive);
	static Pickable *create(TCODZip &zip);

	void load(TCODZip &zip);
	void save(TCODZip &zip);

//#################################################################//
/*                      ITEM MODIFIERS                             */
//#################################################################//
	int minDamage = NULL;
	int maxDamage = NULL;
	int armor = NULL;

//#################################################################//
//#################################################################//
//#################################################################//
protected:
	TargetSelector *selector;
	Effect *effect;
	EquipmentSlot slot;
	ItemName name;
};
