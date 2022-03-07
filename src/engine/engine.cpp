
#include "engine.h"
#include "world.h"
#include "worldresult.h"
#include "../view/playerview.h"
#include "../semaphore.h"
#include <stdio.h>

Engine::Engine(int number)
{
	this->stage = 0;
	
	this->semViews = new Semaphore(1);
	this->semPlayers = new Semaphore(1);
	this->isExiting = false;
	
	this->world = new World(number);
	
	this->results = new WorldResult(this->world);
	
	this->semActions = new Semaphore(1);
}

Engine::~Engine()
{
	delete this->world;
	delete this->semViews;
	delete this->semPlayers;
	
	while(this->newPlayers.size() > 0)
	{
		PlayerEngine* p = this->newPlayers.front();
		delete p->block;
		this->newPlayers.pop_front();
		delete p;
	}
	
	while(this->removePlayers.size() > 0)
	{
		PlayerEngine* p = this->removePlayers.front();
		delete p->block;
		this->removePlayers.pop_front();
		delete p;
	}
	
	delete this->results;
	
	delete this->semActions;
	while(this->actions.size() > 0)
	{
		ActionEngine ae = this->actions.front();
		if(ae.action != NULL)
			delete ae.action;
		this->actions.pop_front();
	}
}

int Engine::getWorldNumber()
{
	return this->world->getNumber();
}

int Engine::getStage()
{
	return stage;
}

int Engine::getAmountPlayers()
{
	return world->getAmountPlayers();
}


/// New - Remove players
bool Engine::newPlayer(char* name, int &xInit, int &yInit, int &equip)
{
	if(this->isExiting || this->isOver())
		return false;
	
	PlayerEngine* p = new PlayerEngine;
	p->name = name;
	p->block = new Semaphore(0);
	
	this->semPlayers->P();
	this->newPlayers.push_back(p);
	this->semPlayers->V();
	
	p->block->P();
	
	xInit = p->xInit;
	yInit = p->yInit;
	equip = p->equip;

	bool ok = p->ok;
	
	delete p->block;
	delete p;
	
	return ok;
}

bool Engine::removePlayer(int equip, char* name)
{
	if(this->isExiting || this->isOver())
		return false;
	
	PlayerEngine* p = new PlayerEngine;
	p->equip = equip;
	p->name = name;
	p->block = new Semaphore(0);
	
	this->semPlayers->P();
	this->removePlayers.push_back(p);
	this->semPlayers->V();
	
	p->block->P();
	
	bool ok = p->ok;
	
	delete p->block;
	delete p;
	
	return ok;
}

void Engine::addRemovePlayers()
{
	this->semPlayers->P();
	// New players.
	while(this->newPlayers.size() > 0)
	{
		PlayerEngine* p = this->newPlayers.front();
		p->ok = this->world->newPlayer(p->name, p->xInit, p->yInit, p->equip);
		p->block->V();
		this->newPlayers.pop_front();
	}
	// Remove players.
	while(this->removePlayers.size() > 0)
	{
		PlayerEngine* p = this->removePlayers.front();
		p->ok = this->world->removePlayer(p->equip, p->name);
		p->block->V();
		this->removePlayers.pop_front();
		
		// Add player as surrender.
		if(p->ok)
			this->results->addSurrender(p->name, p->equip);
	}

	this->semPlayers->V();
}




/// Set - Cancel Actions.

void Engine::setAction(int equip, char* name, Action* action)
{
	if(this->isExiting || this->isOver())
		return;
	
	this->semActions->P();
	
	struct ActionEngine ae;
	ae.equip = equip;
	strcpy(ae.name, name);
	ae.action = action;
	ae.idObjectCancel = 0;
	this->actions.push_back(ae);
	
	this->semActions->V();
}

void Engine::cancelAction(int equip, char* name, unsigned long long idObject)
{
	if(this->isExiting || this->isOver())
		return;
	
	this->semActions->P();
	
	struct ActionEngine ae;
	ae.equip = equip;
	strcpy(ae.name, name);
	ae.action = NULL;
	ae.idObjectCancel = idObject;
	this->actions.push_back(ae);
	
	this->semActions->V();
}

void Engine::applyActions()
{
	this->semActions->P();
	
	while(this->actions.size() > 0)
	{
		ActionEngine ae = this->actions.front();
		if(ae.action != NULL)
			this->world->setAction(ae.equip, ae.name, ae.action);
		else
			this->world->cancelAction(ae.equip, ae.name, ae.idObjectCancel);
		this->actions.pop_front();
	}
	
	this->semActions->V();
}

bool Engine::isOver()
{
	return stage >= 50;
}

void Engine::step(int ms)
{
	applyActions();
	
	// Update stage.
	if(stage == 0)
	{
		stage = 10;
		tsGameStart = Time::currentMs();
	}
	else if(stage == 10 && ((Time::currentMs()-tsGameStart)/1000) >= CLOSE_ENTER_HUMAN_PLAYERS_SEC )
	{
		stage = 20;
	}
	else if(stage == 20 && ((Time::currentMs()-tsGameStart)/1000) >= CLOSE_ENTER_IA_PLAYERS_SEC )
	{
		stage = 30;
		this->world->removeBasesZones();
	}
	else if(stage == 30 && ((Time::currentMs()-tsGameStart)/1000) >= START_TO_CLOSE_SEC )
	{
		stage = 40;
		this->world->startToCloseArea();
	}
	else if(stage == 40)
	{
		
	}
	else if(stage == 50)
	{
		// Release all views, now they can update and recive that the game ended and receive their points.
		releaseViews();
		return;
	}

	// Update world.
	this->world->update(ms);

	// Check and clear destroyed players.
	this->results->clearDestroyedPlayers();
	
	// The game ended: none or only one team has survived after that players don't enter.
	if(stage >= 30 && this->results->checkEnd())
	{
		stage = 50;
		this->results->checkWinners();
	}

	// Release all views, now they can update.
	releaseViews();

	// Add and remove players.
	addRemovePlayers();
}

void Engine::stop()
{
	// Set to exit.
	this->semViews->P();
	this->semPlayers->P();
	this->isExiting = true;
	this->semViews->V();
	this->semPlayers->V();
	
	applyActions();
	
	// Release all views.
	releaseViews();
	
	// release add and remove players.
	addRemovePlayers();
}



/// Update views.

// Update player view.
// Release when was updating.
// return ok if update ok
bool Engine::updateView(PlayerView* view, float xMin, float xMax, float yMin, float yMax, list<unsigned long long> &idSelected)
{
	if(this->isExiting)
		return false;
	
	// Add view to update.
	this->semViews->P();
	this->views.push_back(view);
	this->semViews->V();

	// Block to wait to update.
	view->block();

	// Get results to end.
	int rank;
	bool surrender;
	int teamWin, winToEnd;
	char name[LEN_NAME+1];
	view->getName(name);
	bool end = this->results->getInfo(name, view->getEquip(), rank, surrender, teamWin, winToEnd);
	
	// Update.
	bool ret = view->update(this->world, xMin, xMax, yMin, yMax, idSelected, end, rank, surrender, teamWin, winToEnd);
	return ret;
}

WorldResult* Engine::getResult()
{
	return this->results;
}

void Engine::releaseViews()
{
	// Release all views, now they can update.
	this->semViews->P();
	
	list<PlayerView*>::iterator it = this->views.begin();
	while(it != this->views.end())
	{
		PlayerView* view = *it;

		view->setMiniMapFormat(USE_PNG_MINIMAP);
		view->release();
		
		it++;
	}
	
	// Wait all views finish and clean them.
	while(this->views.size() > 0)
	{
		this->views.front()->waitFinish();
		this->views.pop_front();
	}
	
	this->semViews->V();
}



void Engine::saveReplay()
{
	/*
	char* buf = new char[MAX_BYTES_SAVED_MAP];
	char path[1000];
	size_t tamBuf;
	char name[LEN_NAME+1];
	int versionWorld;
	
	this->world->getName(name);
	versionWorld = this->world->getVersion();
	
	int pos = 0;
	bool ok = this->world->save(buf, MAX_BYTES_SAVED_MAP, pos);
	
	sprintf(path, "%s/worlds/%s/%d/%s_%d_%llu.map", DATA_DIRECTORY, name, versionWorld, name, versionWorld, Time::currentMs());
	FILE* f = fopen(path, "wb");
	fwrite(buf, pos, 1, f);
	fclose(f);
	
	delete[] buf;
	
	return ok;
	*/
}



