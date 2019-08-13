#pragma once
class Effect : public Persistent
{
public:
	virtual bool applyTo(Actor *actor) = 0;
	virtual void load(TCODZip &zip) = 0;
	virtual void save(TCODZip &zip) = 0;
	static Effect *create(TCODZip &zip);
protected:
	enum EffectType {
		HEALTHEFFECT, AICHANGEEFFECT
	};
};
//###################################################//
//###################################################//
//###################################################//
//###################################################//
class HealthEffect : public Effect
{
public:
	float amount;
	const char *message;

	HealthEffect(float amount, const char *message);
	~HealthEffect();
	bool applyTo(Actor *actor);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};
//###################################################//
//###################################################//
//###################################################//
//###################################################//
class AiChangeEffect : public Effect
{
public:
	TemporaryAi *newAi;
	const char *message;

	AiChangeEffect(TemporaryAi *newAi, const char *message);
	~AiChangeEffect();
	bool applyTo(Actor *actor);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};