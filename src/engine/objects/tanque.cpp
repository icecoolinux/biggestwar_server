
#include "tanque.h"

Tanque::Tanque(ParamUnit& params): Soldado(params)
{
	this->fullLife = FULL_LIFE_TANQUE;
	this->vel = VEL_TANQUE;
	this->msToAttack = EVERY_MS_TO_ATTACK_TANQUE;
	this->minDistanceAttack = MIN_DISTANCE_TO_ATTACK_TANQUE;
	this->amountDamage = AMOUNT_DAMAGE_TANQUE;
}
