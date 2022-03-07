
#include "collision.h"
#include "pathfinder.h"
#include "../objects/unit.h"
#include "../objects/building.h"
#include "../objects/object.h"
#include <cfloat>
#include <string.h>

Collision::Collision(Terrain* terrain)
{
	this->terrain = terrain;
	this->pathfinder = new PathFinder();
	this->quadSideMeters = ((float)SIDE_MAP_METERS) / ((float)QUADS_SIDE);
}

Collision::~Collision()
{
	delete this->pathfinder;
}

void Collision::addObject(Object* obj)
{
	vec2 pos = obj->getPos();
	
	int* objQuad = new int[2];
	objQuad[0] = pos.x / this->quadSideMeters;
	objQuad[1] = pos.y / this->quadSideMeters;

	if(objQuad[0] < 0 || objQuad[0] >= QUADS_SIDE || objQuad[1] < 0 || objQuad[1] >= QUADS_SIDE)
		quadOutMap.push_front(obj);
	else
		this->quads[objQuad[0]][objQuad[1]].push_front(obj);
	
	this->objToQuad[obj] = objQuad;
	
	if(obj->type == OT_MINERAL || obj->type == OT_BASE || obj->type == OT_BARRACA || obj->type == OT_TORRETA)
		this->pathfinder->add(obj);
}

void Collision::updateObject(Object* obj)
{
	if(obj->type == OT_MINERAL || obj->type == OT_BASE || obj->type == OT_BARRACA || obj->type == OT_TORRETA)
		this->pathfinder->remove(obj);
	
	vec2 pos = obj->getPos();
	
	int x = pos.x / this->quadSideMeters;
	int y = pos.y / this->quadSideMeters;

	int* objQuad = this->objToQuad[obj];
	if(x != objQuad[0] || y != objQuad[1])
	{
		if(objQuad[0] < 0 || objQuad[0] >= QUADS_SIDE || objQuad[1] < 0 || objQuad[1] >= QUADS_SIDE)
			this->quadOutMap.remove(obj);
		else
			this->quads[objQuad[0]][objQuad[1]].remove(obj);
		
		if(x < 0 || x >= QUADS_SIDE || y < 0 || y >= QUADS_SIDE)
			this->quadOutMap.push_front(obj);
		else
			this->quads[x][y].push_front(obj);
		
		objQuad[0] = x;
		objQuad[1] = y;
	}
}

void Collision::removeObject(Object* obj)
{
	this->pathfinder->remove(obj);
	
	int* objQuad = this->objToQuad[obj];
	
	if(objQuad[0] < 0 || objQuad[0] >= QUADS_SIDE || objQuad[1] < 0 || objQuad[1] >= QUADS_SIDE)
		this->quadOutMap.remove(obj);
	else
		this->quads[objQuad[0]][objQuad[1]].remove(obj);
	
	this->objToQuad.erase(obj);
	
	delete[] objQuad;
}
/*
// Dado un objeto, lo mueve en direccion 'dir' con una distancia 'dist'.
// Return obj collisioned or NULL if not collisioned.
Object* Collision::move(Object* obj, vec2& dir, float dist)
{
	vec2 pos = obj->getPos();
	vec2 posTmp = pos;
	
	pos += dir * dist;

	// Check if there's colision.
	Object* objCollision = this->checkCollision(pos, obj->radio, obj);
	if(objCollision != NULL)
	{
		// Rollback position.
		pos = posTmp;
		
		// Move prevent the objCollision.
		vec2 prevent = pos - objCollision->getPos();
		prevent.normalize();
		vec2 newDir = dir + prevent;

		// Problem with newDir, make a random newdir with small step.
		if( !newDir.normalize() )
		{
			newDir.x = ((float) rand() / ((float)RAND_MAX));
			newDir.y = ((float) rand() / ((float)RAND_MAX));
			newDir.normalize();
			newDir *= 0.4;
		}
		
		pos += newDir * dist;

		// Check newly if there is collision.
		objCollision = this->checkCollision(pos, obj->radio, obj);
		if(objCollision == NULL)
			obj->setPos(pos);
		
		return objCollision;
	}
	else
	{
		obj->setPos(pos);
		return NULL;
	}
}
*/


// Return a position without collision for an object, near to another object.
vec2 Collision::getPosCollisionFree(vec2& posNear, float radioNear, float radio)
{
	// This algorithm search free space like a snail.
	// When find a free space try to insert the object.
	// 0 down, 1 right, 2 up, 3 left.
	int dir = 0;
	vec2 pos = posNear;
	int amountSide = 1;
	int remainSide = 1;
	while(true)
	{
		// Make many step snail until it's far radioNear+radio.
		if(posNear.dist(pos) > (radioNear+radio+.05f))
		{
			// Try to insert object.
			vec2 posObj = pos;
			switch(dir)
			{
				case 0:
					posObj.x -= radio;
					posObj.y -= radio;
					break;
				case 1:
					posObj.x += radio;
					posObj.y -= radio;
					break;
				case 2:
					posObj.x += radio;
					posObj.y += radio;
					break;
				case 3:
					posObj.x -= radio;
					posObj.y += radio;
					break;
			}
			
			Object* collisioned;
			bool withMap;
			if(posObj.x >= 0 && posObj.x < SIDE_MAP_METERS && posObj.y >= 0 && posObj.y < SIDE_MAP_METERS)
				if(!this->pathfinder->getCollision(posObj, radio, NULL, collisioned, withMap))
					if(checkCollision(posObj, radio, NULL) == NULL)
						return posObj;
		}
		
		// One step.
		if(remainSide==0)
		{
			if(dir == 0)
				dir = 1;
			else if(dir == 1)
			{
				dir = 2;
				amountSide++;
			}
			else if(dir == 2)
				dir = 3;
			else
			{
				dir = 0;
				amountSide++;
			}
			remainSide = amountSide;
		}
		switch(dir)
		{
			case 0:
				pos.x += 1.0f;
				break;
			case 1:
				pos.y += 1.0f;
				break;
			case 2:
				pos.x -= 1.0f;
				break;
			case 3:
				pos.y -= 1.0f;
				break;
		}
		remainSide--;
	}
}

// Return all object from a area.
// return number of objects.
int Collision::getObjects(float xMin, float xMax, float yMin, float yMax, Object** objects, int maxObjects)
{
	int posRet = 0;
	
	int xMinQ = xMin / this->quadSideMeters;
	int xMaxQ = xMax / this->quadSideMeters;
	xMaxQ++;
	int yMinQ = yMin / this->quadSideMeters;
	int yMaxQ = yMax / this->quadSideMeters;
	yMaxQ++;
	
	if(xMinQ < 0)
		xMinQ = 0;
	if(xMinQ >= QUADS_SIDE)
		xMinQ = QUADS_SIDE-1;
	if(xMaxQ < 0)
		xMaxQ = 0;
	if(xMaxQ >= QUADS_SIDE)
		xMaxQ = QUADS_SIDE-1;
	if(yMinQ < 0)
		yMinQ = 0;
	if(yMinQ >= QUADS_SIDE)
		yMinQ = QUADS_SIDE-1;
	if(yMaxQ < 0)
		yMaxQ = 0;
	if(yMaxQ >= QUADS_SIDE)
		yMaxQ = QUADS_SIDE-1;
	
	// Check quads and add objects inside area.
	// Return true if must to return.
	auto checkQuad = [&](list<Object*> *objectsList)
	{
int counterror=1;
		list<Object*>::iterator it;
		for(it = objectsList->begin(); it != objectsList->end(); it++)
		{
			Object* objCol = *it;
if( ((unsigned long long)objCol) == 0x1)
{
	printf("Llegue a error1, %d %d, %d %d\n", counterror, objectsList->size(), it, *it);
}
counterror++;
			vec2 pos = objCol->getPos();
			if(pos.x >= xMin && pos.x <= xMax && pos.y >= yMin && pos.y <= yMax)
			{
				objects[posRet] = objCol;
				posRet++;
				if(posRet >= maxObjects)
					return true;
			}
		}
		
		return false;
	};
	
	// Left and right border.
	for(int j=yMinQ; j<=yMaxQ; j++)
	{
		// Check left quad.
		if(checkQuad(&this->quads[xMinQ][j]))
			return posRet;
		
		// Right quads are differents that left quads, check right.
		if(xMinQ < xMaxQ)
		{
			if(checkQuad(&this->quads[xMaxQ][j]))
				return posRet;
		}
	}
	
	// Down and up border.
	for(int i=xMinQ+1; i<xMaxQ; i++)
	{
		// Check down quad.
		if(checkQuad(&this->quads[i][yMinQ]))
			return posRet;
		
		// Right quads are differents that left quads, check right.
		if(yMinQ < yMaxQ)
		{
			if(checkQuad(&this->quads[i][yMaxQ]))
				return posRet;
		}
	}
	
	// Add all objects are inside quads.
	for(int i=xMinQ+1; i<xMaxQ; i++)
	{
		for(int j=yMinQ+1; j<yMaxQ; j++)
		{
			list<Object*> *objectsList = &this->quads[i][j];
			list<Object*>::iterator it;
int counterror=1;
			for(it = objectsList->begin(); it != objectsList->end(); it++)
			{
				Object* objCol = *it;
				objects[posRet] = objCol;
if( ((unsigned long long)objCol) == 0x1)
{
	printf("Llegue a error2, %d %d, %d %d\n", counterror, objectsList->size(), it, *it);
	printf("B %d %d %llu\n", posRet, objCol->type, objCol->id);
}
counterror++;
				posRet++;
				if(posRet >= maxObjects)
					return posRet;
			}
		}
	}
	
	return posRet;
}

// Return all object from a circle area.
// return number of objects.
int Collision::getObjects(vec2& center, float radio, Object** objects, int maxObjects)
{
	int posRet = 0;
	
	int xMinQ = (center.x-radio) / this->quadSideMeters;
	int xMaxQ = (center.x+radio) / this->quadSideMeters;
	xMaxQ++;
	int yMinQ = (center.y-radio) / this->quadSideMeters;
	int yMaxQ = (center.y+radio) / this->quadSideMeters;
	yMaxQ++;
	
	if(xMinQ < 0)
		xMinQ = 0;
	if(xMinQ >= QUADS_SIDE)
		xMinQ = QUADS_SIDE-1;
	if(xMaxQ < 0)
		xMaxQ = 0;
	if(xMaxQ >= QUADS_SIDE)
		xMaxQ = QUADS_SIDE-1;
	if(yMinQ < 0)
		yMinQ = 0;
	if(yMinQ >= QUADS_SIDE)
		yMinQ = QUADS_SIDE-1;
	if(yMaxQ < 0)
		yMaxQ = 0;
	if(yMaxQ >= QUADS_SIDE)
		yMaxQ = QUADS_SIDE-1;
	
	// Add all objects are inside quads.
	for(int i=xMinQ; i<=xMaxQ; i++)
	{
		for(int j=yMinQ; j<=yMaxQ; j++)
		{
			list<Object*> *objectsList = &this->quads[i][j];
			list<Object*>::iterator it;
			for(it = objectsList->begin(); it != objectsList->end(); it++)
			{
				Object* objCol = *it;
				vec2 pos = objCol->getPos();
				if(pos.dist(center) <= (radio + objCol->radio))
				{
					objects[posRet] = objCol;
					posRet++;
					if(posRet >= maxObjects)
						return posRet;
				}
			}
		}
	}
	
	return posRet;
}
		
// Let position and ratio return object collisioned or null if not collision
// Not consider obj as object collisioned.
Object* Collision::checkCollision(vec2& pos, float radio, Object* obj)
{
	// Check first in pathfinder (is more efficient).
	bool withMap;
	Object* collisioned;
	if(this->pathfinder->getCollision(pos, radio, obj, collisioned, withMap))
	{
		if(collisioned != NULL)
		{
			vec2 pos_ = collisioned->getPos();
			if(pos.dist(pos_) < (radio+collisioned->radio))
				return collisioned;
		}
	}
	
	int x = pos.x / this->quadSideMeters;
	int y = pos.y / this->quadSideMeters;

	if(x < 0 || x >= QUADS_SIDE || y < 0 || y >= QUADS_SIDE)
		return NULL;
	
	// Check the quad.
	list<Object*> *objects = &this->quads[x][y];
	list<Object*>::iterator it;
	for(it = objects->begin(); it != objects->end(); it++)
	{
		Object* objCol = *it;
		
		if( obj != NULL && objCol == obj)
			continue;
		
		vec2 pos_ = objCol->getPos();
		if(pos.dist(pos_) < (radio+objCol->radio))
			return objCol;
	}
	
	// Check arround quads.
	for(int i=x-1; i<=x+1; i++)
	{
		if(i < 0 || i >= QUADS_SIDE)
			continue;
		
		for(int j=y-1; j<=y+1; j++)
		{
			if(j < 0 || j >= QUADS_SIDE)
				continue;
			
			if(i==x && j==y)
				continue;
			
			objects = &this->quads[i][j];
			for(it = objects->begin(); it != objects->end(); it++)
			{
				Object* objCol = *it;
				
				if( obj != NULL && objCol == obj)
					continue;
				
				vec2 pos_ = objCol->getPos();
				if(pos.dist(pos_) < (radio+objCol->radio))
					return objCol;
			}
		}
	}
	
	return NULL;
}
		
// Let object return object collisioned or null if not collision
Object* Collision::checkCollision(Object* obj)
{
	vec2 pos_ = obj->getPos();
	return this->checkCollision(pos_, obj->radio, obj);
}

// Return the near type object or/and equip own (equip -1 is object map, -2 all).
// The objet must be close than distLimit
// If created if true then the object must to have creada more than fullLife
Object* Collision::nearObjectType(vec2& pos, ObjectType type, int equip, float distLimit, char* onlyOfPlayer, bool created)
{
	int x = pos.x / this->quadSideMeters;
	int y = pos.y / this->quadSideMeters;

	if(x < 0 || x >= QUADS_SIDE || y < 0 || y >= QUADS_SIDE)
		return NULL;
	
	// Find from center to edge.
	int xIterStart = x;
	int xIterEnd = x;
	int yIterStart = y;
	int yIterEnd = y;
	int limitIters = (distLimit / 2.0f) / this->quadSideMeters;
	limitIters ++;

	// Check quads and find near.
	// Return the near object.
	auto checkQuad = [&](list<Object*> *objectsList, float& distanceNear)
	{
		Object* near = NULL;
		distanceNear = FLT_MAX;
		
		list<Object*>::iterator it = objectsList->begin();
		list<Object*>::iterator itEnd = objectsList->end();
		while(it != itEnd)
		{
			Object* objCol = *it;
			
			if(!created || (objCol->isUnit() && ((Unit*)objCol)->creada >= ((Unit*)objCol)->fullLife) ||
							(objCol->isBuilding() && ((Building*)objCol)->creada >= ((Building*)objCol)->fullLife) )
			{
				if( (type == OT_ALL || objCol->type == type) && (equip == -2 || objCol->equip == equip) )
				{
					// If there is a owner filter.
					if(onlyOfPlayer == NULL || (objCol->getPlayerName() != NULL && strcmp(onlyOfPlayer, objCol->getPlayerName())==0) )
					{
						vec2 pos_ = objCol->getPos();
						float dist = pos.dist(pos_);
						if(dist < distanceNear && dist <= distLimit)
						{
							near = objCol;
							distanceNear = dist;
						}
					}
				}
			}
			
			it++;
		}
		
		return near;
	};
	
	// When I find a near object then i execute another iter because it's possible there is an object near in the next quad.
	bool lastIter = false;
	
	float distanceNear = FLT_MAX;
	Object* near = NULL;
	for(int d=0; d<=limitIters; d++)
	{
		// Down and up.
		int i = max(xIterStart,0);
		while(i <= xIterEnd)
		{
			if(i >= QUADS_SIDE)
				break;
			
			// Down.
			float distanceNearDown;
			Object* nearDown = NULL;
			if(yIterStart >= 0)
				nearDown = checkQuad(&this->quads[i][yIterStart], distanceNearDown);

			float distanceNearUp;
			Object* nearUp = NULL;
			if(yIterEnd < QUADS_SIDE && yIterStart != yIterEnd)
				nearUp = checkQuad(&this->quads[i][yIterEnd], distanceNearUp);
			
			if(nearDown != NULL)
			{
				if(nearUp != NULL)
				{
					if(distanceNearDown < distanceNearUp)
					{
						if(distanceNearDown < distanceNear)
						{
							distanceNear = distanceNearDown;
							near = nearDown;
						}
					}
					else
					{
						if(distanceNearUp < distanceNear)
						{
							distanceNear = distanceNearUp;
							near = nearUp;
						}
					}
				}
				else
				{
					if(distanceNearDown < distanceNear)
					{
						distanceNear = distanceNearDown;
						near = nearDown;
					}
				}
			}
			else if(nearUp != NULL)
			{
				if(distanceNearUp < distanceNear)
				{
					distanceNear = distanceNearUp;
					near = nearUp;
				}
			}
			
			i++;
		}
		
		// Left and right.
		int j = max(yIterStart+1, 0);
		while(j < yIterEnd)
		{
			if(j >= QUADS_SIDE)
				break;
			
			// Left.
			float distanceNearLeft;
			Object* nearLeft = NULL;
			if(xIterStart >= 0)
				nearLeft = checkQuad(&this->quads[xIterStart][j], distanceNearLeft);
			
			float distanceNearRight;
			Object* nearRight = NULL;
			if(xIterEnd < QUADS_SIDE && xIterStart != xIterEnd)
				nearRight = checkQuad(&this->quads[xIterEnd][j], distanceNearRight);
			
			if(nearLeft != NULL)
			{
				if(nearRight != NULL)
				{
					if(distanceNearLeft < distanceNearRight)
					{
						if(distanceNearLeft < distanceNear)
						{
							distanceNear = distanceNearLeft;
							near = nearLeft;
						}
					}
					else
					{
						if(distanceNearRight < distanceNear)
						{
							distanceNear = distanceNearRight;
							near = nearRight;
						}
					}
				}
				else
				{
					if(distanceNearLeft < distanceNear)
					{
						distanceNear = distanceNearLeft;
						near = nearLeft;
					}
				}
			}
			else if(nearRight != NULL)
			{
				if(distanceNearRight < distanceNear)
				{
					distanceNear = distanceNearRight;
					near = nearRight;
				}
			}
			
			j++;
		}
		
		if(near != NULL)
		{
			if(lastIter)
				return near;
			else
				lastIter = true;
		}
		
		// Magnify.
		xIterStart--;
		xIterEnd++;
		yIterStart--;
		yIterEnd++;
	}

	return near;
}

// Check fast collision with buildings.
bool Collision::getBuildingsCollision(vec2 &pos, float radio, Object* ignored, Object* &collisioned, bool &withMap)
{
	return this->pathfinder->getCollision(pos, radio, ignored, collisioned, withMap);
}

bool Collision::getBuildingsCollision(Object* obj, Object* &collisioned, bool &withMap)
{
	return this->pathfinder->getCollision(obj, collisioned, withMap);
}

// Return:
//    Checkpoints number if ok.
//    -1 if there isn't path.
//    -2 if end is far.
int Collision::getPath(vec2 &start, vec2 &end, float radio, Object* ignoredObject, vec2* checkpoints, int max)
{
	return this->pathfinder->getPath(start, end, radio, ignoredObject, checkpoints, max);
}










