#ifndef _BD_h_
#define _BD_h_

#include <stdlib.h>
#include <unordered_map>
#include "config.h"
#include "configPrivate.h"
#include "defs.h"
#include <list>
#include <postgresql/libpq-fe.h>

using namespace std;

class BD
{
	public:
		
		BD(bool &error);
		~BD();
		
		void begin();
		void commit();
		void rollback();
		
		struct WorldInfo {
			int worldNumber;
			int stage;
			int amountPlayers;
			char uri[LEN_URI];
			int port;
		};
		struct UserInfo {
			int wins;
			int lost;
			int points;
			
			bool gamelogged;
			int worldNumber;
			char uri[LEN_URI];
			int port;
			int equip;
			char token[LEN_TOKEN];
			int xView, yView;
		};
		struct Result {
			int world;
			int equip;
			int rank;
			int points;
			bool surrender;
			bool teamWin;
			bool winToEnd;
		};
		
		// Return de world number.
		int newWorld(char* uri, int port, int stage);
		bool removeWorldAndPlayers(int worldNumber);
		bool setStageWorld(int worldNumber, int stage);
		bool setAmountPlayersWorld(int worldNumber, int amount);
		
		int getWorldsInfo(WorldInfo* &worlds);
		WorldInfo* getWorldInfo(int number);
		void getAmountPlayersWorld(int number, int* playersByWorld);
		
		bool login(char* user, char *pass);
		bool loginToken(char* user, char* token);
		
		UserInfo* infoUser(char* user, char *pass);
		UserInfo* infoUserToken(char* user, char* token);

		bool newPlayer(char* user, int worldNumber, int equip, int xView, int yView, char* tokenRet);
		bool removePlayer(char* user, int worldNumber);
		
		bool updateView(char* user, int worldNumber, int xView, int yView);
		bool updatePoints(char* user, bool win, bool lost, int sumPoints);
		
		bool insertResult(char* user, int world, int equip, int rank, int points, bool surrender, bool teamWin, bool winToEnd);
		bool deleteResult(char* user, int world);
		int getResults(char* user, Result* &results);
		
		int getFreeAgents(int amount, char** &user, char** &pass);
		
		
		struct Process{
			int pid;
			int worldNumber;
			bool isLogin;
		};
		
		struct Server{
			char url[LEN_URI];
		};
		
		int getProcess(Process* &process);
		bool addProcess(Process process);
		bool delProcess(int pid);
		
		int getServers(Server* &servers);
		
	private:
		
		PGconn* conexion;
		
		void generateToken(char* token);
		
		
		//Borrar la memoria.
		char*** consulta(char* con, int &filas, int &columnas);
		float* extraerArrayUniFloat(char* datos, int &cant, int &leidos);
		int* extraerArrayUni(char* datos, int &cant, int &leidos);
		int** extraerArrayBi(char* datos, int &cant, int &leidos);
		void borrarMemoria(int filas, int columnas, char*** &resultado);
};

#endif
