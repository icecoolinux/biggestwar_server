

#include "soldadoraso.h"

SoldadoRaso::SoldadoRaso(ParamUnit& params): Soldado(params)
{
	this->fullLife = FULL_LIFE_SOLDADORASO;
	this->vel = VEL_SOLDADORASO;
	this->msToAttack = EVERY_MS_TO_ATTACK_SOLDADORASO;
	this->minDistanceAttack = MIN_DISTANCE_TO_ATTACK_SOLDADORASO;
	this->amountDamage = AMOUNT_DAMAGE_SOLDADORASO;
}
