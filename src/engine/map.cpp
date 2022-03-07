
#include "map.h"
#include "terrain.h"
#include "objects/mineral.h"
#include "collision/collision.h"
#include "world.h"

Map::Map(World* world)
{
	this->world = world;
	this->terrain = new Terrain();
	this->usingZones = true;
}

Map::~Map()
{
	delete this->terrain;
}

Terrain* Map::getTerrain()
{
	return this->terrain;
}

void Map::generate()
{
	// Put bases zones.
	for(int i=0; i<EQUIPS; i++)
	{
		for(int j=0; j<MAX_PLAYERS_BY_EQUIP; j++)
		{
			this->occupedBases[i][j] = false;

			// Until put base correctly.
			while(true)
			{
				float radio = ((float) rand() / ((float)RAND_MAX)) * RADIO_ZONE_EQUIP;
				float angle = ((float) rand() / ((float)RAND_MAX)) * (M_PI/2.0f);

				if(i==0)
				{
					this->bases[i][j].x = cos(angle)*radio + 10.0f;
					this->bases[i][j].y = SIDE_MAP_METERS - sin(angle)*radio - 10.0f;
				}
				else if(i==1)
				{
					this->bases[i][j].x = cos(angle)*radio + 10.0f;
					this->bases[i][j].y = sin(angle)*radio + 10.0f;
				}
				else
				{
					this->bases[i][j].x = SIDE_MAP_METERS - cos(angle)*radio - 10.0f;
					this->bases[i][j].y = sin(angle)*radio + 10.0f;
				}
				
				// Check if not collisined with another base.
				bool ok = true;
				for(int k=0; k<j; k++)
				{
					if( this->bases[i][k].dist(this->bases[i][j]) < RADIO_INIT_BASE_ZONE*2.0f)
					{
						ok = false;
						break;
					}
				}
				if(ok)
					break;
			}
		}
	}

	// Make a lot of minerals.
	int mineralsToMake = MAX_MINERALS_MAP - AMOUNT_MINERALS_BY_BASE*MAX_PLAYERS_BY_EQUIP*EQUIPS;
	while(mineralsToMake > 0)
	{
		// Put minerals in a cumulo.
		vec2 posMineral[MAX_MINERALS_BY_CUMULO];
		int amountCupulo = rand() % (MAX_MINERALS_BY_CUMULO-MIN_MINERALS_BY_CUMULO);
		amountCupulo += (MIN_MINERALS_BY_CUMULO+1);
		for(int k=0; k<amountCupulo; k++)
		{
			// Put one mineral, try 20 times
			int triesToPut = 20;
			while(triesToPut > 0)
			{
				triesToPut--;
				
				// The first mineral set the cumulo position.
				if(k == 0)
					posMineral[k].setRandom(RADIO_MINERAL, SIDE_MAP_METERS-RADIO_MINERAL, RADIO_MINERAL, SIDE_MAP_METERS-RADIO_MINERAL);
				else
					posMineral[k].setRandom(posMineral[0].x-RADIO_CUMULO_MINERALS, posMineral[0].x+RADIO_CUMULO_MINERALS, posMineral[0].y-RADIO_CUMULO_MINERALS, posMineral[0].y+RADIO_CUMULO_MINERALS);

				// It's out of the cumulo zone.
				if(k > 0 && posMineral[0].dist(posMineral[k]) > RADIO_CUMULO_MINERALS)
					continue;
				
				// It's out of the map
				if( posMineral[k].x < RADIO_MINERAL || posMineral[k].x > (SIDE_MAP_METERS-RADIO_MINERAL) || posMineral[k].y < RADIO_MINERAL || posMineral[k].y > (SIDE_MAP_METERS-RADIO_MINERAL) )
					continue;
				
				// It's over other mineral (or to close)
				bool isOver = false;
				for(int n=0; n<k; n++)
				{
					if(posMineral[k].dist(posMineral[n]) < (RADIO_MINERAL*2.0f + 2.0f*RADIO_TANQUEPESADO +0.5f) )
					{
						isOver = true;
						break;
					}
				}
				if(isOver)
					continue;
				
				// There is in a base zone, try newly.
				if(!canBuild(posMineral[k], RADIO_MINERAL, -1))
					continue;
			
				// Make the mineral.
				makeMineral(world, posMineral[k]);
				mineralsToMake--;
				
				break;
			}
			
			// Fail to put minerals.
			if(triesToPut <= 0)
				break;
				
			// Reach the max minerals
			if(mineralsToMake <= 0)
				break;
		}
	}
	this->timeLastMineral = Time::currentMs();
}

// The zone wont use more
void Map::clearZones()
{
	this->usingZones = false;
}

bool Map::updateZones(int myEquip, bool (&enabledBasesZones)[EQUIPS][MAX_PLAYERS_BY_EQUIP], list<bool> &isAddBaseZone, list<int> &equipBaseZone, list<vec2> &posBaseZone, list<int> &radioBaseZone)
{
	bool theresChange = false;
	
	// Delete all
	if(!this->usingZones)
	{
		for(int e=0; e<EQUIPS; e++)
		{
			for(int i=0; i<MAX_PLAYERS_BY_EQUIP; i++)
			{
				if(enabledBasesZones[e][i])
				{
					theresChange = true;
					enabledBasesZones[e][i] = false;
					
					isAddBaseZone.push_back(false);
					equipBaseZone.push_back(e);
					posBaseZone.push_back(this->bases[e][i]);
					radioBaseZone.push_back((int)RADIO_INIT_BASE_ZONE);
				}
			}
		}
	}
	else
	{
		for(int e=0; e<EQUIPS; e++)
		{
			for(int i=0; i<MAX_PLAYERS_BY_EQUIP; i++)
			{
				if(enabledBasesZones[e][i] == this->occupedBases[e][i])
				{
					// If it's a del zone of the an enemy then I dont notify
					if( e != myEquip && this->occupedBases[e][i])
					{
					}
					else
					{
						theresChange = true;
						enabledBasesZones[e][i] = !this->occupedBases[e][i];
						
						// Is add, the zone is enable
						if(enabledBasesZones[e][i])
							isAddBaseZone.push_back(true);
						// Delete zone, it's disabled
						else
							isAddBaseZone.push_back(false);
						equipBaseZone.push_back(e);
						posBaseZone.push_back(this->bases[e][i]);
						radioBaseZone.push_back((int)RADIO_INIT_BASE_ZONE);
					}
				}
			}
		}
	}
	
	return theresChange;
}


// Return object's positions and their types.
void Map::getObjectsInfo(list<vec2> &pos, list<enum ObjectType> &types, list<bool>* mapObjectWasSeenByEquip)
{
	list<Mineral*>::iterator it;
	for(it = this->minerals.begin(); it != this->minerals.end(); it++)
	{
		pos.push_back( (*it)->getPos());
		types.push_back( (*it)->type);
		for(int i=0; i<EQUIPS; i++)
			mapObjectWasSeenByEquip[i].push_back( (*it)->wasSeenByEquip[i] );
	}
}

// Return true if I can build here or false if it's a base zone.
bool Map::canBuild(vec2 pos, float radio, int myEquip)
{
	if(!this->usingZones)
		return true;
	
	int equip = -1;
	// Zone 0.
	if(pos.x <= (RADIO_ZONE_EQUIP + RADIO_INIT_BASE_ZONE + 20.0f) && pos.y >= (SIDE_MAP_METERS - RADIO_ZONE_EQUIP - RADIO_INIT_BASE_ZONE - 20.0f) )
		equip = 0;
	// Zone 1.
	else if(pos.x <= (RADIO_ZONE_EQUIP + RADIO_INIT_BASE_ZONE + 20.0f) && pos.y <= (RADIO_ZONE_EQUIP + RADIO_INIT_BASE_ZONE + 20.0f) )
		equip = 1;
	// Zone 2.
	else if(pos.x >= (SIDE_MAP_METERS - RADIO_ZONE_EQUIP - RADIO_INIT_BASE_ZONE - 20.0f) && pos.y <= (RADIO_ZONE_EQUIP + RADIO_INIT_BASE_ZONE + 20.0f) )
		equip = 2;
	
	if(equip >= 0)
	{
		for(int i=0; i<MAX_PLAYERS_BY_EQUIP; i++)
		{
			bool isOn = this->bases[equip][i].dist(pos) <= (radio+RADIO_INIT_BASE_ZONE + 1.0f);
			if( isOn && (equip != myEquip || !this->occupedBases[equip][i]) )
				return false;
		}
	}
		
	return true;
}

// Return pos for new player, with minerals near.
vec2 Map::getPosNewPlayer(int equip)
{
/*
if(equip == 0)
{
	return vec2(1000, 1000);
}
*/
	Object* objects[200];
	
	for(int i=0; i<MAX_PLAYERS_BY_EQUIP; i++)
	{
		// If there's occuped then continue
		if(this->occupedBases[equip][i])
			continue;
		
		// Check that there's not any building in the zone.
		int amount = this->world->getCollision()->getObjects(this->bases[equip][i], RADIO_INIT_BASE_ZONE, objects, 200);
		bool thereisBuild = false;
		for(int j=0; j<amount; j++)
		{
			if(objects[j]->isBuilding())
			{
				thereisBuild = true;
				break;
			}
		}
		if(!thereisBuild)
		{
			// I occup the base.
			this->occupedBases[equip][i] = true;
			
			float xBase = this->bases[equip][i].x;
			float yBase = this->bases[equip][i].y;
			
			// Put minerals to the player in the zone.
			vec2 posMineral[AMOUNT_MINERALS_BY_BASE];
			for(int k=0; k<AMOUNT_MINERALS_BY_BASE; k++)
			{
				// Put one mineral, try 30 times
				int triesToPut = 30;
				while(triesToPut > 0)
				{
					triesToPut--;
					
					posMineral[k].setRandom(xBase-RADIO_INIT_BASE_ZONE, xBase+RADIO_INIT_BASE_ZONE, yBase-RADIO_INIT_BASE_ZONE, yBase+RADIO_INIT_BASE_ZONE);
					
					// It's out of the base zone.
					if(this->bases[equip][i].dist(posMineral[k]) > RADIO_INIT_BASE_ZONE)
						continue;
					
					// It's too close to the base.
					if(this->bases[equip][i].dist(posMineral[k]) < 3.4f*RADIO_BASE )
						continue;
					
					// It's out of the map
					if( posMineral[k].x < RADIO_MINERAL || posMineral[k].x > (SIDE_MAP_METERS-RADIO_MINERAL) || posMineral[k].y < RADIO_MINERAL || posMineral[k].y > (SIDE_MAP_METERS-RADIO_MINERAL) )
						continue;
					
					// It's over other mineral
					bool isOver = false;
					for(int n=0; n<k; n++)
					{
						if(posMineral[k].dist(posMineral[n]) < (RADIO_MINERAL*2.0f +2.0f*RADIO_RECOLECTOR +0.5f) )
						{
							isOver = true;
							break;
						}
					}
					if(isOver)
						continue;
					
					// Make the mineral.
					makeMineral(world, posMineral[k]);
					
					break;
				}
				
				// Fail to put minerals.
				if(triesToPut <= 0)
					break;
			}
			
			// Return base position
			return this->bases[equip][i];
		}
	}
}

bool Map::quiteMineral(unsigned long long id)
{
	list<Mineral*>::iterator it;
	for(it = this->minerals.begin(); it != this->minerals.end(); it++)
	{
		if((*it)->id == id)
		{
			this->minerals.erase(it);
			return true;
		}
	}
	return false;
}

void Map::update(int ms)
{
	// It's time to make a mineral.
	if(this->minerals.size() < MAX_MINERALS_MAP && (Time::currentMs()-this->timeLastMineral) > MS_TO_NEW_MINERAL)
	{
		vec2 posMineral;
		
		// Try until it isn't in a base zone.
		while(true)
		{
			posMineral.setRandom(RADIO_MINERAL, SIDE_MAP_METERS-RADIO_MINERAL, RADIO_MINERAL, SIDE_MAP_METERS-RADIO_MINERAL);
			
			if(canBuild(posMineral, RADIO_MINERAL, -1))
				break;
		}
			
		Object* obj = this->world->getCollision()->checkCollision(posMineral, RADIO_MINERAL, NULL);
		if(obj != NULL)
		{
			vec2 posObj = obj->getPos();
			posMineral = this->world->getCollision()->getPosCollisionFree(posObj, obj->radio, RADIO_MINERAL);
		}
		
		// Make the mineral.
		makeMineral(this->world, posMineral);
		
		this->timeLastMineral = Time::currentMs();
	}
}

void Map::makeMineral(World* world, vec2& posMineral)
{
	unsigned long long idMineral = world->getNewId();
	int amount = ((float) rand() / ((float)RAND_MAX)) * ((float)(MAX_AMOUNT_MINERAL_BY_ONE - MIN_AMOUNT_MINERAL_BY_ONE));
	amount += MIN_AMOUNT_MINERAL_BY_ONE;
	
	Mineral* mineral = new Mineral(world, idMineral, posMineral, amount);
	this->minerals.push_back(mineral);
	
	world->setObjectId(idMineral, mineral);
}
