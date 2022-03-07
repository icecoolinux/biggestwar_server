#include "commandnet.h"
#include "../engine/action.h"

#include <string.h>

void CommandNet::initCommand(CommandNet::Command* c)
{
	c->intent = NULL;
	c->cancelAction = false;
	c->updateArea = false;
	c->select = false;
	c->newPlayer = false;
	c->surrender = false;
	c->play = false;
	c->login = false;
	c->loginGame = false;
	c->miniMap = false;
}

bool CommandNet::getCommands(string& msgS, list<Command*> &commands)
{
	bool ok = true;
	int pos = 0;
	const char* msg = msgS.c_str();

	while(pos < msgS.size() && ok)
	{
		char command[200];
		if(!getString(msg, &command[0], pos))
		{
			ok = false;
			break;
		}
		
		// Update.
		if(strcmp(command, "update")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);
			
			c->updateArea = true;
			
			if( getInt(msg, c->xMinUpdate, pos) && getInt(msg, c->xMaxUpdate, pos) && getInt(msg, c->yMinUpdate, pos) && getInt(msg, c->yMaxUpdate, pos))
			{
				commands.push_back(c);
			}
			else
			{
				delete c;
				ok = false;
				break;
			}
		}
		
		// Select.
		else if(strcmp(command, "selection")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);

			c->select = true;
			
			bool error = false;
			
			int cant;
			if(!getInt(msg, cant, pos))
			{
				error = true;
			}
			else
			{
				for(int i=0; i<cant; i++)
				{
					unsigned long long id;
					if(!getLong(msg, id, pos))
					{
						error = true;
						break;
					}
					
					c->idsSelected.push_back(id);
				}
			}
			
			if(error)
			{
				delete c;
				ok = false;
				break;
			}
			else
				commands.push_back(c);
		}
			
		// Intent.
		else if(strcmp(command, "intent")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);
			
			ParamAction param;
			int type;
			int subType;
			int make;
			
			if( getInt(msg, type, pos) && getInt(msg, subType, pos) && getLong(msg, param.unitID, pos) &&
				getLong(msg, param.unit2ID, pos) && getLong(msg, param.buildID, pos) && getLong(msg, param.build2ID, pos) &&
				getLong(msg, param.objectMapID, pos) && getFloat(msg, param.pos.x, pos) && getFloat(msg, param.pos.y, pos) && getInt(msg, make, pos) )
			{
				param.type = (ActionType)type;
				param.subType = (ActionSubType)subType;
				param.make = (ObjectType)make;

				c->intent = new Action(param);
				commands.push_back(c);
			}
			else
			{
				delete c;
				ok = false;
				break;
			}
		}
		
		// Cancel Action.
		else if(strcmp(command, "cancel_action")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);
			
			c->cancelAction = true;
			
			if( getLong(msg, c->idObjectCancelAction, pos))
			{
				commands.push_back(c);
			}
			else
			{
				delete c;
				ok = false;
				break;
			}
		}
		
		// New player.
		else if(strcmp(command, "newplayer")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);
			
			c->newPlayer = true;
			
			if(getString(msg, &c->user[0], pos) && getString(msg, &c->pass[0], pos))
			{
				commands.push_back(c);
			}
			else
			{
				delete c;
				ok = false;
				break;
			}
		}
		
		// Surrender.
		else if(strcmp(command, "surrender")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);

			c->surrender = true;
			
			commands.push_back(c);
		}
		
		// Play.
		else if(strcmp(command, "play")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);

			c->play = true;
			
			commands.push_back(c);
		}
		
		// Minimap.
		else if(strcmp(command, "minimap")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);
			
			c->miniMap = true;
			
			if(getInt(msg, c->miniMapRes, pos) && getInt(msg, c->miniMapZoom, pos) && getInt(msg, c->miniMapPosX, pos) && getInt(msg, c->miniMapPosY, pos))
			{
				commands.push_back(c);
			}
			else
			{
				delete c;
				ok = false;
				break;
			}
		}
		
		// Login.
		else if(strcmp(command, "login")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);
			
			c->login = true;
			
			if(getString(msg, &c->user[0], pos) && getString(msg, &c->pass[0], pos))
			{
				commands.push_back(c);
			}
			else
			{
				delete c;
				ok = false;
				break;
			}
		}
		
		// Login game.
		else if(strcmp(command, "login_game")==0)
		{
			Command* c = new Command;
			CommandNet::initCommand(c);
			
			c->loginGame = true;
			
			if(getString(msg, &c->user[0], pos) && getString(msg, &c->token[0], pos))
			{
				commands.push_back(c);
			}
			else
			{
				delete c;
				ok = false;
				break;
			}
		}
	}

	// If failure the delete and return error.
	if(!ok)
	{
		while(commands.size() > 0)
		{
			Command* c = commands.front();
			commands.pop_front();
			if(c->intent != NULL)
				delete c->intent;
			delete c;
		}
	}
	
	return ok;
}

// Extract string until , or ;
bool CommandNet::getString(const char* msg, char* str, int &pos)
{
	int pos_ = pos;
	int strPos = 0;
	
	while( msg[pos_] != ',' && msg[pos_] != ';' && msg[pos_] != '\0' )
	{
		str[strPos] = msg[pos_];
		pos_++;
		strPos++;
	}
	str[strPos] = '\0';
	
	if(msg[pos_] == '\0')
		return false;
	
	pos = pos_;
	pos++;
	
	return true;
}

// Extract float until , or ;
bool CommandNet::getFloat(const char* msg, float &f, int &pos)
{
	int pos_ = pos;
	
	char str[100];
	int strPos = 0;
	
	while( msg[pos_] != ',' && msg[pos_] != ';' && msg[pos_] != '\0' )
	{
		str[strPos] = msg[pos_];
		pos_++;
		strPos++;
	}
	str[strPos] = '\0';
	
	if(msg[pos_] == '\0')
		return false;
	
	f = atof(str);
	
	pos = pos_;
	pos++;
	
	return true;
}

// Extract int until , or ;
bool CommandNet::getInt(const char* msg, int &i, int &pos)
{
	int pos_ = pos;
	
	char str[100];
	int strPos = 0;
	
	while( msg[pos_] != ',' && msg[pos_] != ';' && msg[pos_] != '\0' )
	{
		str[strPos] = msg[pos_];
		pos_++;
		strPos++;
	}
	str[strPos] = '\0';
	
	if(msg[pos_] == '\0')
		return false;
	
	i = atoi(str);
	
	pos = pos_;
	pos++;
	
	return true;
}

// Extract long until , or ;
bool CommandNet::getLong(const char* msg, unsigned long long &l, int &pos)
{
	int pos_ = pos;
	
	char str[100];
	int strPos = 0;
	
	while( msg[pos_] != ',' && msg[pos_] != ';' && msg[pos_] != '\0' )
	{
		str[strPos] = msg[pos_];
		pos_++;
		strPos++;
	}
	str[strPos] = '\0';
	
	if(msg[pos_] == '\0')
		return false;
	
	l = strtoull(str, NULL, 10);
	
	pos = pos_;
	pos++;
	
	return true;
}
