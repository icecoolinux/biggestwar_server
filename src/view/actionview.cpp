
#include "actionview.h"
#include "playerviewupdate.h"
#include "../engine/action.h"

ActionView::ActionView(Action* action)
{
	this->type = action->type;
	this->subType = action->subType;
	
	this->unitID = action->getUnitID();
	this->unit2ID = action->getUnit2ID();
	this->buildID = action->getBuildID();
	this->build2ID = action->getBuild2ID();
	this->objectMapID = action->getObjectMapID();
	
	this->pos = action->pos;
	
	this->make = action->make;
}

ActionView::~ActionView()
{
}
		
bool ActionView::isDifferent(Action* action)
{
	if(this->type != action->type)
		return true;
	if(this->subType != action->subType)
		return true;
	
	if(this->unitID != action->getUnitID())
		return true;
	if(this->unit2ID != action->getUnit2ID())
		return true;
	if(this->buildID != action->getBuildID())
		return true;
	if(this->build2ID != action->getBuild2ID())
		return true;
	if(this->objectMapID != action->getObjectMapID())
		return true;

	if(this->pos.dist(action->pos) > EPSILON_DISTANCE)
		return true;
	
	if(this->make != action->make)
		return true;
		
	return false;
}

void ActionView::getNew(PlayerViewUpdate &update)
{
	update.actionType = this->type;
	update.actionSubType = this->subType;
	update.actionUnitID = this->unitID;
	update.actionUnit2ID = this->unit2ID;
	update.actionBuildID = this->buildID;
	update.actionBuild2ID = this->build2ID;
	update.actionObjectMapID = this->objectMapID;
	update.actionPos = this->pos;
	update.actionMake = this->make;
}



