#ifndef _mineral_h_
#define _mineral_h_

class Mineral;

#include "../../defs.h"
#include "../../config.h"
#include "../vec2.h"
#include "object.h"

class Mineral: public Object
{
	private:
		
		
		
	public:
		
		float amount;
		
		// Set true when a object player see the mineral, then all other players from the same equip can see the mineral too.
		bool wasSeenByEquip[EQUIPS];
		
		
		Mineral(World* world, unsigned long long id, vec2& pos, int amount);
		~Mineral();
		
		void setAmount(int amount);

		void update(int ms);
};


#endif
