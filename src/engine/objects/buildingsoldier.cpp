
#include "soldado.h"
#include "buildingsoldier.h"
#include "../player.h"
#include "../collision/collision.h"

BuildingSoldier::BuildingSoldier(ParamBuilding& params): Building(params)
{
	this->tsSearchedNearEnemy = 0;
	this->msIdleToAttack = 0;
	this->tsLastAttack = 0;
}

bool BuildingSoldier::setAction(Action* action)
{
	if(action == NULL)
		return false;
	
	// Don't allow suicide :D
	if(action->getBuild2() != NULL && action->getBuild() == action->getBuild2())
		return false;
	
	// Must to indicate one enemy.
	if(action->getBuild2() == NULL && action->getUnit2() == NULL)
		return false;
	
	if(action->type == AT_ATACK && action->subType == AST_ATACK_GOTOATACK) { /* OK */ }
	else
		return false;
	
	if(this->action != NULL)
		delete this->action;
	
	this->action = action;
	return true;
}

bool BuildingSoldier::cancelAction()
{
	if(this->action == NULL)
		return false;
	
	this->removeAction();
	return true;
}

void BuildingSoldier::update(int ms)
{
	// Update ms left to attack.
	if(this->msIdleToAttack > 0)
	{
		this->msIdleToAttack -= ms;
		if(this->msIdleToAttack < 0)
			this->msIdleToAttack = 0;
	}
	
	if(this->action != NULL)
	{
		Action* action = this->action;
		
		if(action->type == AT_ATACK)
		{
			if(action->subType == AST_ATACK_GOTOATACK)
			{
				// "Go" to atack unit
				if(action->getUnit2() != NULL)
				{
					// I can atack it
					vec2 pos_ = action->getUnit2()->getPos();
					if( (this->getPos().dist(pos_) - this->radio - action->getUnit2()->radio + 1.0f) < this->minDistanceAttack)
						action->subType = AST_ATACK_ATACKING;
					// I dont reach it, then delete current action.
					else
						this->removeAction();
				}
				// "Go" to atack building
				else if(action->getBuild2() != NULL)
				{
					// I can atack it
					vec2 pos_ = action->getBuild2()->getPos();
					if( (this->getPos().dist(pos_) - this->radio - action->getBuild2()->radio + 1.0f) < this->minDistanceAttack)
						action->subType = AST_ATACK_ATACKING;
					// I dont reach it, then delete current action.
					else
						this->removeAction();
				}
			}
			else if(action->subType == AST_ATACK_ATACKING)
			{
				// First check if the enemy is near to attack it.
				float enemyDist = 0;
				if(action->getUnit2() != NULL)
				{
					vec2 pos_ = action->getUnit2()->getPos();
					enemyDist = this->getPos().dist(pos_) - this->radio - action->getUnit2()->radio + 1.0f;
				}
				else if(action->getBuild2() != NULL)
				{
					vec2 pos_ = action->getBuild2()->getPos();
					enemyDist = this->getPos().dist(pos_) - this->radio - action->getBuild2()->radio + 1.0f;
				}
				// I cant attack it, then remove the action
				if(enemyDist > this->minDistanceAttack)
				{
					this->removeAction();
				}
				// I can attack because is near.
				else
				{
					if(this->msIdleToAttack == 0)
					{
						bool wasAttack = false;
							
						if(action->getUnit2() != NULL)
						{
							wasAttack = true;
							
							Unit* unit2 = action->getUnit2();
							unit2->life -= this->amountDamage;
							
							// Unit destroyed.
							if(unit2->life <= 0.0f)
							{
								unit2->player->quiteUnit(unit2);

								// Object destructor will delete the action and set it to NULL.
								delete unit2;
							}
							// Notify to the victim that I'm attacking him.
							else
								unit2->beenAttacked(this);
						}
						// Atacking a building
						else if(action->getBuild2() != NULL)
						{
							wasAttack = true;
							
							action->getBuild2()->life -= this->amountDamage;
							
							// Building destroyed.
							if(action->getBuild2()->life <= 0.0f)
							{
								Building* build2 = action->getBuild2();
								build2->player->quiteBuilding(build2);
								
								// Object destructor will delete the action and set it to NULL.
								delete build2;
							}
						}
				
						if(wasAttack)
						{
							this->msIdleToAttack = this->msToAttack;
							this->tsLastAttack = Time::currentMs();
						}
					}
				}
			}
		}
	}
	// There's no action.
	else
	{
		// Find enemy object near (for each x ms for efficient).
		if( (Time::currentMs() - this->tsSearchedNearEnemy) > MS_TO_FIND_NEAR_ENEMIES)
		{
			vec2 pos_ = this->getPos();
			int enemyEquip = this->equip;
			Object* enemyObject = NULL;
			for(int i=0; i<EQUIPS-1; i++)
			{
				enemyEquip ++;
				enemyEquip %= EQUIPS;
				enemyObject = this->world->getCollision()->nearObjectType(pos_, OT_ALL, enemyEquip, VISIBILITY_DISTANCE, NULL, false);
				if(enemyObject != NULL)
					break;
			}
			this->tsSearchedNearEnemy = Time::currentMs();
			
			// There is an enemy object near, go to attack it.
			if(enemyObject != NULL)
			{
				struct ParamAction param;
				param.type = AT_ATACK;
				param.subType = AST_ATACK_GOTOATACK;
				param.buildID = this->id;
				param.build2ID = 0;
				param.unitID = 0;
				param.unit2ID = 0;
				if(enemyObject->isUnit())
					param.unit2ID = enemyObject->id;
				if(enemyObject->isBuilding())
					param.build2ID = enemyObject->id;
				param.objectMapID = 0;
				
				Action* a = new Action(param);
				a->setIds(this->world->getIds());
				if(!this->setAction(a))
					delete a;
			}
		}
	}
}




