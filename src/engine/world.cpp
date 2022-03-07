
#include "world.h"
#include "map.h"
#include "players.h"
#include "collision/collision.h"
#include "minimap.h"
#include "ids.h"
#include "../view/playerview.h"
#include <string.h>
#include "action.h"
#include "area.h"

World::World(int number)
{
	this->number = number;
	
	this->area = new Area();
	
	this->ids = new IDS(1);
	this->map = new Map(this);
	this->players = new Players();
	this->minimap = new MiniMap(this->players, this->map);
	this->collision = new Collision(this->map->getTerrain());
	
	this->map->generate();
}

World::~World()
{
	delete this->area;
	delete this->map;
	delete this->players;
	delete this->ids;
	delete this->collision;
	delete this->minimap;
}

bool World::save(char* data, size_t max, int &pos)
{
	return false;
}

int World::getNumber()
{
	return this->number;
}

bool World::newPlayer(char* name, int &xInit, int &yInit, int &equip)
{
	return this->players->add(name, this, xInit, yInit, equip);
}

bool World::removePlayer(int equip, char* name)
{
	return this->players->remove(equip, name);
}

int World::clearDestroyedPlayers(list<char*> &names, list<int> &equips)
{
	return this->players->clearDestroyedPlayers(names, equips);
}

int World::getAmountPlayers()
{
	return players->getAmountPlayers();
}

int World::getAmountPlayersByEquip(int equip)
{
	return players->getAmountPlayersByEquip(equip);
}

void World::getPlayers(list<char*> old, list<char*> &add, list<int> &addEquip, list<char*> &remove)
{
	this->players->getPlayers(old, add, addEquip, remove);
}

bool World::getResourcesPlayer(int equip, char* name, int &mineral, int &oil)
{
	return this->players->getResourcesPlayer(equip, name, mineral, oil);
}

void World::update(int ms)
{
	this->area->update(ms);
	this->map->update(ms);
	this->players->update(ms);
	this->players->destroyArea(this->area);
	this->minimap->update(ms);
}

// Intent to execute an action, return true if successful.
bool World::setAction(int equip, char* name, Action* action)
{
	if(!action->setIds(this->ids))
		return false;

	bool ok = this->players->setAction(equip, name, action);

	if(!ok)
		delete action;
	
	return ok;
}

bool World::cancelAction(int equip, char* name, unsigned long long idObject)
{
	if(idObject > 0)
	{
		Object* obj = this->ids->getObject(idObject);
		return this->players->cancelAction(equip, name, obj);
	}
	else
		return false;
}

void World::removeBasesZones()
{
	this->map->clearZones();
}
		
void World::startToCloseArea()
{
	this->area->start();
}

bool World::isAreaClosing()
{
	return this->area->isClosing();
}

void World::get(bool &isClosing, int &msLeft, 
				vec2 &futureCenter, int &currentBottom, int &currentTop, int &currentLeft, int &currentRight,
				int &futureBottom, int &futureTop, int &futureLeft, int &futureRight,
				float &speedCloseSecBottom, float &speedCloseSecTop, float &speedCloseSecLeft, float &speedCloseSecRight)
{
	this->area->get(isClosing, msLeft, 
					futureCenter, currentBottom, currentTop, currentLeft, currentRight, 
					futureBottom, futureTop, futureLeft, futureRight, 
					speedCloseSecBottom, speedCloseSecTop, speedCloseSecLeft, speedCloseSecRight);
}

unsigned long long World::getNewId()
{
	return this->ids->getNew();
}

void World::setObjectId(unsigned long long id, Object* obj)
{
	this->ids->setObject(id, obj);
}

void World::unsetObjectId(unsigned long long id, Object* obj)
{
	this->ids->free(id);
}

Object* World::getObjectById(unsigned long long id)
{
	return this->ids->getObject(id);
}

Collision* World::getCollision()
{
	return this->collision;
}

MiniMap* World::getMiniMap()
{
	return this->minimap;
}

Map* World::getMap()
{
	return this->map;
}

IDS* World::getIds()
{
	return this->ids;
}


