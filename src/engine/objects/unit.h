#ifndef _unit_h_
#define _unit_h_

#include "../../defs.h"
#include "../vec2.h"
#include "object.h"
#include "../../config.h"
#include <stdio.h>

struct ParamUnit {
	World* world;
	Player* player;
	enum ObjectType type;
	unsigned long long id;
	vec2 pos;
	float radio;
	int life;
	int creada;
};
		

class Unit: public Object
{
	protected:
		float altura;
		float vel;
		
		// Path to move.
		vec2 path[MAX_PATH_MOVE_UNIT];
		int amountPath;
		int posPath;
		
		// Time that collisioned, this prevent move to the eternity.
		unsigned long int timeThatCollisioned;
		
		// Move to position, keep a minDist.
		// Return 0: if not reach de pos yet.
		// Return 1: if reach de pos with a min distance.
		// Return -1: Error, the pos is far.
		// Return -2: Error, there isn't path to pos.
		// Return -3: Error, there is an obstacule.
		int moveTo(vec2& pos, float minDist, Object* goalObject, int ms);
		
	public:
		
		Player* player;
		float life;
		float fullLife;
		float creada;
		
		Unit(ParamUnit& params);
		~Unit();
		
		virtual bool setAction(Action* action);
		virtual bool cancelAction();
		virtual void update(int ms);
		virtual void beenAttacked(Soldado* soldier);
		virtual void beenAttacked(BuildingSoldier* bs);
		
		// Return 1 if reach the target, 0 if not or negative if can't reach.
		int moveToPos(vec2& pos, int ms);
		
		// Return 1 if reach the target, 0 if not or negative if can't reach.
		int moveToObject(Object* object, int ms);
};

#endif

