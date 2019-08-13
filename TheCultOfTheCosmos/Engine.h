#pragma once
#ifndef ENGINE_H
#define ENGINE_H

class Engine
{
public:

	enum GameStatus
	{
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;

	TCODList<Actor *> actors;
	Actor *player;
	Actor *stairsDown;
	Actor *stairsUp;
	Map *map;

	int screenWidth;
	int screenHeight;
	Gui *gui;

	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	Engine(int screenWidth, int screenHeight);
	~Engine();

	int floor;
	int deepestFloor = 0;
	void nextFloor();
	void previousFloor();

	Actor *getClosestMonster(int x, int y, float range) const;
	Actor *getClosestTalkable(int x, int y, float range) const;
	Actor *getActor(int x, int y) const;
	bool pickATile(int *x, int *y, float maxRange = 0.0f, int sizeOfEffect = 0);
	void init();
	void term();
	void load();
	void save();
	void characterGeneration();
	void showControls();
	void clearSavedFloors();
	void getLowestFloor();
	void prepareFloorLoading();
	void update();
	void render();

	void sendToBack(Actor *actor);

	int fovRadius;

private:
	bool computeFov;
};

extern Engine engine;

#endif