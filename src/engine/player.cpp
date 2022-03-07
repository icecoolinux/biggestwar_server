
#include "player.h"
#include <string.h>
#include "area.h"

Player::Player(char* name, int equip)
{
	strcpy(this->name, name);
	this->equip = equip;
	
	this->minerals = INIT_MINERAL_PLAYER;
	this->oil = INIT_OIL_PLAYER;
}

Player::~Player()
{
	while(this->units.size() > 0)
	{
		Unit* u = this->units.front();
		this->units.pop_front();
		delete u;
	}
	while(this->buildings.size() > 0)
	{
		Building* b = this->buildings.front();
		this->buildings.pop_front();
		delete b;
	}
}

// Get and set resources.
int Player::getMinerals()
{
	return this->minerals;
}

int Player::getOil()
{
	return this->oil;
}

void Player::setMinerals(int m)
{
	this->minerals = m;
}

void Player::setOil(int o)
{
	this->oil = o;
}
		
void Player::update(int ms)
{
	list<Building*>::iterator it;
	for(it = this->buildings.begin(); it != this->buildings.end(); it++)
		(*it)->update(ms);
		
	list<Unit*>::iterator it2;
	for(it2 = this->units.begin(); it2 != this->units.end(); it2++)
		(*it2)->update(ms);
}

bool Player::addUnit(Unit* unit)
{
	if(this->units.size() >= MAX_UNITS_PLAYER)
		return false;
	
	this->units.push_back(unit);

	return true;
}

bool Player::quiteUnit(Unit* unit)
{
	list<Unit*>::iterator it;
	for(it = this->units.begin(); it != this->units.end(); it++)
	{
		if((*it)->id == unit->id)
		{
			this->units.erase(it);
			return true;
		}
	}
	return false;
}

bool Player::addBuilding(Building* building)
{
	if(this->buildings.size() >= MAX_BUILDINGS_PLAYER)
		return false;
	
	this->buildings.push_back(building);

	return true;
}

bool Player::quiteBuilding(Building* building)
{
	list<Building*>::iterator it;
	for(it = this->buildings.begin(); it != this->buildings.end(); it++)
	{
		if((*it)->id == building->id)
		{
			this->buildings.erase(it);
			return true;
		}
	}

	return false;
}

int Player::getNumUnits()
{
	return this->units.size();
}

int Player::getNumBuildings()
{
	return this->buildings.size();
}

void Player::destroyArea(Area* area)
{
	vec2 pos_;
	
	list<Building*>::iterator it = this->buildings.begin();
	while(it != this->buildings.end())
	{
		Building* b = *it;
		pos_ = b->getPos();
		if(area->isOut(pos_))
		{
			it = this->buildings.erase(it);
			delete b;
		}
		else
			it++;
	}
		
	list<Unit*>::iterator it2 = this->units.begin();
	while(it2 != this->units.end())
	{
		Unit* u = *it2;
		pos_ = u->getPos();
		if(area->isOut(pos_))
		{
			it2 = this->units.erase(it2);
			delete u;
		}
		else
			it2++;
	}
}

// Return object's positions and their types.
void Player::getObjectsInfo(list<vec2> &pos, list<float> &radio, list<enum ObjectType> &types)
{
	list<Building*>::iterator it;
	for(it = this->buildings.begin(); it != this->buildings.end(); it++)
	{
		pos.push_back( (*it)->getPos());
		radio.push_back( (*it)->radio);
		types.push_back( (*it)->type);
	}
	
	list<Unit*>::iterator it2;
	for(it2 = this->units.begin(); it2 != this->units.end(); it2++)
	{
		pos.push_back( (*it2)->getPos());
		radio.push_back( (*it2)->radio);
		types.push_back( (*it2)->type);
	}
}
		
// return true if action has been applied.
bool Player::setAction(Action* action)
{
	if(action->type == AT_NEWUNIT)
	{
		list<Building*>::iterator it;
		for(it = this->buildings.begin(); it != this->buildings.end(); it++)
		{
			if((*it)->id == action->getBuildID())
				return (*it)->setAction(action);
		}
	}
	else
	{
		list<Unit*>::iterator it;
		for(it = this->units.begin(); it != this->units.end(); it++)
		{
			if((*it)->id == action->getUnitID())
				return (*it)->setAction(action);
		}
	}

	return false;
}

bool Player::cancelAction(Object* obj)
{
	if(obj != NULL && (obj->isUnit() || obj->isBuilding()) && obj->getAction() != NULL )
	{
		if(obj->isUnit())
			return ((Unit*)obj)->cancelAction();
		else
			return ((Building*)obj)->cancelAction();
	}
	else
		return false;
}

/*
// Return units and building if it is inside.
list<Object*> Player::insideArea(vec2& leftTop, vec2& rightTop, vec2& rightBottom, vec2& leftBottom)
{
	list<Object*> ret;
	
	list<Unit*>::iterator it;
	for(it = this->units.begin(); it != this->units.end(); it++)
	{
		if((*it)->insideArea(leftTop, rightTop, rightBottom, leftBottom))
			ret.push_back(*it);
	}

	list<Building*>::iterator it2;
	for(it2 = this->buildings.begin(); it2 != this->buildings.end(); it2++)
	{
		if((*it2)->insideArea(leftTop, rightTop, rightBottom, leftBottom))
			ret.push_back(*it2);
	}
	
	return ret;
}

// Retorna objetos que intersecta con la recta.
list<Object*> Player::intersect(vec2& pos, vec2& dir)
{
	list<Object*> ret;
	
	list<Unit*>::iterator it;
	for(it = this->units.begin(); it != this->units.end(); it++)
	{
		if((*it)->intersect(pos, dir))
			ret.push_back(*it);
	}

	list<Building*>::iterator it2;
	for(it2 = this->buildings.begin(); it2 != this->buildings.end(); it2++)
	{
		if((*it2)->intersect(pos, dir))
			ret.push_back(*it2);
	}

	return ret;
}

// Return the near object.
Object* Player::near(vec2& pos)
{
	Object* ret = NULL;
	float bestDist = FLT_MAX;
	
	list<Unit*>::iterator it;
	for(it = this->units.begin(); it != this->units.end(); it++)
	{
		float dist = pos.dist((*it)->pos);
		if(dist < bestDist)
		{
			bestDist = dist;
			ret = *it;
		}
	}

	list<Building*>::iterator it2;
	for(it2 = this->buildings.begin(); it2 != this->buildings.end(); it2++)
	{
		float dist = pos.dist((*it)->pos);
		if(dist < bestDist)
		{
			bestDist = dist;
			ret = *it;
		}
	}

	return ret;
}

// Return colisioned object if the object colisioned with units or buildings.
Object* Player::checkColision(Object* obj)
{
	list<Unit*>::iterator it;
	for(it = this->units.begin(); it != this->units.end(); it++)
	{
		Object* c = (*it)->checkColision(obj);
		if(c != NULL)
		{
			this->sem->V();
			return c;
		}
	}
	
	list<Building*>::iterator it2;
	for(it2 = this->buildings.begin(); it2 != this->buildings.end(); it2++)
	{
		Object* c = (*it2)->checkColision(obj);
		if(c != NULL)
		{
			this->sem->V();
			return c;
		}
	}

	return NULL;
}
*/







	
	
	
