
// It's used for the check fast collisions and path finder.
// Only contains buildings.


#ifndef _pahtfinder_h_
#define _pahtfinder_h_

#include "../../defs.h"
#include "../../config.h"
#include "../../time.h"
#include "../vec2.h"
#include <list>

using namespace std;

// The size of quads of minimap.
#define QUADS_METERS 2

// The side of map area that search the path.
#define SIDE_MAP_PATH_FINDER 500

// Size of quads dimension.
#define SIDE_QUADS (SIDE_MAP_METERS/QUADS_METERS)

class PathFinder
{
	private:
		
		struct QuadMiniMap
		{
			bool terrainOcuped;
			int cant;
			Object* obj[4];
		};
		struct QuadMiniMap quads[SIDE_QUADS][SIDE_QUADS];
		
		bool quadsIntersect(int i, int j, vec2 &pos, float radio);
		bool canWalk(int x, int y, float radio, Object* ignoredObject);
		void findFreeQuad(int &gx, int &gy, float radio, Object* ignoredObject);
		
		void saveObjectsMap(int sx, int sy, int width, int height);
		
		// Path finder.
		float costsInf[SIDE_MAP_PATH_FINDER][SIDE_MAP_PATH_FINDER];
		float weightQuads(int x1, int y1, int x2, int y2, float radio, Object* ignoredObject);
		int astar(int sx, int sy, int gx, int gy, float radio, vec2* paths, int max, Object* ignoredObject);
		
	public:
		
		PathFinder();
		~PathFinder();
		
		void update(int ms);
		
		void add(Object* obj);
		void remove(Object* obj);
		
		bool getCollision(vec2 &pos, float radio, Object* ignored, Object* &collisioned, bool &withMap);
		bool getCollision(Object* obj, Object* &collisioned, bool &withMap);
		
		// Return:
		//    Checkpoints number if ok.
		//    -1 if there isn't path.
		//    -2 if end is far.
		int getPath(vec2 &start, vec2 &end, float radio, Object* ignoredObject, vec2* checkpoints, int max);
};

#endif
