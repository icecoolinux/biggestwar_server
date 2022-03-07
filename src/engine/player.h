#ifndef _player_h_
#define _player_h_

class Player;

#include <list>
#include "../config.h"

#include "objects/building.h"
#include "objects/unit.h"
#include "vec2.h"

class Player
{
	private:
		list<Building*> buildings;
		list<Unit*> units;
		
		int minerals;
		int oil;
		
	public:
		char name[LEN_NAME];
		int equip;

		
		
		Player(char* name, int equip);
		~Player();
		
		// Get and set resources.
		int getMinerals();
		int getOil();
		void setMinerals(int m);
		void setOil(int o);
		
		void update(int ms);
		
		bool addUnit(Unit* unit);
		bool quiteUnit(Unit* unit);
		bool addBuilding(Building* building);
		bool quiteBuilding(Building* building);
		
		int getNumUnits();
		int getNumBuildings();
		
		void destroyArea(Area* area);
		
		// Return object's positions and their types.
		void getObjectsInfo(list<vec2> &pos, list<float> &radio, list<enum ObjectType> &types);
		
		// return true if action has been applied.
		bool setAction(Action* action);
		bool cancelAction(Object* obj);
		/*
		// Return units and building if it is inside.
		list<Object*> insideArea(vec2& leftTop, vec2& rightTop, vec2& rightBottom, vec2& leftBottom);
		
		// Retorna objetos que intersecta con la recta.
		list<Object*> intersect(vec2& pos, vec2& dir);
		
		// Return the near object.
		Object* near(vec2& pos);
		
		// Return colisioned object if the object colisioned with units or buildings.
		Object* checkColision(Object* obj);
		*/
};


#endif
