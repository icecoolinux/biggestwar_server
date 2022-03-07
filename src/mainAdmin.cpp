
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "config.h"
#include "time.h"
#include "bd.h"
#include "semaphore.h"

bool processIsAlive(int pid)
{
	char buf[500];
	sprintf(buf, "/proc/%d/cmdline", pid);
	FILE* f = fopen(buf, "r");
	if(f == NULL)
		return false;
	else
	{
		fclose(f);
		return true;
	}
}

int main(int argc, char** argv)
{
	Semaphore::init();
	
	bool errorBD;
	BD* bd = new BD(errorBD);
	
	if(argc == 1)
	{
		printf("Use: serverAdmin admin|closeall\n");
	}
	
	/// Make a clean: 
	///    1) remove from database process that it's dead
	///    2) remove worlds and players that world is not longer executing (it's dead)
	BD::Process* process;
	int cantProcess;
	if(argc == 2 && (strcmp(argv[1],"admin")==0 || strcmp(argv[1],"closeall")==0))
	{
		printf("Clear bad records...");
		fflush(stdout);
		
		// Clear database from process which it's died.
		cantProcess = bd->getProcess(process);
		if(cantProcess > 0)
		{
			for(int i=0; i<cantProcess; i++)
			{
				if(!processIsAlive(process[i].pid))
					bd->delProcess(process[i].pid);
			}
			delete[] process;
		}
		cantProcess = bd->getProcess(process);
		
		// Get worlds in database.
		BD::WorldInfo* worlds;
		int cantWorlds = bd->getWorldsInfo(worlds);
		
		// Remove worlds and players that serverWorld is not executing.
		for(int i=0; i<cantWorlds; i++)
		{
			bool thereIsProcess = false;
			for(int j=0; j<cantProcess; j++)
			{
				if(!process[j].isLogin && process[j].worldNumber == worlds[i].worldNumber)
				{
					thereIsProcess = true;
					break;
				}
			}
			if(!thereIsProcess)
			{
				// remove world and players from database.
				bd->removeWorldAndPlayers(worlds[i].worldNumber);
			}
		}
		
		printf("OK\n");
	}
	
	/// Handle servers:
	///   1) Lunch a world server if there isnt a world accepting users
	///   2) Lunch server login if it's not up
	///   3) Add agent to worlds that it's not full and it's time to agents enter
	if(argc == 2 && strcmp(argv[1],"admin")==0)
	{
		printf("Admin...");
		fflush(stdout);

		// Get worlds in database.
		BD::WorldInfo* worlds;
		int cantWorlds = bd->getWorldsInfo(worlds);
			
		// Lunch a world server if there isn't one or there are but is not permited to user enter
		bool thereIsWorldOpen = false;
		for(int i=0; i<cantWorlds; i++)
		{
			if(worlds[i].stage >= 10 && worlds[i].stage < 20)
			{
				thereIsWorldOpen = true;
				break;
			}
		}
		if(!thereIsWorldOpen)
		{
			// Lunch world on a available server
			BD::Server *servers;
			int cantServers = bd->getServers(servers);
			char cmd[500];
			for(int port=8000; port<9000; port++)
			{
				if(port == 8888)
					continue;
				bool portIsBusy = false;
				for(int i=0; i<cantWorlds; i++)
				{
					if(strcmp(worlds[i].uri, servers[0].url)==0 && worlds[i].port == port)
					{
						portIsBusy = true;
						break;
					}
				}
				if(!portIsBusy)
				{
					sprintf(cmd, "./serverWorld %s %d &", servers[0].url, port);
					system(cmd);
					break;
				}
			}
		}
		
		
		// Execute serverLogin if isn't.
		bool thereIsLogin = false;
		for(int i=0; i<cantProcess; i++)
		{
			if(process[i].isLogin)
			{
				thereIsLogin = true;
				break;
			}
		}
		if(!thereIsLogin)
		{
			// Lunch login.
			system("./serverMain 8888 &");
		}
		
		
		// Execute neural model for execute RL
		//TODO
		
		// Add RL agents in worlds that is time to add its.
		for(int w=0; w<cantWorlds; w++)
		{
			if(worlds[w].stage >= 20 && worlds[w].stage < 30)
			{
				// Must to add agents to reach AMOUNT_PLAYERS_BY_EQUIPS_AT_LEAST by equip
				if( (AMOUNT_PLAYERS_BY_EQUIPS_AT_LEAST*EQUIPS - worlds[w].amountPlayers) > 0)
				{
					// Get free agents
					char** userAgent;
					char** passAgent;
					int amountAgents = bd->getFreeAgents(AMOUNT_PLAYERS_BY_EQUIPS_AT_LEAST*EQUIPS - worlds[w].amountPlayers, userAgent, passAgent);
					if(amountAgents > 0)
					{
						char cmd[500];
						for(int i=0; i<amountAgents; i++)
						{
							sprintf(cmd, "%s %s %s &", PATH_TO_AGENT, userAgent[i], passAgent[i]);
							system(cmd);
							delete[] userAgent[i];
							delete[] passAgent[i];
						}
						delete[] userAgent;
						delete[] passAgent;
					}
				}
			}
		}
	
		printf("OK\n");
	}
	
	/// Close all
	///   1) Close all proccess
	if(argc == 2 && strcmp(argv[1],"closeall")==0)
	{
		printf("Closing all...");
		fflush(stdout);
		
		BD::Process* process;
		int cantProcess = bd->getProcess(process);
		if(cantProcess > 0)
		{
			for(int i=0; i<cantProcess; i++)
				kill(process[i].pid, SIGQUIT);
			delete[] process;
		}
		
		//
		
		
		printf("OK\n");
	}
	
	delete bd;
}










