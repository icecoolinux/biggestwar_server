#ifndef _buildingsoldier_h_
#define _buildingsoldier_h_

class BuildingSoldier;

#include "building.h"
#include "../action.h"
#include "../../time.h"

class BuildingSoldier: public Building
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
		
		
		BuildingSoldier(ParamBuilding& params);
		
		virtual bool setAction(Action* action);
		virtual bool cancelAction();
		virtual void update(int ms);
		
};

#endif
