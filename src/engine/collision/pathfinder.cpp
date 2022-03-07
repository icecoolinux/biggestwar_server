
#include "pathfinder.h"
#include <stdio.h>
#include <queue>
#include <limits>
#include <cmath>
#include <string.h>
#include <list>
#include "../objects/object.h"


PathFinder::PathFinder()
{
	for(int i=0; i<SIDE_QUADS; i++)
	{
		for(int j=0; j<SIDE_QUADS; j++)
		{
			this->quads[i][j].terrainOcuped = false;
			this->quads[i][j].cant = 0;
			for(int k=0; k<4; k++)
				this->quads[i][j].obj[k] = NULL;
		}
	}
	
	//Generate costs infinite for speed up path finder.
	const float INF = std::numeric_limits<float>::infinity();
	for (int i=0; i<SIDE_MAP_PATH_FINDER; ++i)
		for(int j=0; j<SIDE_MAP_PATH_FINDER; ++j)
			this->costsInf[i][j] = INF;
}

PathFinder::~PathFinder()
{
}

bool PathFinder::quadsIntersect(int i, int j, vec2 &pos, float radio)
{
	// Check center to object.
	vec2 p = vec2(i*QUADS_METERS + QUADS_METERS/2.0f, j*QUADS_METERS + QUADS_METERS/2.0f);
	
	if(p.dist(pos) <= (radio + QUADS_METERS/2.0f))
		return true;
	
	// Check corner points.
	else
	{
		// Down left point.
		p = vec2(i*QUADS_METERS, j*QUADS_METERS);
		if(p.dist(pos) <= radio)
			return true;
		else
		{
			// Top left point.
			p = vec2( (i+1)*QUADS_METERS, j*QUADS_METERS);
			if(p.dist(pos) <= radio)
				return true;
			else
			{
				// Down right point.
				p = vec2(i*QUADS_METERS, (j+1)*QUADS_METERS );
				if(p.dist(pos) <= radio)
					return true;
				else
				{
					// Top right point.
					p = vec2( (i+1)*QUADS_METERS, (j+1)*QUADS_METERS );
					if(p.dist(pos) <= radio)
						return true;
				}
			}
		}
	}
	
	return false;
}

void PathFinder::update(int ms)
{
}

void PathFinder::add(Object* obj)
{
	float radio = obj->radio;

	vec2 pos = obj->getPos();
	int x1 = ((int)(pos.x - radio)) / QUADS_METERS;
	int x2 = ((int)(pos.x + radio)) / QUADS_METERS;
	int y1 = ((int)(pos.y - radio)) / QUADS_METERS;
	int y2 = ((int)(pos.y + radio)) / QUADS_METERS;

	if(x1 < 0) x1 = 0;
	else if(x1 >= SIDE_QUADS) x1 = SIDE_QUADS-1;
	if(x2 < 0) x2 = 0;
	else if(x2 >= SIDE_QUADS) x2 = SIDE_QUADS-1;
	if(y1 < 0) y1 = 0;
	else if(y1 >= SIDE_QUADS) y1 = SIDE_QUADS-1;
	if(y2 < 0) y2 = 0;
	else if(y2 >= SIDE_QUADS) y2 = SIDE_QUADS-1;

	vec2 p;
	for(int i=x1; i<=x2; i++)
	{
		for(int j=y1; j<=y2; j++)
		{
			if(quadsIntersect(i, j, pos, radio))
			{
				this->quads[i][j].obj[this->quads[i][j].cant] = obj;
				this->quads[i][j].cant++;
			}
		}
	}
}

void PathFinder::remove(Object* obj)
{
	float radio = obj->radio;

	vec2 pos = obj->getPos();
	int x1 = ((int)(pos.x - radio)) / QUADS_METERS;
	int x2 = ((int)(pos.x + radio)) / QUADS_METERS;
	int y1 = ((int)(pos.y - radio)) / QUADS_METERS;
	int y2 = ((int)(pos.y + radio)) / QUADS_METERS;

	if(x1 < 0) x1 = 0;
	else if(x1 >= SIDE_QUADS) x1 = SIDE_QUADS-1;
	if(x2 < 0) x2 = 0;
	else if(x2 >= SIDE_QUADS) x2 = SIDE_QUADS-1;
	if(y1 < 0) y1 = 0;
	else if(y1 >= SIDE_QUADS) y1 = SIDE_QUADS-1;
	if(y2 < 0) y2 = 0;
	else if(y2 >= SIDE_QUADS) y2 = SIDE_QUADS-1;
	
	for(int i=x1; i<=x2; i++)
	{
		for(int j=y1; j<=y2; j++)
		{
			// Remove it.
			bool removeit = false;
			for(int k=0; k<this->quads[i][j].cant; k++)
			{
				if(this->quads[i][j].obj[k] == obj)
				{
					removeit = true;
					for(int m=k; m<3; m++)
						this->quads[i][j].obj[k] = this->quads[i][j].obj[k+1];
					this->quads[i][j].obj[3] = NULL;
					break;
				}
			}
			if(removeit)
				this->quads[i][j].cant--;
		}
	}
}

bool PathFinder::getCollision(vec2 &pos, float radio, Object* ignored, Object* &collisioned, bool &withMap)
{
	int x1 = ((int)(pos.x - radio)) / QUADS_METERS;
	int x2 = ((int)(pos.x + radio)) / QUADS_METERS;
	int y1 = ((int)(pos.y - radio)) / QUADS_METERS;
	int y2 = ((int)(pos.y + radio)) / QUADS_METERS;

	if(x1 < 0) x1 = 0;
	else if(x1 >= (SIDE_MAP_METERS/QUADS_METERS)) x1 = (SIDE_MAP_METERS/QUADS_METERS)-1;
	if(x2 < 0) x2 = 0;
	else if(x2 >= (SIDE_MAP_METERS/QUADS_METERS)) x2 = (SIDE_MAP_METERS/QUADS_METERS)-1;
	if(y1 < 0) y1 = 0;
	else if(y1 >= (SIDE_MAP_METERS/QUADS_METERS)) y1 = (SIDE_MAP_METERS/QUADS_METERS)-1;
	if(y2 < 0) y2 = 0;
	else if(y2 >= (SIDE_MAP_METERS/QUADS_METERS)) y2 = (SIDE_MAP_METERS/QUADS_METERS)-1;
	
	for(int i=x1; i<=x2; i++)
	{
		for(int j=y1; j<=y2; j++)
		{
			if(quadsIntersect(i, j, pos, radio))
			{
				if(this->quads[i][j].terrainOcuped)
				{
					collisioned = NULL;
					withMap = true;
					return true;
				}
				else if(this->quads[i][j].cant > 0)
				{
					if(ignored == NULL)
					{
						collisioned = this->quads[i][j].obj[0];
						withMap = false;
						return true;
					}
					else if(this->quads[i][j].cant > 1 || ignored != this->quads[i][j].obj[0])
					{
						for(int k=0; k<this->quads[i][j].cant; k++)
						{
							if(this->quads[i][j].obj[k] != ignored)
							{
								collisioned = this->quads[i][j].obj[k];
								break;
							}
						}
						withMap = false;
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

bool PathFinder::getCollision(Object* obj, Object* &collisioned, bool &withMap)
{
	vec2 pos = obj->getPos();
	return this->getCollision(pos, obj->radio, obj, collisioned, withMap);
}

// Return:
//    Checkpoints number if ok.
//    -1 if there isn't path.
//    -2 if end is far.
// TODO make it more efficient.
int PathFinder::getPath(vec2 &start, vec2 &end, float radio, Object* ignoredObject, vec2* checkpoints, int max)
{
	/*
	// Set last point, the end.
	checkpoints[0] = end;
	return 1;
	*/
	int sx = ((int)start.x) / QUADS_METERS;
	int sy = ((int)start.y) / QUADS_METERS;
	int gx = ((int)end.x) / QUADS_METERS;
	int gy = ((int)end.y) / QUADS_METERS;

	if(sx < 0) sx = 0;
	else if(sx >= (SIDE_MAP_METERS/QUADS_METERS)) sx = (SIDE_MAP_METERS/QUADS_METERS)-1;
	if(sy < 0) sy = 0;
	else if(sy >= (SIDE_MAP_METERS/QUADS_METERS)) sy = (SIDE_MAP_METERS/QUADS_METERS)-1;
	if(gx < 0) gx = 0;
	else if(gx >= (SIDE_MAP_METERS/QUADS_METERS)) gx = (SIDE_MAP_METERS/QUADS_METERS)-1;
	if(gy < 0) gy = 0;
	else if(gy >= (SIDE_MAP_METERS/QUADS_METERS)) gy = (SIDE_MAP_METERS/QUADS_METERS)-1;

	// Find the close free quad to go.
	findFreeQuad(gx, gy, radio, ignoredObject);
	
	// If the distance between start and end if greater than SIDE_MAP_PATH_FINDER/2 then return error.
	if( abs(sx-gx) > (SIDE_MAP_PATH_FINDER/2 -1) || abs(sy-gy) > (SIDE_MAP_PATH_FINDER/2 -1) )
		return -2;
	
	int amount = astar(sx, sy, gx, gy, radio, checkpoints, max, ignoredObject);
//saveMiniMap(0,0,100,100);
	if(amount <= 0)
		return -1;
	else
	{
		// Set last point, the end.
		checkpoints[amount-1] = end;
		return amount;
	}
}

void PathFinder::saveObjectsMap(int sx, int sy, int width, int height)
{
	FILE* f = fopen("minimap.txt", "w");
	for(int j=sy+height-1; j>=sy; j--)
	{
		for(int i=sx; i<sx+width-1; i++)
		{
			if(this->quads[i][j].terrainOcuped || this->quads[i][j].cant > 0)
				fputc('X', f);
			else
				fputc('O', f);
		}
		fputc('\n', f);
	}
	fclose(f);
}

void PathFinder::findFreeQuad(int &gx, int &gy, float radio, Object* ignoredObject)
{
	// Find a free quad close to end.
	// Find from center to corner because the center is more close.
	int side = 0;
	while(side < 10)
	{
		bool finded = false;
		for(int i=0; i<=side; i++)
		{
			// Left top.
			int gxTmp = gx-side;
			int gyTmp = gy+i;
			if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
			{
				gx = gxTmp;
				gy = gyTmp;
				finded = true;
				break;
			}
			
			if(side > 0)
			{
				// Right top.
				gxTmp = gx+side;
				gyTmp = gy+i;
				if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
				{
					gx = gxTmp;
					gy = gyTmp;
					finded = true;
					break;
				}
				
				if(i > 0)
				{
					// Left down.
					gxTmp = gx-side;
					gyTmp = gy-i;
					if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
					{
						gx = gxTmp;
						gy = gyTmp;
						finded = true;
						break;
					}
					// Right down.
					gxTmp = gx+side;
					gyTmp = gy-i;
					if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
					{
						gx = gxTmp;
						gy = gyTmp;
						finded = true;
						break;
					}
				}
				
				if(i < side)
				{
					// Top left.
					gxTmp = gx-i;
					gyTmp = gy+side;
					if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
					{
						gx = gxTmp;
						gy = gyTmp;
						finded = true;
						break;
					}
					// Down left.
					gxTmp = gx-i;
					gyTmp = gy-side;
					if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
					{
						gx = gxTmp;
						gy = gyTmp;
						finded = true;
						break;
					}
					
					if(i > 0)
					{
						// Top right.
						gxTmp = gx+i;
						gyTmp = gy+side;
						if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
						{
							gx = gxTmp;
							gy = gyTmp;
							finded = true;
							break;
						}
						// Down right.
						gxTmp = gx+i;
						gyTmp = gy-side;
						if(canWalk(gxTmp, gyTmp, radio, ignoredObject))
						{
							gx = gxTmp;
							gy = gyTmp;
							finded = true;
							break;
						}
					}
				}
			}
		}
		if(finded)
			break;
		side++;
	}
}




// This is when can walk side to the an object.
bool PathFinder::canWalk(int x, int y, float radio, Object* ignoredObject)
{
	int quadsRadio = ((int)radio)/QUADS_METERS;
	if( radio > ( ((float)(quadsRadio*QUADS_METERS)) + 0.0001) )
		quadsRadio += 1;

	for(int i=x-quadsRadio; i<=x+quadsRadio; i++)
	{
		for(int j=y-quadsRadio; j<=y+quadsRadio; j++)
		{
			if(i < 0 || j < 0 || i >= (SIDE_MAP_METERS/QUADS_METERS) || j >= (SIDE_MAP_METERS/QUADS_METERS))
				return false;
			
			if(this->quads[i][j].terrainOcuped)
				return false;
			if(this->quads[i][j].cant > 0 && (ignoredObject == NULL || this->quads[i][j].cant > 1 || ignoredObject != this->quads[i][j].obj[0]) )
				return false;
		}
	}
	
	return true;
}

// represents a single pixel
class Node {
	public:
		int x, y;
		float cost;  // cost of traversing this pixel

		Node(int x, int y, float c)
		{
			this->x = x;
			this->y = y;
			this->cost = c;
		}
};

// the top of the priority queue is the greatest element by default,
// but we want the smallest, so flip the sign
bool operator<(const Node &n1, const Node &n2) {
	return n1.cost > n2.cost;
}

bool operator==(const Node &n1, const Node &n2) {
	return n1.x == n2.x && n1.y == n2.y;
}

// See for various grid heuristics:
// http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#S7
// L_\inf norm (diagonal distance)
float linf_norm(int i0, int j0, int i1, int j1) {
	int dx = i0 - i1;
	int dy = j0 - j1;
	return dx*dx + dy*dy;
}

float PathFinder::weightQuads(int x1, int y1, int x2, int y2, float radio, Object* ignoredObject)
{
	// Check a super quad nxn quads.
	if(!canWalk(x2, y2, radio, ignoredObject))
		return SIDE_MAP_PATH_FINDER * SIDE_MAP_PATH_FINDER * 10;
	
	if( (x1 == x2) || (y1 == y2) )
		return 1;
	else
		return M_SQRT2;
}

// Return amount chackpoints, -1 if there isn't path.
int PathFinder::astar(int sx, int sy, int gx, int gy, float radio, vec2* paths, int max, Object* ignoredObject) 
{
	// Traslate the positions to a problem between 0 to SIDE_MAP_PATH_FINDER-1.
	int shiftX = (SIDE_MAP_PATH_FINDER/2 - sx);
	int shiftY = (SIDE_MAP_PATH_FINDER/2 - sy);
	sx += shiftX;
	sy += shiftY;
	gx += shiftX;
	gy += shiftY;

	int paths_map[SIDE_MAP_PATH_FINDER][SIDE_MAP_PATH_FINDER][2];
	
	Node start_node(sx, sy, 0.);
	Node goal_node(gx, gy, 0.);

	float costs[SIDE_MAP_PATH_FINDER][SIDE_MAP_PATH_FINDER];
	memcpy(costs, costsInf, sizeof(float)*SIDE_MAP_PATH_FINDER*SIDE_MAP_PATH_FINDER);
	costs[sx][sy] = 0.;

	std::priority_queue<Node> nodes_to_visit;
	nodes_to_visit.push(start_node);

	int nbrsX[8];
	int nbrsY[8];

	int steps=0;
	bool solution_found = false;
	while (!nodes_to_visit.empty()) 
	{
		// Stop, huge time.
		steps++;
		if(steps >= SIDE_MAP_PATH_FINDER*10)
			break;
		
		Node cur = nodes_to_visit.top();

		if (cur == goal_node) {
			solution_found = true;
			break;
		}

		nodes_to_visit.pop();

		int x = cur.x;
		int y = cur.y;
		
	
		// check bounds and find up to eight neighbors: top to bottom, left to right
		nbrsX[0] = x-1;
		if(x > 0 && y > 0)
			nbrsY[0] = y-1;
		else
			nbrsX[0] = -1;
		
		nbrsX[1] = x;
		if(y > 0)
			nbrsY[1] = y-1;
		else
			nbrsX[1] = -1;
		
		nbrsX[2] = x+1;
		if( (x+1) < SIDE_MAP_PATH_FINDER && y > 0)
			nbrsY[2] = y-1;
		else
			nbrsX[2] = -1;
		
		
		
		
		nbrsX[3] = x-1;
		if(x > 0)
			nbrsY[3] = y;
		else
			nbrsX[3] = -1;
		
		nbrsX[4] = x+1;
		if( (x+1) < SIDE_MAP_PATH_FINDER )
			nbrsY[4] = y;
		else
			nbrsX[4] = -1;
		
		
		
		
		nbrsX[5] = x-1;
		if(x > 0 && (y+1) < SIDE_MAP_PATH_FINDER )
			nbrsY[5] = y+1;
		else
			nbrsX[5] = -1;

		nbrsX[6] = x;
		if( (y+1) < SIDE_MAP_PATH_FINDER )
			nbrsY[6] = y+1;
		else
			nbrsX[6] = -1;

		nbrsX[7] = x+1;
		if( (x+1) < SIDE_MAP_PATH_FINDER && (y+1) < SIDE_MAP_PATH_FINDER )
			nbrsY[7] = y+1;
		else
			nbrsX[7] = -1;

		
		
		float heuristic_cost;
		for (int i = 0; i < 8; ++i) {
			if (nbrsX[i] >= 0) 
			{
				// the sum of the cost so far and the cost of this move
				float new_cost = costs[cur.x][cur.y] + weightQuads(cur.x-shiftX, cur.y-shiftY, nbrsX[i]-shiftX, nbrsY[i]-shiftY, radio, ignoredObject);
				
				if (new_cost < costs[nbrsX[i]][nbrsY[i]]) 
				{
					// estimate the cost to the goal based on legal moves
					heuristic_cost = linf_norm(nbrsX[i], nbrsY[i], gx, gy);

					// paths with lower expected cost are explored first
					float priority = new_cost + heuristic_cost;

					nodes_to_visit.push(Node(nbrsX[i], nbrsY[i], priority));

					costs[nbrsX[i]][nbrsY[i]] = new_cost;

					paths_map[nbrsX[i]][nbrsY[i]][0] = cur.x;
					paths_map[nbrsX[i]][nbrsY[i]][1] = cur.y;
				}
			}
		}
	}

	int amount = 0;
	if(solution_found)
	{
		int pos = max-1;
		while(gx != sx || gy != sy )
		{
			paths[pos].x = ( (gx-shiftX)*QUADS_METERS) + QUADS_METERS/2.0f;
			paths[pos].y = ( (gy-shiftY)*QUADS_METERS) + QUADS_METERS/2.0f;
			pos--;
			
			if(pos == -1)
				break;
			
			int gx_tmp = paths_map[gx][gy][0];
			int gy_tmp = paths_map[gx][gy][1];
			gx = gx_tmp;
			gy = gy_tmp;
		}
		if(pos >= 0)
		{
			paths[pos].x = ( (gx-shiftX)*QUADS_METERS) + QUADS_METERS/2.0f;
			paths[pos].y = ( (gy-shiftY)*QUADS_METERS) + QUADS_METERS/2.0f;
		}
		// Shift pos to position 0.
		if(pos > 0)
		{
			while(pos < max)
			{
				paths[amount] = paths[pos];
				amount++;
				pos++;
			}
		}
	}
//printf("%d %d %d\n", solution_found, steps, amount);
	if(solution_found)
		return amount;
	else
		return -1;
}



