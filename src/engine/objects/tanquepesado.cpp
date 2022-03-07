
#include "tanquepesado.h"

TanquePesado::TanquePesado(ParamUnit& params): Soldado(params)
{
	this->fullLife = FULL_LIFE_TANQUEPESADO;
	this->vel = VEL_TANQUEPESADO;
	this->msToAttack = EVERY_MS_TO_ATTACK_TANQUEPESADO;
	this->minDistanceAttack = MIN_DISTANCE_TO_ATTACK_TANQUEPESADO;
	this->amountDamage = AMOUNT_DAMAGE_TANQUEPESADO;
}
