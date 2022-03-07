

#include "soldadoentrenado.h"


SoldadoEntrenado::SoldadoEntrenado(ParamUnit& params): Soldado(params)
{
	this->fullLife = FULL_LIFE_SOLDADOENTRENADO;
	this->vel = VEL_SOLDADOENTRENADO;
	this->msToAttack = EVERY_MS_TO_ATTACK_SOLDADOENTRENADO;
	this->minDistanceAttack = MIN_DISTANCE_TO_ATTACK_SOLDADOENTRENADO;
	this->amountDamage = AMOUNT_DAMAGE_SOLDADOENTRENADO;
}
