
#include "torreta.h"

Torreta::Torreta(ParamBuilding& params): BuildingSoldier(params)
{
	this->fullLife = FULL_LIFE_TORRETA;
	this->msToAttack = EVERY_MS_TO_ATTACK_TORRETA;
	this->minDistanceAttack = MIN_DISTANCE_TO_ATTACK_TORRETA;
	this->amountDamage = AMOUNT_DAMAGE_TORRETA;
}
