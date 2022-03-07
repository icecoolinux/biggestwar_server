
#include "bd.h"
#include "semaphore.h"

#include <string.h>


BD::BD(bool &error)
{
	char config[500];
	sprintf(config, "user=%s password=%s dbname=%s hostaddr=%s port=%d", DATABASE_USER, DATABASE_PASSWORD, DATABASE_NAME, DATABASE_HOST, DATABASE_PORT);
	conexion = PQconnectdb(config);
	error = PQstatus(conexion) == CONNECTION_BAD;
}

BD::~BD()
{
	PQfinish(conexion);
}

void BD::begin()
{
	PGresult* res = PQexec(conexion, "BEGIN;");
	PQclear(res);
}

void BD::commit()
{
	PGresult* res = PQexec(conexion, "COMMIT;");
	PQclear(res);
}

void BD::rollback()
{
	PGresult* res = PQexec(conexion, "ROLLBACK;");
	PQclear(res);
}

int BD::newWorld(char* uri, int port, int stage)
{
	char q[1000];
	sprintf(q, "INSERT INTO worlds (worldNumber, stage, amountPlayers, uri, port) VALUES (DEFAULT, %d, 0, '%s', %d) RETURNING worldNumber;", stage, uri, port);
printf("%s\n", q);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return -1;
	}
	else
	{
		int numWorld = atoi(PQgetvalue(res, 0, 0));
		PQclear(res);
		return numWorld;
	}
}

bool BD::removeWorldAndPlayers(int worldNumber)
{
	char q[1000];
	sprintf(q, "BEGIN; "\
				"DELETE FROM players WHERE worldNumber = %d; "\
				"DELETE FROM worlds WHERE worldNumber = %d; "\
				"COMMIT;", worldNumber, worldNumber);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

bool BD::setStageWorld(int worldNumber, int stage)
{
	char q[1000];
	sprintf(q, "UPDATE worlds SET stage=%d WHERE worldNumber = %d ;", stage, worldNumber);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

bool BD::setAmountPlayersWorld(int worldNumber, int amount)
{
	char q[1000];
	sprintf(q, "UPDATE worlds SET amountPlayers=%d WHERE worldNumber = %d ;", amount, worldNumber);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

int BD::getWorldsInfo(WorldInfo* &worlds)
{
	char q[1000];
	sprintf(q, "SELECT worldNumber,stage,amountplayers,uri,port FROM worlds;");
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0 )
	{
		PQclear(res);
		return 0;
	}

	int qWorlds = PQntuples(res);
	worlds = new WorldInfo[qWorlds];
	for(int i=0; i<qWorlds; i++)
	{
		worlds[i].worldNumber = atoi(PQgetvalue(res, i, 0));
		worlds[i].stage = atoi(PQgetvalue(res, i, 1));
		worlds[i].amountPlayers = atoi(PQgetvalue(res, i, 2));
		strcpy(worlds[i].uri, PQgetvalue(res, i, 3));
		worlds[i].port = atoi(PQgetvalue(res, i, 4));
	}
	
	PQclear(res);
	 
	return qWorlds;
}

void BD::getAmountPlayersWorld(int number, int* playersByWorld)
{
	for(int i=0; i<EQUIPS; i++)
		playersByWorld[i] = 0;
	
	char q[1000];
	sprintf(q, "SELECT equip, count(username) as amount FROM players WHERE worldNumber = %d GROUP BY equip;", number);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0 )
	{
		PQclear(res);
		return;
	}

	int qEquips = PQntuples(res);
	for(int i=0; i<qEquips; i++)
	{
		int equip = atoi(PQgetvalue(res, i, 0));
		int amount = atoi(PQgetvalue(res, i, 1));
		playersByWorld[equip] = amount;
	}
	
	PQclear(res);
	 
	return;
}

BD::WorldInfo* BD::getWorldInfo(int number)
{
	char q[1000];
	sprintf(q, "SELECT stage,amountplayers,uri,port FROM worlds WHERE worldNumber = %d;", number);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0 )
	{
		PQclear(res);
		return NULL;
	}

	WorldInfo* info = new WorldInfo;
	info->worldNumber = number;
	info->stage = atoi(PQgetvalue(res, 0, 0));
	info->amountPlayers = atoi(PQgetvalue(res, 0, 1));
	strcpy(info->uri, PQgetvalue(res, 0, 2));
	info->port = atoi(PQgetvalue(res, 0, 3));
	
	PQclear(res);
	 
	return info;
}

bool BD::login(char* user, char *pass)
{
	char q[1000];
	sprintf(q, "SELECT username FROM users WHERE username = '%s' AND pass = '%s' ;", user, pass);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0 )
	{
		PQclear(res);
		return false;
	}

	PQclear(res);
	return true;
}

bool BD::loginToken(char* user, char* token)
{
	char q[1000];
	sprintf(q, "SELECT username FROM players WHERE username = '%s' AND token = '%s' ;", user, token);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0)
	{
		PQclear(res);
		return false;
	}

	PQclear(res);
	return true;
}


BD::UserInfo* BD::infoUser(char* user, char *pass)
{
	char q[1000];
	sprintf(q, "SELECT wins,lost,points FROM users WHERE username = '%s' AND pass = '%s';", user, pass);
printf("%s\n", q);
	PGresult* res = PQexec(conexion, q);

	if ( !res || ( (PGRES_TUPLES_OK != PQresultStatus(res)) && (PGRES_COMMAND_OK != PQresultStatus(res)) ) || PQntuples(res) == 0  )
	{
		PQclear(res);
		return NULL;
	}

	UserInfo* info = new UserInfo;
	info->wins = atoi(PQgetvalue(res, 0, 0));
	info->lost = atoi(PQgetvalue(res, 0, 1));
	info->points = atoi(PQgetvalue(res, 0, 2));
	PQclear(res);
	
	sprintf(q, "SELECT players.worldNumber, uri, port, equip, token, xView, yView FROM players, worlds " \
				" WHERE players.worldNumber = worlds.worldNumber AND players.username = '%s' ;", user);
	res = PQexec(conexion, q);

	if ( !res || ( (PGRES_TUPLES_OK != PQresultStatus(res)) && (PGRES_COMMAND_OK != PQresultStatus(res)) ) || PQntuples(res) == 0 )
	{
		info->gamelogged = false;
		PQclear(res);
		return info;
	}

	info->gamelogged = true;
	info->worldNumber = atoi(PQgetvalue(res, 0, 0));
	strcpy(info->uri, PQgetvalue(res, 0, 1));
	info->port = atoi(PQgetvalue(res, 0, 2));
	info->equip = atoi(PQgetvalue(res, 0, 3));
	strcpy(info->token, PQgetvalue(res, 0, 4));
	info->xView = atoi(PQgetvalue(res, 0, 5));
	info->yView = atoi(PQgetvalue(res, 0, 6));
	
	PQclear(res);
	
	return info;
}

BD::UserInfo* BD::infoUserToken(char* user, char* token)
{
	char q[1000];
	sprintf(q, "SELECT wins,lost,points FROM players, users WHERE players.username = users.username AND players.username = '%s' AND token = '%s';", user, token);
	PGresult* res = PQexec(conexion, q);

	if ( !res || ( (PGRES_TUPLES_OK != PQresultStatus(res)) && (PGRES_COMMAND_OK != PQresultStatus(res)) ) || PQntuples(res) == 0  )
	{
		PQclear(res);
		return NULL;
	}

	UserInfo* info = new UserInfo;
	info->wins = atoi(PQgetvalue(res, 0, 0));
	info->lost = atoi(PQgetvalue(res, 0, 1));
	info->points = atoi(PQgetvalue(res, 0, 2));
	PQclear(res);
	
	sprintf(q, "SELECT players.worldNumber, uri, port, equip, token, xView, yView FROM players, worlds " \
				" WHERE players.worldNumber = worlds.worldNumber AND players.username = '%s' ;", user);
	res = PQexec(conexion, q);

	if ( !res || ( (PGRES_TUPLES_OK != PQresultStatus(res)) && (PGRES_COMMAND_OK != PQresultStatus(res)) ) || PQntuples(res) == 0 )
	{
		info->gamelogged = false;
		PQclear(res);
		return info;
	}

	info->gamelogged = true;
	info->worldNumber = atoi(PQgetvalue(res, 0, 0));
	strcpy(info->uri, PQgetvalue(res, 0, 1));
	info->port = atoi(PQgetvalue(res, 0, 2));
	info->equip = atoi(PQgetvalue(res, 0, 3));
	strcpy(info->token, PQgetvalue(res, 0, 4));
	info->xView = atoi(PQgetvalue(res, 0, 5));
	info->yView = atoi(PQgetvalue(res, 0, 6));
	
	PQclear(res);
	
	return info;
}

bool BD::newPlayer(char* user, int worldNumber, int equip, int xView, int yView, char* tokenRet)
{
	generateToken(tokenRet);

	char q[1000];
	sprintf(q, "INSERT INTO players (username, worldNumber, equip, token, xview, yview) VALUES ('%s', '%d', %d, '%s', %d, %d);", user, worldNumber, equip, tokenRet, xView, yView);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

bool BD::removePlayer(char* user, int worldNumber)
{
	char q[1000];
	sprintf(q, "DELETE FROM players WHERE username = '%s' AND worldNumber = %d;", user, worldNumber);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

bool BD::updateView(char* user, int worldNumber, int xView, int yView)
{
	char q[1000];
	sprintf(q, "UPDATE players SET xview=%d, yview=%d WHERE username = '%s' AND worldNumber = %d;", xView, yView, user, worldNumber);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

bool BD::updatePoints(char* user, bool win, bool lost, int sumPoints)
{
	this->begin();
	
	char q[1000];
	sprintf(q, "SELECT wins,lost,points FROM users WHERE username = '%s';", user);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0 )
	{
		PQclear(res);
		return 0;
	}
	
	int wins = atoi(PQgetvalue(res, 0, 0));
	int losts = atoi(PQgetvalue(res, 0, 1));
	int points = atoi(PQgetvalue(res, 0, 2));
	PQclear(res);
	
	
	if(win)
		wins += 1;
	if(lost)
		losts += 1;
	points += sumPoints;
	
	
	sprintf(q, "UPDATE users SET wins=%d, lost=%d, points=%d WHERE username = '%s';", wins, losts, points, user);
	res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		this->rollback();
		PQclear(res);
		return false;
	}
	else
	{
		this->commit();
		PQclear(res);
		return true;
	}
}

bool BD::insertResult(char* user, int world, int equip, int rank, int points, bool surrender, bool teamWin, bool winToEnd)
{
	int surrender_Int = 0;
	if(surrender)
		surrender_Int = 1;
	int teamWin_Int = 0;
	if(teamWin)
		teamWin_Int = 1;
	int winToEnd_Int = 0;
	if(winToEnd)
		winToEnd_Int = 1;
	
	char q[1000];
	sprintf(q, "INSERT INTO results (username, worldNumber, equip, rank, points, surrender, teamWin, winToEnd) VALUES "\
				"('%s', %d, %d, %d, %d, %d, %d, %d);", user, world, equip, rank, points, surrender_Int, teamWin_Int, winToEnd_Int);
printf("%s\n",q);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

bool BD::deleteResult(char* user, int world)
{
	char q[1000];
	sprintf(q, "DELETE FROM results WHERE username = '%s' AND worldNumber = %d;", user, world);
printf("%s\n",q);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

int BD::getResults(char* user, Result* &results)
{
	char q[1000];
	sprintf(q, "SELECT worldNumber,equip,rank,points,surrender,teamWin,winToEnd FROM results WHERE username = '%s';", user);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0 )
	{
		PQclear(res);
		return 0;
	}
	
	int cant = PQntuples(res);

	results = new Result[cant];
	for(int i=0; i<cant; i++)
	{
		results[i].world = atoi(PQgetvalue(res, i, 0));
		results[i].equip = atoi(PQgetvalue(res, i, 1));
		results[i].rank = atoi(PQgetvalue(res, i, 2));
		results[i].points = atoi(PQgetvalue(res, i, 3));
		results[i].surrender = atoi(PQgetvalue(res, i, 4)) == 1;
		results[i].teamWin = atoi(PQgetvalue(res, i, 5)) == 1;
		results[i].winToEnd = atoi(PQgetvalue(res, i, 6)) == 1;
	}
	
	PQclear(res);
	return cant;
}

int BD::getFreeAgents(int amount, char** &user, char** &pass)
{
	char q[1000];
	sprintf(q, "SELECT username,pass FROM users WHERE username ilike 'AgentIA%%' AND username not in (SELECT username FROM players) LIMIT %d;", amount);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0 )
	{
		PQclear(res);
		return 0;
	}
	
	int cant = PQntuples(res);

	user = new char*[cant];
	pass = new char*[cant];
	for(int i=0; i<cant; i++)
	{
		user[i] = new char[strlen(PQgetvalue(res, i, 0))+1];
		strcpy(user[i], PQgetvalue(res, i, 0));
		pass[i] = new char[strlen(PQgetvalue(res, i, 1))+1];
		strcpy(pass[i], PQgetvalue(res, i, 1));
	}
	
	PQclear(res);
	return cant;
}




int BD::getProcess(Process* &process)
{
	char q[1000];
	sprintf(q, "SELECT pid, worldNumber, eslogin FROM Process;");
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0)
	{
		PQclear(res);
		return 0;
	}

	int cant = PQntuples(res);
	process = new Process[cant];
	for(int i=0; i<cant; i++)
	{
		process[i].pid = atoi(PQgetvalue(res, i, 0));
		process[i].worldNumber = atoi(PQgetvalue(res, i, 1));
		process[i].isLogin = atoi(PQgetvalue(res, i, 2)) == 1;
	}
	
	PQclear(res);
	return cant;
}

bool BD::addProcess(Process process)
{
	int isLogin = 0;
	if(process.isLogin)
		isLogin = 1;
	
	char q[1000];
	sprintf(q, "INSERT INTO Process (pid, worldNumber, eslogin) VALUES (%d, %d, %d) ;", process.pid, process.worldNumber, isLogin);
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

bool BD::delProcess(int pid)
{
	char q[1000];
	sprintf(q, "DELETE FROM Process WHERE pid = %d;", pid);
	PGresult* res = PQexec(conexion, q);
	
	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) )
	{
		PQclear(res);
		return false;
	}
	else
	{
		PQclear(res);
		return true;
	}
}

int BD::getServers(Server* &servers)
{
	char q[1000];
	sprintf(q, "SELECT url FROM Servers;");
	PGresult* res = PQexec(conexion, q);

	if ( !res || (PGRES_TUPLES_OK != PQresultStatus(res) && PGRES_COMMAND_OK != PQresultStatus(res)) || PQntuples(res) == 0)
	{
		PQclear(res);
		return 0;
	}

	int cant = PQntuples(res);
	servers = new Server[cant];
	for(int i=0; i<cant; i++)
	{
		strcpy(servers[i].url, PQgetvalue(res, i, 0));
	}
	
	PQclear(res);
	return cant;
}


void BD::generateToken(char* token)
{
	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	const size_t max_index = (sizeof(charset) - 1);
	
	for(int i=0; i<LEN_TOKEN-1; i++)
		token[i] = charset[ rand() % max_index ];
	token[LEN_TOKEN-1] = '\0';
}
