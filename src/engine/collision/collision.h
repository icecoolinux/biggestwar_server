#ifndef _collision_h_
#define _collision_h_

#include "../../defs.h"
#include "../../config.h"
#include "../vec2.h"
#include <unordered_map>
#include <list>
#include <math.h>

#define QUADS_SIDE 300

using namespace std;

class Collision
{
	private:
		
		float quadSideMeters;
		list<Object*> quads[QUADS_SIDE][QUADS_SIDE];
		list<Object*> quadOutMap;
		// Pointer to int is a 2-array within x and y quad.
		unordered_map<Object*, int*> objToQuad; 
		
		Terrain* terrain;
		PathFinder* pathfinder;
		
	public:
		
		Collision(Terrain* terrain);
		~Collision();

		void addObject(Object* obj);
		void updateObject(Object* obj);
		void removeObject(Object* obj);
		
		// Dado un objeto, lo mueve en direccion 'dir' con una distancia 'dist'.
		// Return obj collisioned or NULL if not collisioned.
//		Object* move(Object* obj, vec2& dir, float dist);
		
		// return a position without collision for an object, near to another object.
		vec2 getPosCollisionFree(vec2& posNear, float radioNear, float radio);
		
		// Return all object from a area.
		// return number of objects.
		int getObjects(float xMin, float xMax, float yMin, float yMax, Object** objects, int maxObjects);
		
		// Return all object from a circle area.
		// return number of objects.
		int getObjects(vec2& center, float radio, Object** objects, int maxObjects);
		
		// Let position and ratio return object collisioned or null if not collision
		// Not consider obj as object collisioned.
		Object* checkCollision(vec2& pos, float radio, Object* obj);
		
		// Let object return object collisioned or null if not collision
		Object* checkCollision(Object* obj);
		
		// Return the near type object or/and equip own (equip -1 is object map, -2 all).
		// The objet must be close than distLimit
		// If created if true then the object must to have creada more than fullLife
		Object* nearObjectType(vec2& pos, ObjectType type, int equip, float distLimit, char* onlyOfPlayer, bool created);
		
		// Check fast collision with buildings.
		bool getBuildingsCollision(vec2 &pos, float radio, Object* ignored, Object* &collisioned, bool &withMap);
		bool getBuildingsCollision(Object* obj, Object* &collisioned, bool &withMap);
		
		// Return:
		//    Checkpoints number if ok.
		//    -1 if there isn't path.
		//    -2 if end is far.
		int getPath(vec2 &start, vec2 &end, float radio, Object* ignoredObject, vec2* checkpoints, int max);
};

#endif
