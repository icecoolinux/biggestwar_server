
#include "playerviewupdate.h"
#include <string.h>
#include <stdio.h>

PlayerViewUpdate::PlayerViewUpdate()
{
	init();
}

PlayerViewUpdate::~PlayerViewUpdate()
{
}

void PlayerViewUpdate::init()
{
	this->newPlayer = false;
	this->removedPlayer = false;
	this->wasDeleted = false;
	this->wasDestroyed = false;
	this->updateResource = false;
	this->wasChanged = false;
	this->isNew = false;
	this->updatePos = false;
	this->updateLife = false;
	this->updateFullLife = false;
	this->updateCreada = false;
	this->updateConstruccionCreando = false;
	this->updateTsLastAttack = false;
	this->updateCollected = false;
	this->updateAmount = false;
	this->actionDeleted = false;
	this->newAction = false;
	this->basesZones = false;
	this->updateArea = false;
	this->endGame = false;
}

int PlayerViewUpdate::toString(char* msg, int max)
{
	char buf[10000];
	int ret = -1;
	
	if(this->newPlayer)
	{
		ret = sprintf(buf, "newplayer,%s,%d;", this->playerName, this->equip);
	}
	else if(this->removedPlayer)
	{
		ret = sprintf(buf, "removeplayer,%s;", this->playerName);
	}
	else if(this->wasDeleted)
	{
		if(this->wasDestroyed)
			ret = sprintf(buf, "delete,destroyed,%llu,;", this->id);
		else
			ret = sprintf(buf, "delete,notdestroyed,%llu,;", this->id);
	}
	else if(this->updateResource)
	{
		ret = sprintf(buf, "resources,%llu,%llu;", this->mineral, this->oil);
	}
	else if(this->wasChanged || this->isNew)
	{
		ret = 0;
		
		if(this->isNew)
			ret += sprintf(buf, "new,%llu,player,%s,type,%d,", this->id, this->playerName, this->type);
		else
			ret += sprintf(buf, "change,%llu,", this->id);
		
		if(this->updatePos)
			ret += sprintf(&buf[ret], "pos,%.3f,%.3f,", this->pos.x, this->pos.y);
		if(this->updateLife)
			ret += sprintf(&buf[ret], "life,%d,", (int) ceilf(this->life) );
		if(this->updateFullLife)
			ret += sprintf(&buf[ret], "fulllife,%d,", (int) ceilf(this->fullLife) );
		if(this->updateCreada)
			ret += sprintf(&buf[ret], "creada,%d,", (int) ceilf(this->creada) );
		if(this->updateConstruccionCreando)
			ret += sprintf(&buf[ret], "construccioncreando,%d,%d,", (int) ceilf(this->construccionCreando), (int) ceilf(this->construccionFull) );
		if(this->updateTsLastAttack)
			ret += sprintf(&buf[ret], "tslastattack,%llu,", this->tsLastAttack );
		if(this->updateCollected)
			ret += sprintf(&buf[ret], "collected,%d,", (int) ceilf(this->collected) );
		if(this->updateAmount)
			ret += sprintf(&buf[ret], "amount,%d,", (int) ceilf(this->amount) );
	
		// Action.
		if(this->actionDeleted)
		{
			ret += sprintf(&buf[ret], "deleteaction,");
		}
		else if(this->newAction)
		{
			ret += sprintf(&buf[ret], "newaction,actiontype,%d,actionsubtype,%d,actionunitid,%llu,actionunit2id,%llu,actionbuildid,%llu,actionbuild2id,%llu,"\
									"actionobjectmapid,%llu,actionpos,%f,%f,actionmake,%d,", 
									this->actionType, this->actionSubType, this->actionUnitID, this->actionUnit2ID, this->actionBuildID, this->actionBuild2ID, 
									this->actionObjectMapID, this->actionPos.x, this->actionPos.y, this->actionMake);
		}
		
		strcat(&buf[ret], ";");
		ret += 1;
	}
	// Bases zones
	else if(this->basesZones)
	{
		ret = sprintf(buf, "baseszones,%d", this->isAddBaseZone.size());

		list<bool>::iterator itAdd = this->isAddBaseZone.begin();
		list<int>::iterator itEquip = this->equipBaseZone.begin();
		list<vec2>::iterator itPos = this->posBaseZone.begin();
		list<int>::iterator itRadio = this->radioBaseZone.begin();
		while(itAdd != this->isAddBaseZone.end())
		{
			if(*itAdd)
				ret += sprintf(&(buf[ret]), ",add,%d,%d,%d,%d", *itEquip, (int)(*itPos).x, (int)(*itPos).y, *itRadio);
			else
				ret += sprintf(&(buf[ret]), ",del,%d,%d", (int)itPos->x, (int)itPos->y);

			itAdd++;
			itEquip++;
			itPos++;
			itRadio++;
		}

		strcat(&buf[ret], ";");
		ret += 1;
	}
	else if(this->updateArea)
	{
		ret = sprintf(buf, "area,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f;", this->areaIsClosing, this->msToClose, (int)this->areaFutureCenter.x, (int)this->areaFutureCenter.y, 
											areaCurrentBottom, areaCurrentTop, areaCurrentLeft, areaCurrentRight,
											areaFutureBottom, areaFutureTop, areaFutureLeft, areaFutureRight,
											areaSpeedCloseSecBottom, areaSpeedCloseSecTop, areaSpeedCloseSecLeft, areaSpeedCloseSecRight);
	}
	else if(this->endGame)
	{
		char surrender_String[10];
		char win_String[10];
		char lose_String[10];
		char gameFinish_String[10];
		strcpy(surrender_String, "false");
		strcpy(win_String, "false");
		strcpy(lose_String, "false");
		strcpy(gameFinish_String, "false");
		if(this->surrenderEndGame)
			strcpy(surrender_String, "true");
		if(this->winEndGame)
			strcpy(win_String, "true");
		if(this->loseEndGame)
			strcpy(lose_String, "true");
		if(this->gameFinishEndGame)
			strcpy(gameFinish_String, "true");
		
		ret = sprintf(buf, "endgame,%s,%s,%s,%s,%d;", surrender_String, win_String, lose_String, gameFinish_String, this->rankEquipEndGame);
	}

	if( ret >= max)
		ret = -1;
	else
		strcpy(msg, buf);
	
	return ret;
}











/*
#define BYTES_ID_ULL_NET 3
#define BYTES_POS_FLOAT_NET 4
#define BYTES_RESOURCES_ULL_NET 4

int PlayerViewUpdate::toCompressBinary(char* msg, int max)
{
	char buf[10000];
	int pos = 0;
	
	if(this->newPlayer)
	{
		// sprintf(buf, "newplayer,%s,%d;", this->playerName, this->equip);
		buf[pos] = 'a';
		buf[pos+1] = 'p';
		pos+=2;
		addString(buf, this->playerName, pos);
		addEquip(buf, this->equip, pos);
	}
	else if(this->removedPlayer)
	{
		// sprintf(buf, "removeplayer,%s;", this->playerName);
		buf[pos] = 'a';
		buf[pos+1] = 'r';
		pos+=2;
		addString(buf, this->playerName, pos);
	}
	else if(this->wasDeleted)
	{
		buf[pos] = 'a';
		buf[pos+1] = 'd';
		pos+=2;
		if(this->wasDestroyed)
		{
			// sprintf(buf, "delete,destroyed,%llu,;", this->id);
			buf[pos] = 'y';
			pos++;
			addId(buf, this->id, pos);
		}
		else
		{
			// sprintf(buf, "delete,notdestroyed,%llu,;", this->id);
			buf[pos] = 'n';
			pos++;
			addId(buf, this->id, pos);
		}
	}
	else if(this->updateResource)
	{
		// sprintf(buf, "resources,%llu,%llu;", this->mineral, this->oil);
		buf[pos] = 'a';
		buf[pos+1] = 'u';
		pos+=2;
		addResource(buf, this->mineral, pos);
		addResource(buf, this->oil, pos);
	}
	else if(this->wasChanged || this->isNew)
	{
		if(this->isNew)
		{
			// sprintf(buf, "new,%llu,player,%s,type,%d,", this->id, this->playerName, this->type);
			buf[pos] = 'a';
			buf[pos+1] = 'n';
			pos+=2;
			addId(buf, this->id, pos);
			addString(buf, this->playerName, pos);
			addType(buf, this->type, pos);
		}
		else
		{
			// sprintf(buf, "change,%llu,", this->id);
			buf[pos] = 'a';
			buf[pos+1] = 'c';
			pos+=2;
			addId(buf, this->id, pos);
		}
		
		
		
		
		
		
		
		
		Falta de a partir de aca porque la cantidad de parametros es dinamico en change
		if(this->updatePos)
		{
			// sprintf(&buf[ret], "pos,%.3f,%.3f,", this->pos.x, this->pos.y);
			buf[pos] = 'p';
			pos++;
		}
		if(this->updateLife)
			ret += sprintf(&buf[ret], "life,%d,", (int) ceilf(this->life) );
		if(this->updateFullLife)
			ret += sprintf(&buf[ret], "fulllife,%d,", (int) ceilf(this->fullLife) );
		if(this->updateCreada)
			ret += sprintf(&buf[ret], "creada,%d,", (int) ceilf(this->creada) );
		if(this->updateConstruccionCreando)
			ret += sprintf(&buf[ret], "construccioncreando,%d,%d,", (int) ceilf(this->construccionCreando), (int) ceilf(this->construccionFull) );
		if(this->updateTsLastAttack)
			ret += sprintf(&buf[ret], "tslastattack,%llu,", this->tsLastAttack );
		if(this->updateCollected)
			ret += sprintf(&buf[ret], "collected,%d,", (int) ceilf(this->collected) );
		if(this->updateAmount)
			ret += sprintf(&buf[ret], "amount,%d,", (int) ceilf(this->amount) );
	
		// Action.
		if(this->actionDeleted)
		{
			ret += sprintf(&buf[ret], "deleteaction,");
		}
		else if(this->newAction)
		{
			ret += sprintf(&buf[ret], "newaction,actiontype,%d,actionsubtype,%d,actionunitid,%llu,actionunit2id,%llu,actionbuildid,%llu,actionbuild2id,%llu,"\
									"actionobjectmapid,%llu,actionpos,%f,%f,actionmake,%d,", 
									this->actionType, this->actionSubType, this->actionUnitID, this->actionUnit2ID, this->actionBuildID, this->actionBuild2ID, 
									this->actionObjectMapID, this->actionPos.x, this->actionPos.y, this->actionMake);
		}
		
		strcat(&buf[ret], ";");
		ret += 1;
	}
	// Bases zones
	else if(this->basesZones)
	{
		ret = sprintf(buf, "baseszones,%d", this->isAddBaseZone.size());

		list<bool>::iterator itAdd = this->isAddBaseZone.begin();
		list<int>::iterator itEquip = this->equipBaseZone.begin();
		list<vec2>::iterator itPos = this->posBaseZone.begin();
		list<int>::iterator itRadio = this->radioBaseZone.begin();
		while(itAdd != this->isAddBaseZone.end())
		{
			if(*itAdd)
				ret += sprintf(&(buf[ret]), ",add,%d,%d,%d,%d", *itEquip, (int)(*itPos).x, (int)(*itPos).y, *itRadio);
			else
				ret += sprintf(&(buf[ret]), ",del,%d,%d", (int)itPos->x, (int)itPos->y);

			itAdd++;
			itEquip++;
			itPos++;
			itRadio++;
		}

		strcat(&buf[ret], ";");
		ret += 1;
	}
	else if(this->updateArea)
	{
		ret = sprintf(buf, "area,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f;", this->areaIsClosing, this->msToClose, (int)this->areaFutureCenter.x, (int)this->areaFutureCenter.y, 
											areaCurrentBottom, areaCurrentTop, areaCurrentLeft, areaCurrentRight,
											areaFutureBottom, areaFutureTop, areaFutureLeft, areaFutureRight,
											areaSpeedCloseSecBottom, areaSpeedCloseSecTop, areaSpeedCloseSecLeft, areaSpeedCloseSecRight);
	}
	else if(this->endGame)
	{
		char surrender_String[10];
		char win_String[10];
		char lose_String[10];
		char gameFinish_String[10];
		strcpy(surrender_String, "false");
		strcpy(win_String, "false");
		strcpy(lose_String, "false");
		strcpy(gameFinish_String, "false");
		if(this->surrenderEndGame)
			strcpy(surrender_String, "true");
		if(this->winEndGame)
			strcpy(win_String, "true");
		if(this->loseEndGame)
			strcpy(lose_String, "true");
		if(this->gameFinishEndGame)
			strcpy(gameFinish_String, "true");
		
		ret = sprintf(buf, "endgame,%s,%s,%s,%s,%d;", surrender_String, win_String, lose_String, gameFinish_String, this->rankEquipEndGame);
	}

	if( ret >= max)
		ret = -1;
	else
		strcpy(msg, buf);
	
	return ret;
}


void PlayerViewUpdate::addId(char* buf, unsigned long long v, int &pos)
{
	for(int i=BYTES_ID_ULL_NET-1; i>=0; i--)
	{
		buf[pos] = v%256;
		v /= 256;
		pos++;
	}
}

void PlayerViewUpdate::addString(char* buf, char* s, int &pos)
{
	strcpy(&buf[pos], s);
	pos += strlen(s) +1;
}

void PlayerViewUpdate::addEquip(char* buf, int e, int &pos)
{
	buf[pos] = e;
	pos++;
}

void PlayerViewUpdate::addType(char* buf, int t, int &pos)
{
	buf[pos] = t;
	pos++;
}

void PlayerViewUpdate::addPos(char* buf, float p, int &pos)
{
	buf[pos] = 'p';
	if(p < 0)
	{
		buf[pos] = 'n';
		p *= -1;
	}
	pos++;
	
	if(p > 16000.0f)
		p = 16000.0f;
	
	int v = p * 1000.0f;
	for(int i=(BYTES_POS_FLOAT_NET-1)-1; i>=0; i--)
	{
		buf[pos] = v%256;
		v /= 256;
		pos++;
	}
}



*/
