
#include "object.h"
#include "../world.h"
#include "../collision/collision.h"
#include "../action.h"
#include "../player.h"
#include "unit.h"
#include "building.h"

Object::Object(World* world, enum ObjectType type, unsigned long long id, int equip, vec2& pos, float radio)
{
	this->world = world;
	this->type = type;
	this->id = id;
	this->equip = equip;
	this->pos = pos;
	this->radio = radio;
	
	this->action = NULL;
	
	this->world->getCollision()->addObject(this);
}

Object::~Object()
{
	this->world->getCollision()->removeObject(this);
	
	// This function make to subjects delete theirs actions.
	Action::removeLinkActions(this);
	
	this->world->unsetObjectId(this->id, this);
}

bool Object::isUnit()
{
	return this->type == OT_RECOLECTOR || this->type == OT_SOLDADO_RASO || this->type == OT_SOLDADO_ENTRENADO || 
			this->type == OT_TANQUE || this->type == OT_TANQUE_PESADO;
}

bool Object::isBuilding()
{
	return this->type == OT_BASE || this->type == OT_BARRACA || this->type == OT_TORRETA;
}

// Return player name (not delete!) or NULL if is a map object.
char* Object::getPlayerName()
{
	if(this->isUnit())
		return ((Unit*)this)->player->name;
	else if(this->isBuilding())
		return ((Building*)this)->player->name;
	else
		return NULL;
}

Action* Object::getAction()
{
	return this->action;
}

void Object::removeAction()
{
	// remove the action
	if(this->action != NULL)
	{
		delete this->action;
		this->action = NULL;
	}
}

vec2 Object::getPos()
{
	return this->pos;
}

void Object::setPos(vec2& p)
{
	this->pos = p;
	this->world->getCollision()->updateObject(this);
}
