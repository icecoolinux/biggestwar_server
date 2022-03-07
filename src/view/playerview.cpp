
#include "playerview.h"
#include "objectview.h"
#include "actionview.h"
#include "../engine/map.h"
#include "../semaphore.h"
#include "../engine/world.h"
#include "../engine/collision/collision.h"
#include "../engine/objects/object.h"
#include "../engine/objects/mineral.h"
#include <string.h>

PlayerView::PlayerView(char* nameCurrent, int equipCurrent, bool bot)
{
	strcpy(this->nameCurrent, nameCurrent);
	this->equipCurrent = equipCurrent;
	this->bot = bot;
	
	this->mineral = 0;
	this->oil = 0;
	
	for(int e=0; e<EQUIPS; e++)
		for(int i=0; i<MAX_PLAYERS_BY_EQUIP; i++)
			this->enabledBasesZones[e][i] = false;
	
	lastTsUpdateArea = 0;
	areaIsClosing = false;
		
	this->gameEnd = false;
	
	this->miniMapRes = RES_MINIMAP_UI_256;
	this->miniMapZoom = 1;
	this->miniMapPosX = 0;
	this->miniMapPosY = 0;
	this->settingsChanged = true;
	this->miniMap = new unsigned char[RES_MINIMAP_UI_512*RES_MINIMAP_UI_512*3];
	this->compress_minimap = NULL;
	this->timeLastMiniMap = 0;
	this->size_minimap = -1;
	this->isPNG = true;
	this->nextUsePNG = true;
	
	this->semWaitUpdate = new Semaphore(0);
	this->semWaitFinish = new Semaphore(0);
}

PlayerView::~PlayerView()
{
	delete this->semWaitUpdate;
	delete this->semWaitFinish;
	
	while(this->playersName.size() > 0)
	{
		char* p = this->playersName.front();
		this->playersName.pop_front();
		delete[] p;
	}

	while(this->objects.size() > 0)
	{
		ObjectView* o = this->objects.front();
		this->objects.pop_front();
		delete o;
	}
	
	delete[] this->miniMap;
	
	// Remove memory if there is.
	if(this->compress_minimap != NULL)
		free(this->compress_minimap);
}

void PlayerView::getName(char* name)
{
	strcpy(name, this->nameCurrent);
}

int PlayerView::getEquip()
{
	return this->equipCurrent;
}

bool PlayerView::isBot()
{
	return this->bot;
}

// Block waiting for update.
void PlayerView::block()
{
	this->semWaitUpdate->P();
}
		
// Release for update.
void PlayerView::release()
{
	this->semWaitUpdate->V();
}
		
// Update view, return true if update ok.
bool PlayerView::update(World* world, 
						float xMin, float xMax, float yMin, float yMax, 
						list<unsigned long long> idsSelected, 
						bool end, int rank, bool surrender, int teamWin, int winToEnd)
{
	PlayerViewUpdate ud;
	
	/// End game or I was destroyed or I surrender.
	if(end)
	{
		this->gameEnd = true;
		
		
		ud.init();
		ud.endGame = true;
		ud.surrenderEndGame = surrender;
		
		if(teamWin == 1)
		{
			ud.winEndGame = true;
			ud.loseEndGame = false;
			ud.gameFinishEndGame = true;
		}
		else if(teamWin == 0)
		{
			ud.winEndGame = false;
			ud.loseEndGame = true;
			ud.gameFinishEndGame = true;
		}
		else
		{
			ud.winEndGame = false;
			ud.loseEndGame = false;
			ud.gameFinishEndGame = false;
		}
		ud.rankEquipEndGame = rank;
		
		this->updates.push_back(ud);
		
		this->semWaitFinish->V();

		return true;
	}

	
	/// Send new and remove players.
	list<char*> newPlayers;
	list<int> newEquip;
	list<char*> removedPlayers;
	world->getPlayers(this->playersName, newPlayers, newEquip, removedPlayers);
	// removed players.
	while(removedPlayers.size() > 0)
	{
		char* rm = removedPlayers.front();
		removedPlayers.pop_front();
		
		ud.init();
		ud.removedPlayer = true;
		strcpy(ud.playerName, rm);
		this->updates.push_back(ud);
		
		list<char*>::iterator itMine;
		for(itMine = this->playersName.begin(); itMine != this->playersName.end(); itMine++)
		{
			if( strcmp(rm, *itMine) == 0)
			{
				char* n = *itMine;
				this->playersName.erase(itMine);
				delete[] n;
				break;
			}
		}
		delete[] rm;
	}
	// new players.
	while(newPlayers.size() > 0)
	{
		char* n = newPlayers.front();
		int e = newEquip.front();
		newPlayers.pop_front();
		newEquip.pop_front();

		ud.init();
		ud.newPlayer = true;
		strcpy(ud.playerName, n);
		ud.equip = e;
		this->updates.push_back(ud);
		
		this->playersName.push_back(n);
	}
	
	/// Send new and deleted bases zones.
	list<bool> isAddBaseZone;
	list<int> equipBaseZone;
	list<vec2> posBaseZone;
	list<int> radioBaseZone;
	if(world->getMap()->updateZones(equipCurrent, this->enabledBasesZones, isAddBaseZone, equipBaseZone, posBaseZone, radioBaseZone))
	{
		ud.init();
		ud.basesZones = true;
		ud.isAddBaseZone = isAddBaseZone;
		ud.equipBaseZone = equipBaseZone;
		ud.posBaseZone = posBaseZone;
		ud.radioBaseZone = radioBaseZone;
		this->updates.push_back(ud);
	}
	
	//Change my player state.
	int currentMineral;
	int currentOil;
	world->getResourcesPlayer(this->equipCurrent, this->nameCurrent, currentMineral, currentOil);
	if(this->mineral != currentMineral || this->oil != currentOil)
	{
		this->mineral = currentMineral;
		this->oil = currentOil;
		
		ud.init();
		ud.updateResource = true;
		ud.mineral = currentMineral;
		ud.oil = currentOil;
		this->updates.push_back(ud);
	}
	
	
	
	/// Get all objects.
	Object* objectsWorld[MAX_OBJECTS_UPDATE_VIEW];
	int numObjects = world->getCollision()->getObjects(xMin, xMax, yMin, yMax, &objectsWorld[0], MAX_OBJECTS_UPDATE_VIEW);

	/// Quite not visibe objects.
	int k = 0;
	while(k < numObjects)
	{
		// If it's an object map and I can see it then continue.
		if( objectsWorld[k]->equip == -1 && ((Mineral*)objectsWorld[k])->wasSeenByEquip[this->equipCurrent] )
		{
			// Nothing, only next object
		}
		// If it's an object map or enemy and I cannot see it then remove it.
		else if( objectsWorld[k]->equip == -1 || objectsWorld[k]->equip != this->equipCurrent )
		{
			// Find if there are an aliade or mine object near.
			bool thereAreNear = false;
			for(int k2 = 0; k2 < numObjects; k2++)
			{
				if(k == k2)
					continue;
				if(objectsWorld[k2]->equip == this->equipCurrent ) // Mine or aliade.
				{
					vec2 posk2 = objectsWorld[k2]->getPos();
					
					// I consider the object map/enemy object radio to calculate the distance
					float dist = objectsWorld[k]->getPos().dist(posk2);
					dist -= objectsWorld[k]->radio; 
					
					if( dist <= VISIBILITY_DISTANCE)
					{
						thereAreNear = true;
						break;
					}
				}
			}
			// I can see it.
			if(thereAreNear)
			{
				// Set as visible the mineral
				if(objectsWorld[k]->equip == -1)
					((Mineral*)objectsWorld[k])->wasSeenByEquip[this->equipCurrent] = true;
			}
			// I cannot see it
			else
			{
				// And remove from objectsWorld
				for(int j=k; j<(numObjects-1); j++)
					objectsWorld[j] = objectsWorld[j+1];
				numObjects--;
				k--;
			}
			
		}
		
		k++;
	}

	/// Check objects isn't or it's not visible.
	list<ObjectView*>::iterator it = this->objects.begin();
	while(it != this->objects.end())
	{
		ObjectView* obj = *it;
		
		obj->inTheView = false;
		for(int i=0; i<numObjects; i++)
		{
			if(obj->id == objectsWorld[i]->id)
			{
				obj->inTheView = true;
				break;
			}
		}
		
		obj->inTheSelection = false;
		list<unsigned long long>::iterator it2 = idsSelected.begin();
		while(it2 != idsSelected.end())
		{
			if(obj->id == *it2)
			{
				obj->inTheSelection = true;
				break;
			}
			it2++;
		}
		
		// It was destroyed.
		if(world->getObjectById(obj->id) == NULL)
		{
			// Quite of tracking.
			if(obj->aliade || obj->mine)
				objectsMineAndAliadeNotifyDestroyed.erase(obj->id);
			
			ud.init();
			ud.wasDeleted = true;
			ud.wasDestroyed = true;
			ud.id = obj->id;
			it = this->objects.erase(it);
			delete obj;
			this->updates.push_back(ud);

			continue;
		}
		// The object wasn't destroyed.
		else
		{
			// Is an unit.
//TODO comente esta linea para borrar hasta las construcciones y liberar espacio del lado del cliente
//			if(obj->type == OT_RECOLECTOR || obj->type == OT_SOLDADO_RASO || obj->type == OT_SOLDADO_ENTRENADO || obj->type == OT_TANQUE || obj->type == OT_TANQUE_PESADO)
			{
				if(obj->enemy)
				{
					// Delete enemy unit, I can't view it.
					if(!obj->inTheView)
					{
						ud.init();
						ud.wasDeleted = true;
						ud.id = obj->id;
						it = this->objects.erase(it);
						delete obj;
						this->updates.push_back(ud);

						continue;
					}
				}
				else if(obj->aliade && !obj->mine)
				{
					// Delete aliade unit, I can't view it and isn't selected.
					if(!obj->inTheView && !obj->inTheSelection)
					{
						ud.init();
						ud.wasDeleted = true;
						ud.id = obj->id;
						it = this->objects.erase(it);
						delete obj;
						this->updates.push_back(ud);
						
						continue;
					}
				}
				else if(obj->mine)
				{
					// The fake object if visible but the real not, i have to update pos.
					if(!obj->inTheView && obj->pos.x >= xMin && obj->pos.x <= xMax && obj->pos.y >= yMin && obj->pos.y <= yMax )
					{
						// Get real object.
						Object* realObject = world->getObjectById(obj->id);
						if(realObject != NULL)
						{
							ud.init();
							ud.wasChanged = true;
							ud.id = obj->id;
							ud.updatePos = true;
							ud.pos = realObject->getPos();
							obj->pos = realObject->getPos();
							this->updates.push_back(ud);
						}
					}
				}
			}
		}
		
		it++;
	}
	
	
	/// Notify mine and aliade objects that was destroyed and there isn't in the view.
	unordered_map<unsigned long long, bool>::iterator itNotify = this->objectsMineAndAliadeNotifyDestroyed.begin();
	while(itNotify != this->objectsMineAndAliadeNotifyDestroyed.end())
	{
		if(world->getObjectById(itNotify->first) == NULL)
		{
			ud.init();
			ud.wasDeleted = true;
			ud.wasDestroyed = true;
			ud.id = itNotify->first;
			this->updates.push_back(ud);
			
			itNotify = this->objectsMineAndAliadeNotifyDestroyed.erase(itNotify);
		}
		else
			itNotify++;
	}
	
	
	/// Check new objects.
	for(int i=0; i<numObjects; i++)
	{
		bool its = false;
		for(it = this->objects.begin(); it != this->objects.end(); it++)
		{
			ObjectView* obj = *it;
			if(obj->id == objectsWorld[i]->id)
			{
				its = true;
				break;
			}
		}
		// It's new.
		if(!its)
		{
			ObjectView* obj = new ObjectView(objectsWorld[i], this->nameCurrent, this->equipCurrent);
			obj->inTheView = true;
			obj->inTheSelection = false;
			obj->getNew(ud);
			this->updates.push_back(ud);
			objects.push_back(obj);

			// If it's mine or aliade add to track.
			if(obj->aliade || obj->mine)
			{
				// Check if object isn't.
				if(objectsMineAndAliadeNotifyDestroyed.find(obj->id) == objectsMineAndAliadeNotifyDestroyed.end())
					objectsMineAndAliadeNotifyDestroyed[obj->id] = true;
			}
		}
	}

	/// Update change on the view or selected objects.
	for(it = this->objects.begin(); it != this->objects.end(); it++)
	{
		ObjectView* obj = *it;

		if(obj->inTheView)
		{
			for(int i=0; i<numObjects; i++)
			{
				if(obj->id == objectsWorld[i]->id)
				{
					if(obj->getChanges(objectsWorld[i], ud))
						this->updates.push_back(ud);
					break;
				}
			}
		}
		else if(obj->inTheSelection)
		{
			Object* objWorld = world->getObjectById(obj->id);
			if(objWorld != NULL)
			{
				if(obj->getChanges(objWorld, ud))
					this->updates.push_back(ud);
			}
		}
	}
	
	/// Add selected objects that I dont have it in the list objects.
	list<unsigned long long>::iterator itIdsSelected = idsSelected.begin();
	while(itIdsSelected != idsSelected.end())
	{
		bool its = false;
		for(it = this->objects.begin(); it != this->objects.end(); it++)
		{
			ObjectView* obj = *it;
			if(*itIdsSelected == obj->id)
			{
				its = true;
				break;
			}
		}
		// It's new.
		if(!its)
		{
			Object* newObj = world->getObjectById(*itIdsSelected);
			if(newObj != NULL)
			{
				ObjectView* obj = new ObjectView(newObj, this->nameCurrent, this->equipCurrent);
				obj->inTheView = false;
				obj->inTheSelection = true;
				obj->getNew(ud);
				this->updates.push_back(ud);
				objects.push_back(obj);

				// If it's mine or aliade add to track.
				if(obj->aliade || obj->mine)
				{
					// Check if object isn't.
					if(objectsMineAndAliadeNotifyDestroyed.find(obj->id) == objectsMineAndAliadeNotifyDestroyed.end())
						objectsMineAndAliadeNotifyDestroyed[obj->id] = true;
				}
			}
		}
		
		itIdsSelected++;
	}
	
	
	/// Update area.
	if( lastTsUpdateArea == 0 ||
		(world->isAreaClosing() && !areaIsClosing) || 
		(!world->isAreaClosing() && areaIsClosing) || 
		(world->isAreaClosing() && (Time::currentMs()-lastTsUpdateArea) >= TIME_TO_UPDATE_AREA) )
	{
		ud.init();
		ud.updateArea = true;
		world->get(ud.areaIsClosing, ud.msToClose, ud.areaFutureCenter, 
					ud.areaCurrentBottom, ud.areaCurrentTop, 
					ud.areaCurrentLeft, ud.areaCurrentRight, 
					ud.areaFutureBottom, ud.areaFutureTop, 
					ud.areaFutureLeft, ud.areaFutureRight, 
					ud.areaSpeedCloseSecBottom, ud.areaSpeedCloseSecTop, 
					ud.areaSpeedCloseSecLeft, ud.areaSpeedCloseSecRight);

		lastTsUpdateArea = Time::currentMs();
		areaIsClosing = ud.areaIsClosing;
		
		this->updates.push_back(ud);
	}
	
	
	// Generate minimap.
	// Send when the user change settings or the minimap sended is old.
	if(this->settingsChanged || this->timeLastMiniMap < world->getMiniMap()->getLastPreMiniMap())
	{
		this->settingsChanged = false;
		
		int res = this->miniMapRes;
		int zoom = this->miniMapZoom;
		int posx = this->miniMapPosX;
		int posy = this->miniMapPosY;
		
		world->getMiniMap()->getMiniMap(this->nameCurrent, this->equipCurrent, this->miniMap, res, zoom, posx, posy, true);
		this->timeLastMiniMap = world->getMiniMap()->getLastPreMiniMap();
		
		// Remove memory if there is.
		if(this->compress_minimap != NULL)
		{
			this->size_minimap = -1;
			free(this->compress_minimap);
			this->compress_minimap = NULL;
		}
	
		// Compress map to png.
		int size = compress_image(this->nextUsePNG, res, res, this->miniMap, this->compress_minimap);
		this->size_minimap = size;
		this->isPNG = this->nextUsePNG;
	}
	
	this->semWaitFinish->V();

	return true;
}

// Block until update finish.
void PlayerView::waitFinish()
{
	this->semWaitFinish->P();
}

// Return true if game endend.
bool PlayerView::isGameEnd()
{
	return this->gameEnd;
}

// Return updates to send.
// return length or -1 if there isn't updates.
int PlayerView::toString(char* msg, int max)
{
	if(this->updates.size() == 0)
		return -1;
	
	int tam = 0;

	while(this->updates.size() > 0)
	{
		int len = this->updates.front().toString(&msg[tam], max-tam-1);
		
		if(len < 0)
			break;
		else
		{
			this->updates.pop_front();
			tam += len;
		}
	}
	
	return tam;
}

// Set minimap settings.
void PlayerView::setMiniMapSetting(int res, int zoom, int posx, int posy)
{
	if(res != RES_MINIMAP_UI_512 && res != RES_MINIMAP_UI_256)
		return;
	
	this->miniMapRes = res;
	this->miniMapZoom = zoom;
	this->miniMapPosX = posx;
	this->miniMapPosY = posy;
	this->settingsChanged = true;
}

void PlayerView::setMiniMapFormat(bool usePNG)
{
	this->nextUsePNG = usePNG;
}

// Return -1 if there isn't or the info length.
// Return if it's png or jpeg otherwise
int PlayerView::getMiniMap(char* info_minimap, int max, bool &png)
{
	if(this->size_minimap < 0)
		return -1;
	if(this->size_minimap >= max)
		return -1;
	
	int size = this->size_minimap;
	this->size_minimap = -1;
	
	png = this->isPNG;
	
	memcpy(info_minimap, this->compress_minimap, size);
	free(this->compress_minimap);
	this->compress_minimap = NULL;
	
	return size;
}




		

