#pragma once
class itemListOne
{
public:
	enum itemList
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

	Actor *create(itemList item, int x, int y);

protected:
	itemList item;
};