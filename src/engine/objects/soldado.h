#ifndef _soldado_h_
#define _soldado_h_

class Soldado;

#include "unit.h"
#include "../action.h"
#include "../../time.h"

#define MS_TO_FIND_NEAR_ENEMIES 500

class Soldado: public Unit
{
	private:
		
		unsigned long long tsSearchedNearEnemy;
		
		// Indicate the ms that cant attack, when is zero then can attack again
		int msIdleToAttack;
		
	public:
		
		// Indicate when was the last attack, when it change then notice to the user it was an attack.
		unsigned long long tsLastAttack;
		
		
		float msToAttack;
		float minDistanceAttack;
		float amountDamage;
		
		Soldado(ParamUnit& params);
		
		bool setAction(Action* action);
		
		void update(int ms);
		
};

#endif
