#ifndef _players_h_
#define _players_h_

#include "../defs.h"
#include "../config.h"
#include <list>

using namespace std;
	
class Players
{
	private:
		
		list<Player*> players[EQUIPS];
		
	public:
		
		Players();
		~Players();

		bool add(char* name, World* world, int &xInit, int &yInit, int &equip);
		bool remove(int equip, char* name);
		int clearDestroyedPlayers(list<char*> &names, list<int> &equips);
		
		// This function is safe because wouldn't must be call it new neither remove players.
		void getPlayers(list<char*> old, list<char*> &add, list<int> &addEquip, list<char*> &remove);
		
		bool getResourcesPlayer(int equip, char* name, int &mineral, int &oil);
		
		int getAmountPlayers();
		int getAmountPlayersByEquip(int equip);
		
		void destroyArea(Area* area);
		
		// Return object's positions and their types.
		void getObjectsInfo(list<vec2> &pos, list<float> &radio, list<enum ObjectType> &types, list<char*> &playerName, int equip);
		
		void update(int ms);
		
		// Intent to execute an action, return true if succesfull.
		bool setAction(int equip, char* name, Action* action);
		bool cancelAction(int equip, char* name, Object* obj);
};


#endif
