#include "pch.h"
#include "main.h"

char smallHealingPotionTile = '!';
char rustyIronSwordTile = '/';
char flimsyWoodenBowTile = 'D';
char crackedWoodenShieldTile = 'O';
char rippedLeatherTunicTile = 'T';
char rippedLeatherPantsTile = 'M';
char fireBallScrollTile = '#';
char lightningScrollTile = '#';
char confusionScrollTile = '#';
char envelopeTile = 'v';

Actor *itemListOne::create(itemList item, int x, int y)
{
	std::vector<std::string> description;
	Actor *itemActor = NULL;
	switch (item)
	{
		case ROOM_ONE_FLOOR_ONE_SCROLL_OF_USELESS_GOODIES:
		{
			description.push_back("I knew you'd come. Live long enough and you'll find me.");
			description.push_back("                                           -F.B.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(x, y, envelopeTile, "Mysterious Note",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::ROOM_ONE_FLOOR_ONE_SCROLL_OF_USELESS_GOODIES, description, NULL,
				NULL, Pickable::INVENTORY, false, false);
		}
		break;
		case SMALL_HEALING_POTION:
		{
			// create a health potion
			description.push_back("A small potion of healing.");
			description.push_back("");
			description.push_back("The alchemists of Kear'il have long been renowned");
			description.push_back("for their talents in brewing foul tasting drinks");
			description.push_back("capable of rapidly restoring the vitality of the");
			description.push_back("embiber.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(x, y, smallHealingPotionTile, "Small Health Potion",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::SMALL_HEALING_POTION, description, NULL,
				new HealthEffect(40, NULL), Pickable::INVENTORY, false, true);
		}
		break;

		case SCROLL_OF_FIREBALL:
		{
			// create a scroll of fireball
			description.push_back("A scorched sheet of paper inscribed with runes.");
			description.push_back("");
			description.push_back("Many of the mages from the eastern land of");
			description.push_back("Az'ral Irabbi favor fire based magic.");
			description.push_back("This scroll is a testament to their desire");
			description.push_back("to spread their knowledge to even the lay-man.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(x, y, fireBallScrollTile, "Scroll of Fireball",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::SCROLL_OF_FIREBALL, description,
				new TargetSelector(TargetSelector::SELECTED_RANGE, 1),
				new HealthEffect(-25, "The %s gets burned for %g damage!"), Pickable::INVENTORY, false, true);
		}
		break;

		case SCROLL_OF_LIGHTNING_BOLT:
		{
			// create a scroll of lightning bolt 
			description.push_back("A sheet of paper that crackles softly when touched.");
			description.push_back("");
			description.push_back("The harnessing of electricity has long been sought");
			description.push_back("by the engineers of Ulon. This scroll is");
			description.push_back("merely an imitation created by some of the more");
			description.push_back("magical of them.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(x, y, lightningScrollTile, "Scroll of Lightning Bolt",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::SCROLL_OF_LIGHTNING_BOLT,
				description, new TargetSelector(TargetSelector::CLOSEST_MONSTER, 5),
				new HealthEffect(-40, "A lightning bolt strikes the %s with a loud crack!\n"
					"It takes %g damage."), Pickable::INVENTORY, false, true);
		}
		break;

		case SCROLL_OF_CONFUSION:
		{
			// create a scroll of confusion
			description.push_back("Your eyes glaze over when you look upon this scroll.");
			description.push_back("");
			description.push_back("GHZA MAGI BADGINZAA REL UOCK PLA MAGI.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(x, y, confusionScrollTile, "Scroll of Confusion",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::SCROLL_OF_CONFUSION,
				description, new TargetSelector(TargetSelector::SELECTED_MONSTER, 5),
				new AiChangeEffect(new ConfusedMonsterAi(10),
					"The eyes of the %s look vacant."), Pickable::INVENTORY, false, true);
		}
		break;

		case RUSTY_IRON_SWORD:
		{
			// spawn test weapon
			description.push_back("A cracked tire iron.");
			description.push_back("");
			description.push_back("Capable of a variety of delinquet actions.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(engine.player->x + 1, engine.player->y + 1, rustyIronSwordTile, "Rusty Iron Sword",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::RUSTY_IRON_SWORD, description, NULL, NULL, Pickable::MAIN_HAND, true, false);
			itemActor->pickable->minDamage = 0;
			itemActor->pickable->maxDamage = 2;
		}
		break;

		case CRACKED_WOODEN_SHIELD:
		{
			// spawn test shield
			description.push_back("A shattered board that could be called a shield.");
			description.push_back("");
			description.push_back("In moments of crises even this might be useful.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(engine.player->x, engine.player->y + 1, crackedWoodenShieldTile, "Cracked Wooden Shield",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::CRACKED_WOODEN_SHIELD, description, NULL, NULL, Pickable::OFF_HAND, true, false);
			itemActor->pickable->armor = 1;
		}
		break;

		case RIPPED_LEATHER_TUNIC:
		{
			// spawn test chest piece
			description.push_back("A ripped leather tunic.");
			description.push_back("");
			description.push_back("It smells awful and provides next to no protection.");
			description.push_back("Press (space) to return.");
			itemActor = new Actor(engine.player->x, engine.player->y - 1, rippedLeatherTunicTile, "Ripped Leather Tunic",
				TCODColor::white);
			itemActor->blocks = false;
			itemActor->pickable = new Pickable(Pickable::RIPPED_LEATHER_TUNIC, description, NULL, NULL, Pickable::CHEST, true, false);
			itemActor->pickable->armor = 1;
		}
		break;
	}
	return itemActor;
}
