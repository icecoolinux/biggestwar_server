#ifndef _object_h_
#define _object_h_

#include "../../defs.h"
#include "../vec2.h"

class Object {
	protected:
		// It's private for the collision system.
		vec2 pos;
		
		Action* action;
		
	public:
		
		World* world;
		enum ObjectType type;
		unsigned long long id;
		int equip; // Map object has -1.
		float radio;
		
		Object(World* world, enum ObjectType type, unsigned long long id, int equip, vec2& pos, float radio);
		~Object();
		
		bool isUnit();
		bool isBuilding();
		
		// Return player name (not delete!) or NULL if is a map object.
		char* getPlayerName();
		
		Action* getAction();
		void removeAction();
		
		virtual vec2 getPos();
		virtual void setPos(vec2& p);
};

#endif
