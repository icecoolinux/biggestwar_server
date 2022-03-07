
#include "soldado.h"
#include "building.h"
#include "../player.h"
#include "../collision/collision.h"

Soldado::Soldado(ParamUnit& params): Unit(params)
{
	this->tsSearchedNearEnemy = 0;
	this->msIdleToAttack = 0;
	this->tsLastAttack = 0;
}

bool Soldado::setAction(Action* action)
{
	if(action == NULL)
		return false;
	
	if(action->type == AT_MOVE)
		return Unit::setAction(action);
	
	// Don't allow suicide :D
	if(action->getUnit2() != NULL && action->getUnit() == action->getUnit2())
		return false;
	
	if(action->type == AT_ATACK && action->subType == AST_ATACK_GOTOATACK) { /* OK */ }
	else
		return false;
	
	if(this->action != NULL)
		delete this->action;
	
	this->action = action;
	return true;
}

void Soldado::update(int ms)
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
		
		if(action->type == AT_MOVE)
			Unit::update(ms);
		else
		{
			if(action->type == AT_ATACK)
			{
				if(action->subType == AST_ATACK_GOTOATACK)
				{
					// Go to atack unit
					if(action->getUnit2() != NULL)
					{
						this->moveToObject(action->getUnit2(), ms);
						
						// I can atack it
						vec2 pos_ = action->getUnit2()->getPos();
						if( (this->getPos().dist(pos_) - this->radio - action->getUnit2()->radio + 1.0f) < this->minDistanceAttack)
							action->subType = AST_ATACK_ATACKING;
					}
					// Go to atack building
					else if(action->getBuild2() != NULL)
					{
						this->moveToObject(action->getBuild2(), ms);

						// I can atack it
						vec2 pos_ = action->getBuild2()->getPos();
						if( (this->getPos().dist(pos_) - this->radio - action->getBuild2()->radio + 1.0f) < this->minDistanceAttack)
							action->subType = AST_ATACK_ATACKING;
					}
					// Go to attack position.
					else
					{
						int err = this->moveToPos(action->pos, ms);
						
						// Find enemy object near (for each x ms for efficient).
						Object* enemyObject = NULL;
						if( (Time::currentMs() - this->tsSearchedNearEnemy) > MS_TO_FIND_NEAR_ENEMIES)
						{
							vec2 pos_ = this->getPos();
							int enemyEquip = this->equip;
							for(int i=0; i<EQUIPS-1; i++)
							{
								enemyEquip ++;
								enemyEquip %= EQUIPS;
								enemyObject = this->world->getCollision()->nearObjectType(pos_, OT_ALL, enemyEquip, VISIBILITY_DISTANCE, NULL, false);
								if(enemyObject != NULL)
									break;
							}
							this->tsSearchedNearEnemy = Time::currentMs();
						}
						
						// There is an enemy object near, go to attack it.
						if(enemyObject != NULL)
						{
							if(enemyObject->isBuilding())
								action->setBuild2((Building*)enemyObject);
							else if(enemyObject->isUnit())
								action->setUnit2((Unit*)enemyObject);
						}
						// Reach the position or there are error.
						else if(err == 1 || err < 0) 
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
					// I cant attack it, then set to go attack
					if(enemyDist > this->minDistanceAttack)
					{
						action->subType = AST_ATACK_GOTOATACK;
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
				param.unitID = this->id;
				param.unit2ID = 0;
				param.buildID = 0;
				param.build2ID = 0;
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




