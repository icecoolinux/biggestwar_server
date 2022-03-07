#ifndef _world_h_
#define _world_h_

#include "../defs.h"
#include "../config.h"
#include <list>

using namespace std;

class World {
	private:
		
		int number;
		
		IDS* ids;
		Collision* collision;
		MiniMap* minimap;
		Map* map;
		Players* players;
		
		Area* area;
		
	public:
		
		World(int number);
		~World();
	
		bool save(char* data, size_t max, int &pos);
		
		int getNumber();
		
		bool newPlayer(char* name, int &xInit, int &yInit, int &equip);
		bool removePlayer(int equip, char* name);
		int clearDestroyedPlayers(list<char*> &names, list<int> &equips);
		
		int getAmountPlayers();
		int getAmountPlayersByEquip(int equip);
		void getPlayers(list<char*> old, list<char*> &add, list<int> &addEquip, list<char*> &remove);
		bool getResourcesPlayer(int equip, char* name, int &mineral, int &oil);
		
		void update(int ms);
		
		// Intent to execute an action, return true if succesfull.
		bool setAction(int equip, char* name, Action* action);
		bool cancelAction(int equip, char* name, unsigned long long idObject);
		
		void removeBasesZones();
		
		void startToCloseArea();
		bool isAreaClosing();
		void get(bool &isClosing, int &msLeft, 
				vec2 &futureCenter, int &currentBottom, int &currentTop, int &currentLeft, int &currentRight,
				int &futureBottom, int &futureTop, int &futureLeft, int &futureRight,
				float &speedCloseSecBottom, float &speedCloseSecTop, float &cspeedCloseSecLeft, float &speedCloseSecRight);
		
		unsigned long long getNewId();
		void setObjectId(unsigned long long id, Object* obj);
		void unsetObjectId(unsigned long long id, Object* obj);
		Object* getObjectById(unsigned long long id);
		
		Collision* getCollision();
		MiniMap* getMiniMap();
		Map* getMap();
		IDS* getIds();
};

#endif
