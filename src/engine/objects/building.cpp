
#include "building.h"
#include "../world.h"
#include "../player.h"
#include "../collision/collision.h"

Building::Building(ParamBuilding& params): Object(params.world, params.type, params.id, params.player->equip, params.pos, params.radio)
{
	this->life = params.life;
	this->creada = params.creada;
	
	this->construccionCreando = 0;
	this->construccionFull = 0;
	
	this->player = params.player;
}

Building::~Building()
{
	// If i'm making a unit then i delete it.
	if(this->action != NULL && this->action->type == AT_NEWUNIT && this->action->getUnit2() != NULL)
	{
		Unit* unitMake = this->action->getUnit2();
		this->action->setUnit2(NULL);
		delete unitMake;
	}
}

void Building::getResourceBuildCost(Action* action, int &minerals, int &oil)
{
	minerals = 0;
	oil = 0;
	if( action->getBuild()->type == OT_BASE && action->make == OT_RECOLECTOR)
	{
		minerals = MINERALS_COST_RECOLECTOR;
	}
	else if( action->getBuild()->type == OT_BARRACA)
	{
		if(action->make == OT_SOLDADO_RASO)
		{
			minerals = MINERALS_COST_SOLDADO_RASO;
		}
		else if(action->make == OT_SOLDADO_ENTRENADO)
		{
			minerals = MINERALS_COST_SOLDADO_ENTRENADO;
		}
		else if(action->make == OT_TANQUE)
		{
			minerals = MINERALS_COST_SOLDADO_TANQUE;
		}
		else if(action->make == OT_TANQUE_PESADO)
		{
			minerals = MINERALS_COST_SOLDADO_TANQUE_PESADO;
		}
	}
}

bool Building::setAction(Action* action)
{
	if(action == NULL)
		return false;

	if(this->creada < this->fullLife)
		return false;

	if(this->action != NULL)
		return false;

	if(action->type == AT_NEWUNIT && action->subType == AST_NEWUNIT_MAKEUNIT) {/* OK */}
	else
		return false;

	if( (action->getBuild()->type == OT_BASE && action->make == OT_RECOLECTOR) ||
		(action->getBuild()->type == OT_BARRACA && (action->make == OT_SOLDADO_RASO || 
												action->make == OT_SOLDADO_ENTRENADO || 
												action->make == OT_TANQUE || 
												action->make == OT_TANQUE_PESADO)) ) { /* OK */ }
	else
		return false;
	
	// Check and substracts resources.
	int mineral = this->player->getMinerals();
	int mineralsCost, oilCost;
	getResourceBuildCost(action, mineralsCost, oilCost);
	if(mineral < mineralsCost)
		return false;
	else
		this->player->setMinerals(mineral - mineralsCost);
		
	ParamUnit params;
	params.world = this->world;
	params.player = this->player;
	params.type = action->make;
	params.id = this->world->getNewId();
	params.pos.x = -1000;
	params.pos.y = -1000;
	params.life = 0;
	params.creada = 0;
			
	switch(action->make)
	{
		case OT_RECOLECTOR:
			params.radio = RADIO_RECOLECTOR;
			action->setUnit2(new Recolector(params));
			break;
		case OT_SOLDADO_RASO:
			params.radio = RADIO_SOLDADORASO;
			action->setUnit2(new SoldadoRaso(params));
			break;
		case OT_SOLDADO_ENTRENADO:
			params.radio = RADIO_SOLDADOENTRENADO;
			action->setUnit2(new SoldadoEntrenado(params));
			break;
		case OT_TANQUE:
			params.radio = RADIO_TANQUE;
			action->setUnit2(new Tanque(params));
			break;
		case OT_TANQUE_PESADO:
			params.radio = RADIO_TANQUEPESADO;
			action->setUnit2(new TanquePesado(params));
			break;
		default:
			return false;
	}
	
	this->construccionCreando = 0;
	this->construccionFull = action->getUnit2()->fullLife;
	
	this->action = action;
	
	this->world->setObjectId(params.id, action->getUnit2());

	return true;
}

bool Building::cancelAction()
{
	if(this->action == NULL)
		return false;
	
	// Re-add the resources.
	if(this->action->type == AT_NEWUNIT && this->action->subType == AST_NEWUNIT_MAKEUNIT)
	{
		int mineralsCost, oilCost;
		getResourceBuildCost(this->action, mineralsCost, oilCost);
		this->player->setMinerals(this->player->getMinerals() + mineralsCost);
	}
	
	this->removeAction();
	return true;
}

void Building::update(int ms)
{
	if(this->action != NULL && this->action->getUnit2() != NULL)
	{
		Unit* unit = this->action->getUnit2();
		if(this->type == OT_BASE)
			unit->creada += AMOUNT_MAKE_BY_SECOND_BASE * (((float)ms)/1000.0f) ;
		else if(this->type == OT_BARRACA)
			unit->creada += AMOUNT_MAKE_BY_SECOND_BARRACA * (((float)ms)/1000.0f) ;
		
		this->construccionCreando = unit->creada;
		
		if(unit->creada >= unit->fullLife)
		{
			if(this->player->getNumUnits() < MAX_UNITS_PLAYER)
			{
				vec2 pos_ = this->getPos();
				pos_ = this->world->getCollision()->getPosCollisionFree(pos_, this->radio, unit->radio);
				unit->setPos(pos_);
				unit->life = unit->fullLife;
				this->player->addUnit(unit);
				this->action->setUnit2(NULL);
				delete this->action;
				this->action = NULL;
				
				this->construccionCreando = 0;
				this->construccionFull = 0;
			}
		}
	}
}


