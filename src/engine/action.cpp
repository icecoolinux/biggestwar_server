
#include "action.h"
#include "objects/unit.h"
#include "objects/building.h"
#include "ids.h"

unordered_map<Object*, list<Action*>> Action::actionsOfObject;

Action::Action(ParamAction& params)
{
	this->type = params.type;
	this->subType = params.subType;
	
	this->unit = NULL;
	this->unit2 = NULL;
	this->build = NULL;
	this->build2 = NULL;
	this->objectMap = NULL;
	
	this->unitID = params.unitID;
	this->unit2ID = params.unit2ID;
	this->buildID = params.buildID;
	this->build2ID = params.build2ID;
	this->objectMapID = params.objectMapID;
	
	this->pos = params.pos;
	
	this->make = params.make;
}

Action::~Action()
{
	// Quite action from obj-action map.
	Object* objs[5];
	objs[0] = this->unit;
	objs[1] = this->unit2;
	objs[2] = this->build;
	objs[3] = this->build2;
	objs[4] = this->objectMap;
	for(int i=0; i<5; i++)
	{
		if(objs[i] != NULL)
			Action::unsetAction(objs[i], this);
	}
}

bool Action::setIds(IDS* ids)
{
	if(this->unitID > 0)
	{
		this->unit = (Unit*)ids->getObject(this->unitID);
		if(this->unit == NULL)
			return false;
		else
			this->actionsOfObject[this->unit].push_back(this);
	}
	
	if(this->unit2ID > 0)
	{
		this->unit2 = (Unit*)ids->getObject(this->unit2ID);
		if(this->unit2 == NULL)
			return false;
		else
			this->actionsOfObject[this->unit2].push_back(this);
	}
	
	if(this->buildID > 0)
	{
		this->build = (Building*)ids->getObject(this->buildID);
		if(this->build == NULL)
			return false;
		else
			this->actionsOfObject[this->build].push_back(this);
	}

	if(this->build2ID > 0)
	{
		this->build2 = (Building*)ids->getObject(this->build2ID);
		if(this->build2 == NULL)
			return false;
		else
			this->actionsOfObject[this->build2].push_back(this);
	}
	
	if(this->objectMapID > 0)
	{
		this->objectMap = ids->getObject(this->objectMapID);
		if(this->objectMap == NULL)
			return false;
		else
			this->actionsOfObject[this->objectMap].push_back(this);
	}

	return true;
}

unsigned long long Action::getUnitID()
{
	return this->unitID;
}
unsigned long long Action::getUnit2ID()
{
	return this->unit2ID;
}
unsigned long long Action::getBuildID()
{
	return this->buildID;
}
unsigned long long Action::getBuild2ID()
{
	return this->build2ID;
}
unsigned long long Action::getObjectMapID()
{
	return this->objectMapID;
}

Unit* Action::getUnit()
{
	return this->unit;
}

Unit* Action::getUnit2()
{
	return this->unit2;
}

void Action::setUnit2(Unit* unit2)
{
	if(this->unit2 != NULL)
		Action::unsetAction(this->unit2, this);
	
	this->unit2 = unit2;
	if(unit2 != NULL)
	{
		this->actionsOfObject[unit2].push_back(this);
		this->unit2ID = unit2->id;
	}
	else
		this->unit2ID = 0;
}

Building* Action::getBuild()
{
	return this->build;
}
		
Building* Action::getBuild2()
{
	return this->build2;
}

void Action::setBuild2(Building* build2)
{
	if(this->build2 != NULL)
		Action::unsetAction(this->build2, this);
	
	this->build2 = build2;
	if(build2 != NULL)
	{
		this->actionsOfObject[build2].push_back(this);
		this->build2ID = build2->id;
	}
	else
		this->build2ID = 0;
}

Object* Action::getObjectMap()
{
	return this->objectMap;
}

void Action::setObjectMap(Object* obj)
{
	if(this->objectMap != NULL)
		Action::unsetAction(this->objectMap, this);
	
	this->objectMap = obj;
	if(obj != NULL)
	{
		this->actionsOfObject[obj].push_back(this);
		this->objectMapID = obj->id;
	}
	else
		this->objectMapID = 0;
}

bool Action::isMoveAction()
{
	return this->subType == AST_MOVE_MOVE || this->subType == AST_RECOLLECT_GORESOURCE || 
			this->subType == AST_RECOLLECT_GOBASE || this->subType == AST_BUILD_GOTOBUILD || 
			this->subType == AST_ATACK_GOTOATACK;
}

// Remove and unset actions that have this object.
// Notice to subject to delete the action.
void Action::removeLinkActions(Object* obj)
{
	list<Action*> *actionsList = &(Action::actionsOfObject[obj]);
	while(actionsList->size() > 0)
	{
		Action* action = actionsList->front();
		actionsList->pop_front();
		
		// Unit is the subject.
		if(action->unit != NULL)
			action->unit->removeAction();
		// Build is the subject.
		else
			action->build->removeAction();
	}
	
	Action::actionsOfObject.erase(obj);
}

// Return all the actions that has the object (as subject or passive).
void Action::getActionFromObject(Object* obj, list<Action*> &actions)
{
	list<Action*> *actionsList = &(Action::actionsOfObject[obj]);
	actions.assign(actionsList->begin(), actionsList->end());
}

// Unset the action to the object.
void Action::unsetAction(Object* obj, Action* action)
{
	list<Action*> *actionsList = &(Action::actionsOfObject[obj]);
	list<Action*>::iterator it;
	for(it = actionsList->begin(); it != actionsList->end(); it++)
	{
		Action* a = *it;
		if(a == action)
		{
			actionsList->erase(it);
			return;
		}
	}
}

void Action::print()
{
	printf("Type: %d\n", this->type);
	printf("SubType: %d\n", this->subType);
	printf("InitID: %llu\n", this->unitID);
	printf("Unit2ID: %llu\n", this->unit2ID);
	printf("BuildID: %llu\n", this->buildID);
	printf("Build2ID: %llu\n", this->build2ID);
	printf("ObjectmapID: %llu\n", this->objectMapID);
	printf("Pos: %f %f\n", this->pos.x, this->pos.y);
	printf("Make: %d\n", this->make);
}


