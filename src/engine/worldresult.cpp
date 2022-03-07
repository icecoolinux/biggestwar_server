
#include "worldresult.h"
#include "world.h"
#include "../semaphore.h"

WorldResult::WorldResult(World* world)
{
	this->world = world;
	this->sem = new Semaphore(1);
}

WorldResult::~WorldResult()
{
	delete this->sem;
}
		
bool WorldResult::checkEnd()
{
	int survivedEquips = 0;
	
	for(int i=0; i<EQUIPS; i++)
		if(this->world->getAmountPlayersByEquip(i) > 0)
			survivedEquips += 1;
		
	if(survivedEquips <= 1)
		return true;
	else
		return false;
}

void WorldResult::addSurrender(char* name, int equip)
{
	this->sem->P();
	
	PlayerInfoResult info;
	strcpy(info.name, name);
	info.equip = equip;
	info.rank = this->world->getAmountPlayersByEquip(equip)+1;
	info.surrender = true;
	info.teamWin = -1;
	info.winToEnd = -1;
	this->playerInfo[equip].push_back(info);
	
	this->sem->V();
}

void WorldResult::clearDestroyedPlayers()
{
	list<char*> names;
	list<int> equips;
	if(this->world->clearDestroyedPlayers(names, equips) > 0)
	{
		this->sem->P();
		
		list<char*>::iterator itName = names.begin();
		list<int>::iterator itEquip = equips.begin();
		while(itName != names.end())
		{
			PlayerInfoResult info;
			strcpy(info.name, *itName);
			info.equip = *itEquip;
			info.rank = this->world->getAmountPlayersByEquip(info.equip)+1;
			info.surrender = false;
			info.teamWin = -1;
			info.winToEnd = -1;
			this->playerInfo[info.equip].push_back(info);
			itName++;
			itEquip++;
		}
		
		this->sem->V();
	}
}

void WorldResult::checkWinners()
{
	int maxPlayer = 0;
	bool isWinner[EQUIPS];
	for(int i=0; i<EQUIPS; i++)
		isWinner[i] = false;
	
	for(int e=0; e<EQUIPS; e++)
	{
		if(this->world->getAmountPlayersByEquip(e) > maxPlayer)
		{
			for(int j=0; j<EQUIPS; j++)
				isWinner[j] = false;
			maxPlayer = this->world->getAmountPlayersByEquip(e);
			isWinner[e] = true;
		}
		else if(this->world->getAmountPlayersByEquip(e) == maxPlayer)
			isWinner[e] = true;
	}
	
	this->sem->P();
	
	// Set winners and losers.
	for(int e=0; e<EQUIPS; e++)
	{
		list<PlayerInfoResult>::iterator it = this->playerInfo[e].begin();
		while(it != this->playerInfo[e].end())
		{
			if(isWinner[e])
				(*it).teamWin = 1;
			else
				(*it).teamWin = 0;
			(*it).winToEnd = 0;
			it++;
		}
	}

	// Get alive players.
	list<char*> old_;
	list<char*> alivePlayers;
	list<int> aliveEquip;
	list<char*> remove_;
	this->world->getPlayers(old_, alivePlayers, aliveEquip, remove_);
	
	// Set live players (winners).
	list<char*>::iterator it2 = alivePlayers.begin();
	list<int>::iterator it3 = aliveEquip.begin();
	while(it2 != alivePlayers.end())
	{
		PlayerInfoResult info;
		strcpy(info.name, *it2);
		info.equip = *it3;
		info.rank = 1;
		info.surrender = false;
		if(isWinner[info.equip])
		{
			info.teamWin = 1;
			info.winToEnd = 1;
		}
		else
		{
			info.teamWin = 0;
			info.winToEnd = 0;
		}
		this->playerInfo[info.equip].push_back(info);
		it2++;
		it3++;
	}
	
	this->sem->V();
}

bool WorldResult::getInfo(char* player, int equip, int &rank, bool &surrender, int &teamWin, int &winToEnd)
{
	rank = -1;
	surrender = false;
	teamWin = -1;
	winToEnd = -1;
	
	this->sem->P();
	
	list<PlayerInfoResult>::iterator it = this->playerInfo[equip].begin();
	while(it != this->playerInfo[equip].end())
	{
		if(strcmp(player, (*it).name)==0)
		{
			rank = (*it).rank;
			surrender = (*it).surrender;
			teamWin = (*it).teamWin;
			winToEnd = (*it).winToEnd;
			
			this->sem->V();
			return true;
		}
		it++;
	}
	
	this->sem->V();
	return false;
}


// Save info to database.
bool WorldResult::saveToDB(BD* bd)
{
	bool ret = true;
	
	this->sem->P();
	
	for(int e=0; e<EQUIPS; e++)
	{
		list<PlayerInfoResult>::iterator it = this->playerInfo[e].begin();
		while(it != this->playerInfo[e].end())
		{
			PlayerInfoResult info = *it;
			
			bool teamWin = false;
			if(info.teamWin == 1)
				teamWin = true;
			bool winToEnd = false;
			if(info.winToEnd == 1)
				winToEnd = true;
			int points = 0;
			if(winToEnd)
				points = 200;
			else if(!info.surrender && teamWin)
				points = 100 - info.rank +1;
			
			ret = bd->updatePoints(info.name, teamWin && !info.surrender, info.surrender || !teamWin, points);
			if(ret)
				ret = bd->insertResult(info.name, this->world->getNumber(), info.equip, info.rank, points, info.surrender, teamWin, winToEnd);
			
			it++;
		}
	}
	
	this->sem->V();
	
	return ret;
}









