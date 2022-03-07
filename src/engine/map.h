#ifndef _map_h_
#define _map_h_

class Map;

#include <list>
#include <math.h>
#include "../defs.h"
#include "../config.h"
#include "vec2.h"
#include "../time.h"
#include <stdlib.h>
#include <stdio.h>

using namespace std;

#define MS_UPDATE_WAS_SEEN_MINERAL 200


class Map
{
	private:
		
		World* world;
		
		Terrain* terrain;
		list<Mineral*> minerals;
		
		bool usingZones;
		
		vec2 bases[EQUIPS][MAX_PLAYERS_BY_EQUIP];
		bool occupedBases[EQUIPS][MAX_PLAYERS_BY_EQUIP];
		
		unsigned long long timeLastMineral;
		
		void makeMineral(World* world, vec2& posMineral);
		
	public:
		
		Map(World* world);
		~Map();
		
		Terrain* getTerrain();
		
		void generate();
		
		// The zone wont use more
		void clearZones();
		
		bool updateZones(int myEquip, bool (&enabledBasesZones)[EQUIPS][MAX_PLAYERS_BY_EQUIP], list<bool> &isAddBaseZone, list<int> &equipBaseZone, list<vec2> &posBaseZone, list<int> &radioBaseZone);
		
		// Return object's positions and their types.
		void getObjectsInfo(list<vec2> &pos, list<enum ObjectType> &types, list<bool>* mapObjectWasSeenByEquip);
		
		// Return true if I can build here or false if it's a base zone.
		bool canBuild(vec2 pos, float radio, int myEquip);
		
		// Return pos for new player, with minerals near.
		vec2 getPosNewPlayer(int equip);

		bool quiteMineral(unsigned long long id);
		
		void update(int ms);
};

#endif
