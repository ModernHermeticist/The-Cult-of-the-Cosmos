#pragma once
class Actor : public Persistent
{
public:
	int x, y; // position on the map
	int ch;   // ascii code (tile code in future)
	TCODColor col; // color

	int attackFeedbackTurns = 3;
	bool showAttackFeedback = false;

	std::string name; //actor's name
	std::string talkableType;
	std::string talkableName;
	bool blocks; // can we walk on this actor
	bool fovOnly; // only display when in fov

	bool talkable;

	Attacker *attacker; // something that deals damage
	Destructible *destructible; // something that can be damaged
	Ai *ai; // something self-updating
	Pickable *pickable; // something that can be picked up and used
	Equipment *equipment; // set of actors this actor is wearing
	Container *container; // something that can contain other actors

	std::vector<std::string> dialogueSet;
	std::vector<std::string> talkOptions;


	Actor(int x, int y, int ch, std::string name, const TCODColor &col);
	~Actor();

	void load(TCODZip &zip);
	void save(TCODZip &zip);

	float getDistance(int cx, int cy) const;
	void drawAttackFeedback();
	void update();
	void render() const;
};