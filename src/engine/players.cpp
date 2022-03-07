
#include "players.h"
#include "../semaphore.h"
#include "player.h"
#include "map.h"
#include "collision/collision.h"
#include "objects/base.h"
#include "../view/playerview.h"
#include <string.h>

Players::Players()
{
	
}

Players::~Players()
{
	for(int i=0; i<EQUIPS; i++)
	{
		while(this->players[i].size() > 0)
		{
			Player* p = this->players[i].front();
			this->players[i].pop_front();
			delete p;
		}
	}
}

bool Players::add(char* name, World* world, int &xInit, int &yInit, int &equip)
{
	bool ok = true;

	// Find the equip more leak.
	equip = 0;
	for(int i=1; i<EQUIPS; i++)
	{
		if(this->players[i].size() < this->players[equip].size())
			equip = i;
	}

	if(this->players[equip].size() >= MAX_PLAYERS_BY_EQUIP)
		ok = false;
	else
	{
		// Check is name is unique.
		for(int i=0; i<EQUIPS; i++)
		{
			list<Player*>::iterator it;
			for(it = this->players[i].begin(); it != this->players[i].end(); it++)
			{
				if(strcmp(name, (*it)->name) == 0)
				{
					ok = false;
					break;
				}
			}
			if(!ok)
				break;
		}
	
		if(ok)
		{
			Player* p = new Player(name, equip);
			this->players[equip].push_back(p);

			// Get position of the player
			vec2 posInit = world->getMap()->getPosNewPlayer(equip);
			
			// Destroy units or buildings on the base space.
			Object* objects[100];
			int cantObj = world->getCollision()->getObjects(posInit, RADIO_BASE+0.02f, &(objects[0]), 100);
			for(int i=0; i<cantObj; i++)
			{
				// Destroy.
				if(objects[i]->isUnit())
				{
					Unit* u = (Unit*)objects[i];
					u->player->quiteUnit(u);
					delete u;
				}
				else if(objects[i]->isBuilding())
				{
					Building* b = (Building*)objects[i];
					b->player->quiteBuilding(b);
					delete b;
				}
			}
			
			// Add the first base.
			ParamBuilding params;
			params.world = world;
			params.player = p;
			params.type = OT_BASE;
			params.id = world->getNewId();
			params.pos = posInit;
			params.radio = RADIO_BASE;
			params.life = FULL_LIFE_BASE;
			params.creada = FULL_LIFE_BASE;

			Building* base = new Base(params);

			world->setObjectId(params.id, base);
			p->addBuilding(base);
			
			xInit = posInit.x;
			yInit = posInit.y;
		}
	}
	
	return ok;
}

bool Players::remove(int equip, char* name)
{
	bool ok = false;

	list<Player*>::iterator it;
	for(it = this->players[equip].begin(); it != this->players[equip].end(); it++)
	{
		if(strcmp(name, (*it)->name) == 0)
		{
			Player* p = *it;
			this->players[equip].erase(it);
			delete p;
			ok = true;
			break;
		}
	}

	return ok;
}

int Players::clearDestroyedPlayers(list<char*> &names, list<int> &equips)
{
	names.clear();
	equips.clear();
	
	for(int e=0; e<EQUIPS; e++)
	{
		list<Player*>::iterator it;
		for(it = this->players[e].begin(); it != this->players[e].end(); it++)
		{
			if( (*it)->getNumUnits() == 0 && (*it)->getNumBuildings() == 0)
			{
				char* name = new char[LEN_NAME+1];
				strcpy(name, (*it)->name);
				names.push_back(name);
				equips.push_back(e);
			}
		}
	}
	
	list<char*>::iterator itName = names.begin();
	list<int>::iterator itEquip = equips.begin();
	while(itName != names.end())
	{
		this->remove(*itEquip, *itName);
		itName++;
		itEquip++;
	}
	
	return names.size();
}

void Players::getPlayers(list<char*> old, list<char*> &add, list<int> &addEquip, list<char*> &remove)
{
	// Is new
	for(int equip=0; equip<EQUIPS; equip++)
	{
		list<Player*>::iterator it;
		for(it = this->players[equip].begin(); it != this->players[equip].end(); it++)
		{
			bool isNew = true;
			list<char*>::iterator itOld;
			for(itOld = old.begin(); itOld != old.end(); itOld++)
			{
				if(strcmp(*itOld, (*it)->name) == 0)
				{
					isNew = false;
					break;
				}
			}
			if(isNew)
			{
				char* name = new char[strlen((*it)->name)+1];
				strcpy(name, (*it)->name);
				add.push_back(name);
				addEquip.push_back(equip);
			}
		}
	}
	
	// Is removed
	list<char*>::iterator itOld;
	for(itOld = old.begin(); itOld != old.end(); itOld++)
	{
		bool isRemoved = true;
		for(int equip=0; equip<EQUIPS; equip++)
		{
			list<Player*>::iterator it;
			for(it = this->players[equip].begin(); it != this->players[equip].end(); it++)
			{
				if(strcmp(*itOld, (*it)->name) == 0)
				{
					isRemoved = false;
					break;
				}
			}
			if(!isRemoved)
				break;
		}
		if(isRemoved)
		{
			char* name = new char[strlen(*itOld)+1];
			strcpy(name, *itOld);
			remove.push_back(name);
		}
	}
}

bool Players::getResourcesPlayer(int equip, char* name, int &mineral, int &oil)
{
	list<Player*>::iterator it;
	for(it = this->players[equip].begin(); it != this->players[equip].end(); it++)
	{
		Player* p = *it;
		if(strcmp(p->name, name)==0)
		{
			mineral = p->getMinerals();
			oil = p->getOil();
			return true;
		}
	}
	return false;
}

int Players::getAmountPlayersByEquip(int equip)
{
	return this->players[equip].size();
}

int Players::getAmountPlayers()
{
	int amount = 0;
	for(int i=0; i<EQUIPS; i++)
		amount += this->players[i].size();
	return amount;
}

void Players::destroyArea(Area* area)
{
	for(int i=0; i<EQUIPS; i++)
	{
		list<Player*>::iterator it;
		for(it = this->players[i].begin(); it != this->players[i].end(); it++)
			(*it)->destroyArea(area);
	}
}

// Return object's positions and their types.
void Players::getObjectsInfo(list<vec2> &pos, list<float> &radio, list<enum ObjectType> &types, list<char*> &playerName, int equip)
{
	list<Player*>::iterator it;
	for(it = this->players[equip].begin(); it != this->players[equip].end(); it++)
	{
		int sizeBefore = pos.size();
		
		Player* player = *it;
		player->getObjectsInfo(pos, radio, types);
		
		for(int j=sizeBefore; j<pos.size(); j++)
			playerName.push_back( &(player->name[0]));
	}
}

void Players::update(int ms)
{
	for(int i=0; i<EQUIPS; i++)
	{
		list<Player*>::iterator it;
		for(it = this->players[i].begin(); it != this->players[i].end(); it++)
			(*it)->update(ms);
	}
}

// Intent to execute an action, return true if succesfull.
bool Players::setAction(int equip, char* name, Action* action)
{
	bool ok = false;
	
	list<Player*>::iterator it;
	for(it = this->players[equip].begin(); it != this->players[equip].end(); it++)
	{
		Player* p = *it;
		if(strcmp(p->name, name)==0)
		{
			ok = p->setAction(action);
			break;
		}
	}

	return ok;
}

bool Players::cancelAction(int equip, char* name, Object* obj)
{
	if(obj == NULL)
		return false;
	
	Player* player = NULL;
	if(obj->isUnit())
		player = ((Unit*)obj)->player;
	if(obj->isBuilding())
		player = ((Building*)obj)->player;
	
	if(player != NULL)
		return player->cancelAction(obj);
	else
		return false;
}

		
