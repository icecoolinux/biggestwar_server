
#include "unit.h"
#include "../world.h"
#include "../player.h"
#include "../action.h"
#include "../collision/collision.h"
#include "../../time.h"

Unit::Unit(ParamUnit& params): Object(params.world, params.type, params.id, params.player->equip, params.pos, params.radio)
{
	this->life = params.life;
	this->creada = params.creada;
	
	this->player = params.player;
	this->action = NULL;
	this->amountPath = -1;
	this->timeThatCollisioned = 0;
}

Unit::~Unit()
{
	
}

bool Unit::setAction(Action* action)
{
	if(action == NULL)
		return false;
	
	if(action->type != AT_MOVE)
		return false;
	
	if(action->subType != AST_MOVE_MOVE)
		return false;
	
	// Move to out map.
	if( action->pos.x <= 0.0f || action->pos.x >= SIDE_MAP_METERS || action->pos.y <= 0.0f || action->pos.y >= SIDE_MAP_METERS )
		return false;
	
	if(this->action != NULL)
		delete this->action;
	this->action = action;
	
	return true;
}

bool Unit::cancelAction()
{
	if(this->action == NULL)
		return false;
	
	this->removeAction();
	return true;
}

void Unit::update(int ms)
{
	if(this->action != NULL)
	{
		if(this->action->subType == AST_MOVE_MOVE)
		{
			// Move to object.
			if(this->action->getUnit2ID() > 0 || this->action->getBuild2ID() > 0)
			{
				int err;
				if(this->action->getUnit2ID() > 0)
					err = this->moveToObject(this->action->getUnit2(), ms);
				else
					err = this->moveToObject(this->action->getBuild2(), ms);
				
				if(err == 1 || err < 0) // Reach the target or there are error.
					this->removeAction();
			}
			// Move to position.
			else
			{
				int err = this->moveToPos(this->action->pos, ms);
				if(err == 1 || err < 0) // Reach the target or there are error.
					this->removeAction();
			}
		}
	}
}

void Unit::beenAttacked(Soldado* soldier)
{
	// I dont doing anything
}

void Unit::beenAttacked(BuildingSoldier* bs)
{
	// I dont doing anything
}

// Move to position, keep a minDist.
// Return 0: if not reach de pos yet.
// Return 1: if reach de pos with a min distance.
// Return -1: Error, the pos is far.
// Return -2: Error, there isn't path to pos.
// Return -3: Error, there is an obstacule.
int Unit::moveTo(vec2& pos, float minDist, Object* goalObject, int ms)
{
	// If there isn't path then make the path.
	if(this->amountPath < 0 || this->path[this->amountPath-1].dist(pos) > EPSILON_DISTANCE)
	{
		// Make a path.
		vec2 start = this->getPos();
		this->amountPath = this->world->getCollision()->getPath(start, pos, this->radio, goalObject, &this->path[0], MAX_PATH_MOVE_UNIT);

		// If there is an error then set one checkpoint (the pos objective).
		if(this->amountPath < 0)
		{
			this->path[0] = pos;
			this->amountPath = 1;
		}

		this->posPath = 0;
		
		// I start to move, initialize this.
		this->timeThatCollisioned = 0;
	}

	// Calculate the distance to do.
	float distStepRemaining = this->vel * ( ((float)ms)/1000.0f );
	
	// For each checkpoint in the path.
	while(this->posPath < this->amountPath)
	{
		// calculate the dir and distance to go.
		float distStep = distStepRemaining;
		float distDest = this->getPos().dist(this->path[this->posPath]);

		bool isLast = this->posPath == (this->amountPath-1);
		
		if(isLast && (distDest-minDist) < (distStep+.01f) )
			distStep = distDest-minDist-.01f;
		else if(!isLast && distDest < distStep )
			distStep = distDest;
		
		vec2 dir = this->path[this->posPath] - this->getPos();
		dir.normalize();
		
		// Move my position.
		vec2 posAux = this->getPos();
		posAux += dir * distStep;

		// Check if there's collision.
		Object* objCollision = this->world->getCollision()->checkCollision(posAux, this->radio, this);
		if(objCollision != NULL)
		{
			// The collision is with the goal object, OK.
			if(objCollision == goalObject)
				break;
			
			// If the position target is under the collision object.
			if(objCollision->getPos().dist(pos) <= (objCollision->radio + this->radio) )
			{
				this->amountPath = -1;
				return -3;
			}
			
			//TODO check if this work fine or is a mierda, improvment it
			// If I colissioned for 5 seconds then verify.
			// If my path to end there are buildings or units without move action then stop.
			if( this->timeThatCollisioned > 0 && (Time::currentSeconds() - this->timeThatCollisioned) > 5)
			{
				bool stopForEternity = true;
				
				// For each segment.
				vec2 step = this->getPos();
				for(int k=this->posPath; k < this->amountPath; k++)
				{
					// radio step to segment end.
					vec2 dirStep = this->path[k] - step;
					dirStep.normalize();
					float distStep;
					while((distStep = step.dist(this->path[k])) > EPSILON_DISTANCE)
					{
						if(distStep < this->radio)
							step = this->path[k];
						else
							step += dirStep * this->radio;
						
						// If there isn't collision or is a unit with move action then i don't do nothing.
						Object* objCollStep = this->world->getCollision()->checkCollision(step, this->radio, this);
						if(objCollStep == NULL || (objCollStep->isUnit() && objCollStep->getAction() != NULL && objCollStep->getAction()->isMoveAction()))
						{
							stopForEternity = false;
							break;
						}
					}
					if(!stopForEternity)
						break;
				}
printf("Check enternity %d\n", stopForEternity);
				if(stopForEternity)
				{
					this->amountPath = -1;
					return -3;
				}
				else
					this->timeThatCollisioned = 0;
			}
			// Set time that collisioned.
			else if(this->timeThatCollisioned == 0)
				this->timeThatCollisioned = Time::currentSeconds();

			
			/// If I collsioned then prevent the object.
			
			// Rollback position.
			posAux = this->getPos();
			
			// Move checkpoints to next free.
			int newPosPath = this->posPath+1;
			while(newPosPath < this->amountPath && objCollision->getPos().dist(this->path[newPosPath]) <= (objCollision->radio + this->radio + 0.01f) )
				newPosPath++;
			
			// The path is full to end, set to end.
			if(newPosPath >= this->amountPath)
				newPosPath = this->amountPath-1;

			// Move prevent the objCollision.
			vec2 prevent = posAux - objCollision->getPos();
			prevent.normalize();
			
			// Dir to next free checkpoint.
			dir =  this->path[newPosPath] - this->getPos();
			dir.normalize();

			vec2 newDir = dir + prevent;
			if( !newDir.normalize() )
			{
				// Problem with newDir, make a random newdir with small step.
				newDir.x = ((float) rand() / ((float)RAND_MAX));
				newDir.y = ((float) rand() / ((float)RAND_MAX));
				newDir.normalize();
				distStep *= 0.4;
			}
			
			// Move newly my position.
			posAux += newDir * distStep;

			// Check newly if there is collision.
			objCollision = this->world->getCollision()->checkCollision(posAux, this->radio, this);
			if(objCollision == NULL)
			{
				this->setPos(posAux);
				
				this->posPath = newPosPath;
				
				isLast = this->posPath == (this->amountPath-1);
			}
			// If i collisioned newly then move random.
			else
			{
				// Rollback position.
				posAux = this->getPos();
				
				// Random vector.
				newDir.x = ((float) rand() / ((float)RAND_MAX));
				newDir.y = ((float) rand() / ((float)RAND_MAX));
				newDir.normalize();
				
				posAux += newDir * distStep;
				
				// Check newly if there is collision.
				objCollision = this->world->getCollision()->checkCollision(posAux, this->radio, this);
				if(objCollision == NULL)
					this->setPos(posAux);
			}
		}
		// Not collision, ok, move.
		else
		{
			this->setPos(posAux);
			
			// Init the collision counter.
			this->timeThatCollisioned = 0;
		}

		
		if( (isLast && this->getPos().dist(this->path[this->posPath]) < (minDist+.02f)) || 
			(!isLast && this->getPos().dist(this->path[this->posPath]) < .01f) )
		{
			this->posPath++;
			distStepRemaining -= distStep;
		}
		else
			return 0;
	}

	// Reach the last checkpoint (the end).
	this->amountPath = -1;
	return 1;
}

// Return 1 if reach the target, 0 if not or negative if can't reach.
int Unit::moveToPos(vec2& pos, int ms)
{
	return this->moveTo(pos, 0.01f, NULL, ms);
}

// Return 1 if reach the target, 0 if not or negative if can't reach.
int Unit::moveToObject(Object* object, int ms)
{
	vec2 posObj_ = object->getPos();
	return this->moveTo(posObj_, object->radio + this->radio + 0.01f, object, ms);
}




