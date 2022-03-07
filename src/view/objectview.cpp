
#include "objectview.h"
#include "playerview.h"
#include "playerviewupdate.h"
#include "actionview.h"
#include "../engine/objects/object.h"
#include "../engine/objects/unit.h"
#include "../engine/objects/buildingsoldier.h"
#include "../engine/objects/mineral.h"
#include "../engine/player.h"
#include "../engine/objects/recolector.h"

#include <string.h>

ObjectView::ObjectView(Object* obj, char* myName, int myEquip)
{
	this->id = obj->id;
	this->type = obj->type;
	this->pos = obj->getPos();
	
	this->enemy = obj->equip != -1 && obj->equip != myEquip; // Not map and isn't in my equip.
	this->aliade = false;
	this->mine = false;
	this->map = false;
	
	strcpy(this->playerName, "");
	this->action = NULL;
	
	this->tsLastAttack = 0;
	
	if(obj->isUnit())
	{
		Unit* u = (Unit*)obj;
		strcpy(this->playerName, u->player->name);
		
		this->life = u->life;
		this->fullLife = u->fullLife;
		this->creada = u->creada;
		this->construccionCreando = 0; //Only for send 0 in the net.
		this->construccionFull = 0; //Only for send 0 in the net.
		
		if(u->getAction() == NULL)
			this->action = NULL;
		else
			this->action = new ActionView(u->getAction());
		
		// It's soldier.
		if(u->type == OT_SOLDADO_RASO || u->type == OT_SOLDADO_ENTRENADO || u->type == OT_TANQUE || u->type == OT_TANQUE_PESADO)
			this->tsLastAttack = ((Soldado*)u)->tsLastAttack;
		
		if(u->type == OT_RECOLECTOR)
			this->collected = ((Recolector*)u)->getCollected();
		
		// Is mine.
		if(strcmp(myName, this->playerName) == 0)
		{
			this->mine = true;
			this->aliade = false;
		}
		else if(obj->equip == myEquip) // Is in my equip.
			this->aliade = true;
	}
	else if(obj->isBuilding())
	{
		Building* b = (Building*)obj;
		strcpy(this->playerName, b->player->name);
		
		this->life = b->life;
		this->fullLife = b->fullLife;
		this->creada = b->creada;
		this->construccionCreando = b->construccionCreando;
		this->construccionFull = b->construccionFull; 
		
		if(b->getAction() == NULL)
			this->action = NULL;
		else
			this->action = new ActionView(b->getAction());
		
		// It's buildingsoldier.
		if(b->type == OT_TORRETA)
			this->tsLastAttack = ((BuildingSoldier*)b)->tsLastAttack;
		
		// Is mine.
		if(strcmp(myName, this->playerName) == 0)
		{
			this->mine = true;
			this->aliade = false;
		}
		else if(obj->equip == myEquip) // Is in my equip.
			this->aliade = true;
	}
	else if(obj->type == OT_MINERAL)
	{
		this->map = true;
		this->amount = ((Mineral*)obj)->amount;
	}
}

ObjectView::~ObjectView()
{
	if(this->action != NULL)
		delete this->action;
}
		
bool ObjectView::thereAreChange(Object *obj)
{
	vec2 pos_ = this->pos;
	if(obj->getPos().dist(pos_) > EPSILON_DISTANCE)
		return true;

	if(obj->isUnit())
	{
		Unit* u = (Unit*)obj;
		if(this->life != u->life)
			return true;
		if(this->fullLife != u->fullLife)
			return true;
		if(this->creada != u->creada)
			return true;
	}
	else if(obj->isBuilding())
	{
		Building* b = (Building*)obj;
		if(this->life != b->life)
			return true;
		if(this->fullLife != b->fullLife)
			return true;
		if(this->creada != b->creada)
			return true;
		if(this->construccionCreando != b->construccionCreando)
			return true;
		if(this->construccionFull != b->construccionFull)
			return true;
	}

	if(obj->type == OT_SOLDADO_RASO || obj->type == OT_SOLDADO_ENTRENADO || obj->type == OT_TANQUE || obj->type == OT_TANQUE_PESADO)
		if(this->tsLastAttack != ((Soldado*)obj)->tsLastAttack )
			return true;
	
	if(obj->type == OT_TORRETA)
		if(this->tsLastAttack != ((BuildingSoldier*)obj)->tsLastAttack)
			return true;
		
	if(obj->type == OT_RECOLECTOR && this->collected != ((Recolector*)obj)->getCollected())
		return true;

	if(obj->type == OT_MINERAL && this->amount != ((Mineral*)obj)->amount)
		return true;
	
	if(this->action == NULL && obj->getAction() != NULL)
		return true;
	else if(this->action != NULL && obj->getAction() == NULL)
		return true;
	else if(this->action != NULL && obj->getAction() != NULL)
		return this->action->isDifferent(obj->getAction());

	return false;
}

// return true if there are changes.
bool ObjectView::getChanges(Object* obj, PlayerViewUpdate &update)
{
	if(this->thereAreChange(obj))
	{
		update.init();

		update.id = this->id;
		
		update.wasChanged = true;
		
		if(obj->getPos().dist(this->pos) > EPSILON_DISTANCE)
		{
			update.updatePos = true;
			update.pos = obj->getPos();
			this->pos = obj->getPos();
		}
		
		if(obj->isUnit())
		{
			Unit* u = (Unit*)obj;
			if(this->life != u->life)
			{
				update.updateLife = true;
				update.life = u->life;
				this->life = u->life;
			}
			
			if(this->fullLife != u->fullLife)
			{
				update.updateFullLife = true;
				update.fullLife = u->fullLife;
				this->fullLife = u->fullLife;
			}
			
			if(this->creada != u->creada)
			{
				update.updateCreada = true;
				update.creada = u->creada;
				this->creada = u->creada;
			}
		}
		if(obj->isBuilding())
		{
			Building* b = (Building*)obj;
			if(this->life != b->life)
			{
				update.updateLife = true;
				update.life = b->life;
				this->life = b->life;
			}
			
			if(this->fullLife != b->fullLife)
			{
				update.updateFullLife = true;
				update.fullLife = b->fullLife;
				this->fullLife = b->fullLife;
			}
			
			if(this->creada != b->creada)
			{
				update.updateCreada = true;
				update.creada = b->creada;
				this->creada = b->creada;
			}
			
			if(this->construccionCreando != b->construccionCreando || this->construccionFull != b->construccionFull)
			{
				update.updateConstruccionCreando = true;
				update.construccionCreando = b->construccionCreando;
				this->construccionCreando = b->construccionCreando;
				update.construccionFull = b->construccionFull;
				this->construccionFull = b->construccionFull;
			}
		}

		if(this->action == NULL && obj->getAction() != NULL)
		{
			update.newAction = true;
			this->action = new ActionView(obj->getAction());
			this->action->getNew(update);
		}
		else if(this->action != NULL && obj->getAction() == NULL)
		{
			update.actionDeleted = true;
			delete this->action;
			this->action = NULL;
		}
		else if(this->action != NULL && obj->getAction() != NULL && this->action->isDifferent(obj->getAction()))
		{
			update.newAction = true;
			delete this->action;
			this->action = new ActionView(obj->getAction());
			this->action->getNew(update);
		}
		
		if(obj->type == OT_SOLDADO_RASO || obj->type == OT_SOLDADO_ENTRENADO || obj->type == OT_TANQUE || obj->type == OT_TANQUE_PESADO)
		{
			if(this->tsLastAttack != ((Soldado*)obj)->tsLastAttack)
			{
				update.updateTsLastAttack = true;
				update.tsLastAttack = ((Soldado*)obj)->tsLastAttack;
				this->tsLastAttack = ((Soldado*)obj)->tsLastAttack;
			}
		}
		
		if(obj->type == OT_TORRETA)
		{
			if(this->tsLastAttack != ((BuildingSoldier*)obj)->tsLastAttack)
			{
				update.updateTsLastAttack = true;
				update.tsLastAttack = ((BuildingSoldier*)obj)->tsLastAttack;
				this->tsLastAttack = ((BuildingSoldier*)obj)->tsLastAttack;
			}
		}
		
		if(obj->type == OT_RECOLECTOR && this->collected != ((Recolector*)obj)->getCollected())
		{
			update.updateCollected = true;
			update.collected = ((Recolector*)obj)->getCollected();
			this->collected = ((Recolector*)obj)->getCollected();
		}
		
		if(obj->type == OT_MINERAL && this->amount != ((Mineral*)obj)->amount)
		{
			update.updateAmount = true;
			update.amount = ((Mineral*)obj)->amount;
			this->amount = ((Mineral*)obj)->amount;
		}
		
		return true;
	}
	else
		return false;
}

void ObjectView::getNew(PlayerViewUpdate &update)
{
	update.init();
	
	update.id = this->id;
	
	update.isNew = true;
	strcpy(update.playerName, this->playerName);
	update.type = this->type;
	
	update.updatePos = true;
	update.pos = this->pos;

	update.updateLife = true;
	update.life = this->life;

	update.updateFullLife = true;
	update.fullLife = this->fullLife;

	update.updateCreada = true;
	update.creada = this->creada;

	update.updateConstruccionCreando = true;
	update.construccionCreando = this->construccionCreando;
	update.construccionFull = this->construccionFull;

	if(this->action != NULL)
	{
		update.newAction = true;
		this->action->getNew(update);
	}

	update.updateCollected = true;
	update.collected = this->collected;

	update.updateAmount = true;
	update.amount = this->amount;
}










