
#include <stdio.h>
#include "mineral.h"
#include "../minimap.h"
#include "../world.h"

Mineral::Mineral(World* world, unsigned long long id, vec2& pos, int amount): Object(world, OT_MINERAL, id, -1, pos, RADIO_MINERAL)
{
	this->type = OT_MINERAL;
	this->amount = amount;
	for(int i=0; i<EQUIPS; i++)
		this->wasSeenByEquip[i] = false;
}

Mineral::~Mineral()
{
	
}

void Mineral::setAmount(int amount)
{
	this->amount = amount;
}

void Mineral::update(int ms)
{
	
}
