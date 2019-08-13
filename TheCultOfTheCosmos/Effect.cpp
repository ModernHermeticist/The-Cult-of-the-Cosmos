#include "pch.h"
#include "main.h"


Effect * Effect::create(TCODZip & zip)
{
	EffectType type = (EffectType)zip.getInt();
	Effect *effect = NULL;
	switch (type)
	{
	case HEALTHEFFECT:
	{
		effect = new HealthEffect(0, 0);
		effect->load(zip);
	}
	break;
	case AICHANGEEFFECT:
	{
		effect = new AiChangeEffect(NULL, 0);
		effect->load(zip);
	}
	break;
	}
	return effect;
}

HealthEffect::HealthEffect(float amount, const char *message)
	: amount(amount), message(message)
{
	this->message = _strdup(message);
}

HealthEffect::~HealthEffect()
{
	free((char*)message);
}

void HealthEffect::load(TCODZip & zip)
{
	amount = zip.getFloat();
	bool hasMessage = zip.getInt();
	if (hasMessage)
	{
		message = _strdup(zip.getString());
	}
}
void HealthEffect::save(TCODZip & zip)
{
	zip.putInt(HEALTHEFFECT);
	zip.putFloat(amount);
	zip.putInt(message != NULL);
	if (message) zip.putString(message);
}


bool HealthEffect::applyTo(Actor *actor)
{
	if (!actor->destructible) return false;

	if (amount > 0)
	{
		float pointsHealed = actor->destructible->heal(amount);
		if (pointsHealed > 0)
		{
			if (message)
			{
				engine.gui->message(TCODColor::lightGrey, message, actor->name, pointsHealed);
			}
			return true;
		}
	}
	else
	{
		if (message && -amount - actor->destructible->armor > 0)
		{
			engine.gui->message(TCODColor::lightGrey, message, actor->name,
				-amount - actor->destructible->armor);
		}
		if (actor->destructible->takeDamage(actor, -amount) > 0)
		{
			return true;
		}
	}
	return false;
}
//###################################################//
//###################################################//
//###################################################//
//###################################################//
AiChangeEffect::AiChangeEffect(TemporaryAi *newAi, const char *message)
	: newAi(newAi), message(message)
{
	this->message = _strdup(message);
}

AiChangeEffect::~AiChangeEffect()
{
	free((char*)message);
}

bool AiChangeEffect::applyTo(Actor *actor)
{
	newAi->applyTo(actor);
	if (message)
	{
		engine.gui->message(TCODColor::lightGrey, message, actor->name);
	}
	return true;
}

void AiChangeEffect::load(TCODZip & zip)
{
	message = _strdup(zip.getString());
	bool hasNewAi = zip.getInt();
	if (hasNewAi)
	{
		newAi = TemporaryAi::create(zip);
		newAi->load(zip);
	}
}

void AiChangeEffect::save(TCODZip & zip)
{
	zip.putInt(AICHANGEEFFECT);
	zip.putString(message);
	zip.putInt(newAi != NULL);
	if (newAi) newAi->save(zip);
}