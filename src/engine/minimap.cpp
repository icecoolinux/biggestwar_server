
#include "minimap.h"
#include <stdio.h>
#include "objects/object.h"
#include "players.h"
#include "map.h"
#include <queue>
#include <limits>
#include <cmath>
#include <string.h>
#include <list>

#include "../compress_image.h"
#include <math.h>

MiniMap::MiniMap(Players* players, Map* map)
{
	this->players = players;
	this->map = map;
	this->lastPreMiniMap = 0;
	
	createMiniMapVars(&this->miniMap256, RES_MINIMAP_UI_256);
	createMiniMapVars(&this->miniMap512, RES_MINIMAP_UI_512);
	createMiniMapVars(&this->miniMap1024, 1024);
	createMiniMapVars(&this->miniMap2048, 2048);
}

MiniMap::~MiniMap()
{
	destroyMiniMapVars(&this->miniMap256);
	destroyMiniMapVars(&this->miniMap512);
	destroyMiniMapVars(&this->miniMap1024);
	destroyMiniMapVars(&this->miniMap2048);
}

void MiniMap::update(int ms)
{
	if( (Time::currentMs() - this->lastPreMiniMap) > MS_GENERATE_MINIMAP)
	{
		doCommonWork();
		preMakeMiniMap(&this->miniMap2048);
		preMakeMiniMap(&this->miniMap1024);
		preMakeMiniMap(&this->miniMap512);
		preMakeMiniMap(&this->miniMap256);
		this->lastPreMiniMap = Time::currentMs();
	}
}


unsigned long long MiniMap::getLastPreMiniMap()
{
	return this->lastPreMiniMap;
}

void MiniMap::getMiniMap(char* playerName, int equip, unsigned char* map, int res, int zoom, int posx, int posy, bool invertY)
{
	if(res != RES_MINIMAP_UI_256 && res != RES_MINIMAP_UI_512)
		res = RES_MINIMAP_UI_256;

	// Calculate the dimensions
	float dimx = SIDE_MAP_METERS;
	float dimy = SIDE_MAP_METERS;
	for(int z=1; z<zoom; z++)
	{
		dimx /= 2.0f;
		dimy /= 2.0f;
	}
	
	// Adjust pos
	if(posx < 0)
		posx = 0;
	if(posy < 0)
		posy = 0;
	if( (posx+dimx+2) >= SIDE_MAP_METERS)
		posx = SIDE_MAP_METERS-dimx-2;
	if( (posy+dimy+2) >= SIDE_MAP_METERS)
		posy = SIDE_MAP_METERS-dimy-2;
	
	// Transform world coordinates to minimap imagen coordiante (also with zoom)
	float posxCoordF = ((float)posx) * ( ((float)(res*4)) / ((float)SIDE_MAP_METERS) );
	float posyCoordF = ((float)posy) * ( ((float)(res*4)) / ((float)SIDE_MAP_METERS) );
	for(int z=3; z>zoom; z--)
	{
		posxCoordF /= 2.0f;
		posyCoordF /= 2.0f;
	}
	int posxCoord = posxCoordF;
	int posyCoord = posyCoordF;
	
	// Copy pre minimap.
	for(int i=0; i<res; i++) // Rows
	{
		int indexIMap = i;
		if(invertY)
			indexIMap = res-i-1;
		
		for(int j=0; j<res; j++) // Columns
		{
			int jCommon = posxCoord + j;
			jCommon *= 3;
			int iCommon = posyCoord + i;
			
			for(int k=0; k<3; k++)
			{
				if(zoom == 1)
				{
					if(res == RES_MINIMAP_UI_256)
						map[indexIMap*res*3 +j*3+k] = this->miniMap256.miniMapCommon[equip][iCommon*256*3 +jCommon+k];
					else
						map[indexIMap*res*3 +j*3+k] = this->miniMap512.miniMapCommon[equip][iCommon*512*3 +jCommon+k];
				}
				else if(zoom == 2)
				{
					if(res == RES_MINIMAP_UI_256)
						map[indexIMap*res*3 +j*3+k] = this->miniMap512.miniMapCommon[equip][iCommon*512*3 +jCommon+k];
					else
						map[indexIMap*res*3 +j*3+k] = this->miniMap1024.miniMapCommon[equip][iCommon*1024*3 +jCommon+k];
				}
				else if(zoom == 3)
				{
					if(res == RES_MINIMAP_UI_256)
						map[indexIMap*res*3 +j*3+k] = this->miniMap1024.miniMapCommon[equip][iCommon*1024*3 +jCommon+k];
					else
						map[indexIMap*res*3 +j*3+k] = this->miniMap2048.miniMapCommon[equip][iCommon*2048*3 +jCommon+k];
				}
			}
		}
	}
	
	
	float ratioToMipMap = ((float)res) / ((float)SIDE_MAP_METERS);
	for(int z=1; z<zoom; z++)
		ratioToMipMap *= 2.0f;
	
	// Put my objects.
	list<vec2>::iterator itPos = this->posList[equip].begin();
	list<float>::iterator itRadio = this->radioList[equip].begin();
	list<char*>::iterator itName = this->nameList[equip].begin();
	while(itPos != this->posList[equip].end())
	{
		// If is not my object continue to next.
		if( strcmp(*itName, playerName) != 0)
		{
			itPos++;
			itRadio++;
			itName++;
			continue;
		}

		int x = (*itPos).x;
		int y = (*itPos).y;
		if( x < posx || y < posy || x > (posx+dimx) || y > (posy+dimy) )
		{
			itPos++;
			itRadio++;
			itName++;
			continue;
		}
		
		x = (*itPos).x * ratioToMipMap;
		y = (*itPos).y * ratioToMipMap;
		float radio = *itRadio * ratioToMipMap;
		
		x -= posxCoord;
		y -= posyCoord;
		
		/*
		int x = (*itPos).x;
		int y = (*itPos).y;
		float radio = *itRadio;
		
		if( x < posx || y < posy || x > (posx+dimx) || y > (posy+dimy) )
		{
			itPos++;
			itRadio++;
			itName++;
			continue;
		}
		
		x -= posx;
		y -= posy;
		
		x *= ratioToMipMap;
		y *= ratioToMipMap;
		radio *= ratioToMipMap;
		*/
		putObjectToMap(y, x, invertY, radio, map, res, COLOR_R_CODE_MINE, COLOR_G_CODE_MINE, COLOR_B_CODE_MINE);

		itPos++;
		itRadio++;
		itName++;
	}
}



void MiniMap::createMiniMapVars(struct MiniMapVars* m, int res)
{
	m->res = res;
	for(int e=0; e<EQUIPS; e++)
	{
		m->miniMapCommon[e] = new unsigned char[res*res*3];
		m->knowZones[e] = new bool[res*res];
		m->knowZonesGray[e] = new unsigned char[res*res*3];
		for(int i=0; i<res; i++)
		{
			int posi = i*res*3;
			
			// Init knowZone as all dark
			for(int j=0; j<res; j++)
			{
				m->knowZonesGray[e][posi +j*3] = COLOR_CODE_DARK;
				m->knowZonesGray[e][posi +j*3+1] = COLOR_CODE_DARK;
				m->knowZonesGray[e][posi +j*3+2] = COLOR_CODE_DARK;
			}
		}
	}
	m->knowZonesAllFalse_ = new bool[res*res];
	for(int i=0; i<res*res; i++)
		m->knowZonesAllFalse_[i] = false;
}

void MiniMap::destroyMiniMapVars(struct MiniMapVars* m)
{
	for(int e=0; e<EQUIPS; e++)
	{
		delete[] m->miniMapCommon[e];
		delete[] m->knowZones[e];
		delete[] m->knowZonesGray[e];
	}
	delete[] m->knowZonesAllFalse_;
}

void MiniMap::doCommonWork()
{
	this->mapPosList.clear();
	this->mapTypeList.clear();
	for(int i=0; i<EQUIPS; i++)
		this->mapObjectWasSeenByEquip[i].clear();
	
	// Get map objects.
	this->map->getObjectsInfo(this->mapPosList, this->mapTypeList, this->mapObjectWasSeenByEquip);
	
	// Clear players info.
	for(int i=0; i<EQUIPS; i++)
	{
		this->posList[i].clear();
		this->radioList[i].clear();
		this->typeList[i].clear();
		this->nameList[i].clear();
	}
	
	// Get players info.
	for(int e=0; e<EQUIPS; e++)
		this->players->getObjectsInfo(this->posList[e], this->radioList[e], this->typeList[e], this->nameList[e], e);
}

void MiniMap::putObjectToMap(int j, int i, bool invertY, float radio, unsigned char* m, int res, unsigned char colorR, unsigned char colorG, unsigned char colorB)
{
	int rad = ceil(radio);
	for(int a=j-rad+1; a<j+rad; a++)
	{
		int indexA = a;
		if(invertY)
			indexA = res-a-1;
		
		for(int b=i-rad+1; b<i+rad; b++)
		{
			if(a<0 || a>=res || b<0 || b>=res)
				continue;
			
			m[indexA*res*3 +b*3] = colorR;
			m[indexA*res*3 +b*3+1] = colorG;
			m[indexA*res*3 +b*3+2] = colorB;
		}
	}
}
void MiniMap::preMakeMiniMap(struct MiniMapVars* m)
{
	int res = m->res;
		
	// For each equip.
	for(int e=0; e<EQUIPS; e++)
	{
		// Set the gray zones.
		memcpy (m->miniMapCommon[e], m->knowZonesGray[e], sizeof(unsigned char)*res*res*3 );

		// Init know zones.
		memcpy(m->knowZones[e], m->knowZonesAllFalse_, sizeof(bool)*res*res);
			
		float ratioToMipMap = ((float)res) / ((float)SIDE_MAP_METERS);
		int visibility = ((float)VISIBILITY_DISTANCE) * ratioToMipMap;
		
		// Put zone that i know (aliade and my objects).
		list<vec2>::iterator itPos = this->posList[e].begin();
		while(itPos != this->posList[e].end())
		{
			int x = (*itPos).x * ratioToMipMap;
			int y = (*itPos).y * ratioToMipMap;
			
			for(int i=y-visibility; i<=y+visibility; i++)
			{
				for(int j=x-visibility; j<=x+visibility; j++)
				{
					if(i>=0 && j>=0 && i<res && j<res)
					{
						int diffX = (x-j);
						int diffY = (y-i);
						int distpow = diffX*diffX + diffY*diffY;
						
						if(distpow <= (visibility*visibility +1) )
						{
							m->knowZones[e][i*res +j] = true;
							
							m->knowZonesGray[e][i*res*3 +j*3] = COLOR_R_CODE_KNOWN;
							m->knowZonesGray[e][i*res*3 +j*3+1] = COLOR_G_CODE_KNOWN;
							m->knowZonesGray[e][i*res*3 +j*3+2] = COLOR_B_CODE_KNOWN;
							
							m->miniMapCommon[e][i*res*3 +j*3] = COLOR_R_CODE_VISIBLE;
							m->miniMapCommon[e][i*res*3 +j*3+1] = COLOR_G_CODE_VISIBLE;
							m->miniMapCommon[e][i*res*3 +j*3+2] = COLOR_B_CODE_VISIBLE;
						}
					}
				}
			}
			
			itPos++;
		}
		
		// Put map objects.
		itPos = this->mapPosList.begin();
		list<bool>::iterator itWasSeenObjectMap = this->mapObjectWasSeenByEquip[e].begin();
		while(itPos != this->mapPosList.end())
		{
			//If i know it.
			if( *itWasSeenObjectMap )
			{
				if( (*itPos).x > 0 && (*itPos).x < SIDE_MAP_METERS && (*itPos).y > 0 && (*itPos).y < SIDE_MAP_METERS )
				{
					int x = (*itPos).x * ratioToMipMap;
					int y = (*itPos).y * ratioToMipMap;
					float radio = RADIO_MINERAL * ratioToMipMap;
					
					putObjectToMap(y, x, false, radio, m->miniMapCommon[e], res, COLOR_R_CODE_MAP, COLOR_G_CODE_MAP, COLOR_B_CODE_MAP);
				}
			}
			
			itPos++;
			itWasSeenObjectMap++;
		}
		
		// Put aliade objects.
		itPos = this->posList[e].begin();
		list<float>::iterator itRadio = this->radioList[e].begin();
		while(itPos != this->posList[e].end())
		{
			if( (*itPos).x > 0 && (*itPos).x < SIDE_MAP_METERS && (*itPos).y > 0 && (*itPos).y < SIDE_MAP_METERS )
			{
				int x = (*itPos).x * ratioToMipMap;
				int y = (*itPos).y * ratioToMipMap;
				float radio = *itRadio * ratioToMipMap;;

				// Put aliade objects.
				putObjectToMap(y, x, false, radio, m->miniMapCommon[e], res, COLOR_R_CODE_ALIADE, COLOR_G_CODE_ALIADE, COLOR_B_CODE_ALIADE);
			}
			
			itPos++;
			itRadio++;
		}
		
		// Put enemy objects.
		for(int e2=0; e2<EQUIPS; e2++)
		{
			if(e2 == e)
				continue;
			
			itPos = this->posList[e2].begin();
			itRadio = this->radioList[e2].begin();
			while(itPos != this->posList[e2].end())
			{
				if( (*itPos).x > 0 && (*itPos).x < SIDE_MAP_METERS && (*itPos).y > 0 && (*itPos).y < SIDE_MAP_METERS )
				{
					int x = (*itPos).x * ratioToMipMap;
					int y = (*itPos).y * ratioToMipMap;
					float radio = *itRadio * ratioToMipMap;;
					
					//If i know it.
					if(m->knowZones[e][y*res+x])
						putObjectToMap(y, x, false, radio, m->miniMapCommon[e], res, COLOR_R_CODE_ENEMY, COLOR_G_CODE_ENEMY, COLOR_B_CODE_ENEMY);
				}
				
				itPos++;
				itRadio++;
			}
		}
	}
}







