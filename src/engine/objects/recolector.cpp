
#include "recolector.h"
#include "../action.h"
#include "mineral.h"
#include "../world.h"
#include "../map.h"
#include "../player.h"
#include "../collision/collision.h"
#include "../minimap.h"
#include "base.h"
#include "barraca.h"
#include "torreta.h"

Recolector::Recolector(ParamUnit& params): Unit(params)
{
	this->fullLife = FULL_LIFE_RECOLECTOR;
	this->vel = VEL_RECOLECTOR;
	this->collected = 0;
}

float Recolector::getCollected()
{
	return this->collected;
}

bool Recolector::setAction(Action* action)
{
	if(action == NULL)
		return false;
	
	if(action->type == AT_MOVE)
		return Unit::setAction(action);
	
	if( (action->type == AT_RECOLLECT && action->subType == AST_RECOLLECT_GORESOURCE) ||
		(action->type == AT_BUILD && action->subType == AST_BUILD_GOTOBUILD) ) { /* OK */ }
	else
		return false;
	
	// Check that if I can building here
	if(action->type == AT_BUILD)
	{
		float radioBuilding = 0;
		if(action->make == OT_BASE)
			radioBuilding = RADIO_BASE;
		else if(action->make == OT_BARRACA)
			radioBuilding = RADIO_BARRACA;
		else if(action->make == OT_TORRETA)
			radioBuilding = RADIO_TORRETA;
		
		if(!this->world->getMap()->canBuild(action->pos, radioBuilding+2.0f, this->equip))
			return false;
	}
	
	if(this->action != NULL)
		delete this->action;
	
	this->action = action;
	return true;
}

void Recolector::update(int ms)
{
	if(this->action != NULL)
	{
		Action* action = this->action;
		
		if(action->type == AT_MOVE)
			Unit::update(ms);
		else
		{
			if(action->type == AT_RECOLLECT)
			{
				if(action->subType == AST_RECOLLECT_GORESOURCE)
				{
					// Resource isn't, find another resource.
					if(action->getObjectMap() == NULL)
					{
						vec2 pos_ = this->getPos();
						Mineral* mineral = (Mineral*)this->world->getCollision()->nearObjectType(pos_, OT_MINERAL, -2, DISTANCE_LIMIT_FIND_ANOTHER_MINERAL_RECOLECTOR, NULL, false);
						if(mineral == NULL)
							this->removeAction();
						// Set new resource.
						else
							action->setObjectMap(mineral);
					}
					// There are resource.
					else if(this->moveToObject(action->getObjectMap(), ms) == 1) // Reach the target.
						action->subType = AST_RECOLLECT_RECOLLECTING;
				}
				else if(action->subType == AST_RECOLLECT_RECOLLECTING)
				{
					Mineral* mineral = NULL;
					if(action->getObjectMap()->type == OT_MINERAL)
						mineral = (Mineral*)action->getObjectMap();

					float amountCollected = AMOUNT_RECOLLECT_BY_SECOND_RECOLECTOR * (((float)ms)/1000.0f);
					
					if( (this->collected +amountCollected) > MAX_AMOUNT_RECOLLECT_RECOLECTOR)
						amountCollected = MAX_AMOUNT_RECOLLECT_RECOLECTOR - this->collected;
					
					if(mineral != NULL)
					{
						// Mineral is out.
						if(mineral->amount < amountCollected)
						{
							// Get ALL recolectores that are recolectando this mineral and set it go to base or another mineral and clear the mineral.
							list<Action*> actions;
							Action::getActionFromObject(mineral, actions);
							list<Action*>::iterator itAction;
							for(itAction = actions.begin(); itAction != actions.end(); itAction++)
							{
								Action* a = *itAction;
								if(a->getUnit() != NULL && a->getObjectMap() == mineral && a->type == AT_RECOLLECT)
								{
									a->setObjectMap(NULL);

									// If the unit hasn't mineral then go to another mineral else go to base.
									if(((Recolector*)a->getUnit())->getCollected() < 1.0f)
										a->subType = AST_RECOLLECT_GORESOURCE;
									else
										a->subType = AST_RECOLLECT_GOBASE;
								}
							}

							amountCollected = mineral->amount;
							
							// Remove the mineral.
							this->world->getMap()->quiteMineral(mineral->id);
							delete mineral;
						}
						else
							mineral->amount -= amountCollected;
						
						this->collected += amountCollected;
					}
					
					if(this->collected >= MAX_AMOUNT_RECOLLECT_RECOLECTOR)
						action->subType = AST_RECOLLECT_GOBASE;
				}
				else if(action->subType == AST_RECOLLECT_GOBASE)
				{
					// I must to find the closed base.
					if(action->getBuild2() == NULL)
					{
						vec2 pos_ = this->getPos();
						Building* nearBase = (Building*)this->world->getCollision()->nearObjectType(pos_, OT_BASE, this->equip, DISTANCE_LIMIT_FIND_BASE_RECOLECTOR, this->player->name, true);
						action->setBuild2(nearBase);

						// There isn't a close base.
						if(action->getBuild2() == NULL)
							this->removeAction();
					}
					else if(this->moveToObject(action->getBuild2(), ms) == 1) // Reach the target.
						action->subType = AST_RECOLLECT_PUTTOBASE;
				}
				else if(action->subType == AST_RECOLLECT_PUTTOBASE)
				{
					this->player->setMinerals( this->player->getMinerals() + this->collected );
					this->collected = 0;
					
					// Go to the resource.
					action->subType = AST_RECOLLECT_GORESOURCE;
				}
			}
			
			else if(action->type == AT_BUILD)
			{
				if(action->subType == AST_BUILD_GOTOBUILD)
				{
					// Go to build a started building.
					if(action->getBuild2() != NULL)
					{
						// The building is already builded.
						if(action->getBuild2()->creada > action->getBuild2()->fullLife)
							this->removeAction();
						// Else move to near to building.
						else if(this->moveToObject(action->getBuild2(), ms) == 1) // Reach the target.
							action->subType = AST_BUILD_BUILDING;
					}
					// Go to start a new build.
					else
					{
						int retMove = this->moveToPos(action->pos, ms);
						if(retMove == 1) // Reach the target.
						{
							if(this->player->getNumBuildings() < MAX_BUILDINGS_PLAYER)
							{
								int mineral = this->player->getMinerals();
								
								// Check if there is resources.
								bool thereIsResources = true;
								if(action->make == OT_BASE && mineral < MINERALS_COST_BASE)
									thereIsResources = false;
								else if(action->make == OT_BARRACA && mineral < MINERALS_COST_BARRACA)
									thereIsResources = false;
								else if(action->make == OT_TORRETA && mineral < MINERALS_COST_TORRETA)
									thereIsResources = false;
								
								if(thereIsResources)
								{
									ParamBuilding params;
									params.world = this->world;
									params.player = this->player;
									params.type = action->make;
									params.id = this->world->getNewId();
									params.pos = action->pos;
									params.life = 0;
									params.creada = 0;
									if(action->make == OT_BASE)
										params.radio = RADIO_BASE;
									else if(action->make == OT_BARRACA)
										params.radio = RADIO_BARRACA;
									else if(action->make == OT_TORRETA)
										params.radio = RADIO_TORRETA;
									
									// Check if there is space.
									Object* collisionedTmp;
									bool withMapTmp;
									if(this->world->getCollision()->getBuildingsCollision(params.pos, params.radio, this, collisionedTmp, withMapTmp) ||
										this->world->getCollision()->checkCollision(params.pos, params.radio, this) != NULL )
									{
										// There isn't space.
										this->removeAction();
									}
									else
									{
										// Substract resources.
										if(action->make == OT_BASE && mineral < MINERALS_COST_BASE)
											thereIsResources = false;
										else if(action->make == OT_BARRACA && mineral < MINERALS_COST_BARRACA)
											thereIsResources = false;
										else if(action->make == OT_TORRETA && mineral < MINERALS_COST_TORRETA)
												thereIsResources = false;
										
										// Make building and substract resources.
										if(action->make == OT_BASE)
										{
											this->player->setMinerals(mineral - MINERALS_COST_BASE);
											action->setBuild2(new Base(params));
										}
										else if(action->make == OT_BARRACA)
										{
											this->player->setMinerals(mineral - MINERALS_COST_BARRACA);
											action->setBuild2(new Barraca(params));
										}
										else if(action->make == OT_TORRETA)
										{
											this->player->setMinerals(mineral - MINERALS_COST_TORRETA);
											action->setBuild2(new Torreta(params));
										}

										this->world->setObjectId(params.id, action->getBuild2());
										this->player->addBuilding(action->getBuild2());
										action->subType = AST_BUILD_BUILDING;

										// Positing unit out of building.
										vec2 pos_ = action->getBuild2()->getPos();
										pos_ = this->world->getCollision()->getPosCollisionFree(pos_, action->getBuild2()->radio, this->radio);
										this->setPos(pos_);
									}
								}
								// There isn't resources.
								else
									this->removeAction();
							}
							// Reach max number of buildings
							else
								this->removeAction();
						}
						// Error to reach the target, cancel action.
						else if(retMove < 0)
							this->removeAction();
					}
				}
				else if(action->subType == AST_BUILD_BUILDING)
				{
					action->getBuild2()->creada += AMOUNT_BUILD_BY_SECOND_RECOLECTOR * (((float)ms)/1000.0f);
					
					if(action->getBuild2()->creada > action->getBuild2()->fullLife)
					{
						action->getBuild2()->life = action->getBuild2()->fullLife;
						this->removeAction();
					}
				}
			}
		}
	}
}

