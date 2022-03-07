#ifndef _engine_h_
#define _engine_h_

#include "../defs.h"
#include <list>
#include "../bd.h"
#include <string>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

class Engine {
	private:
		
		// 0: Init server.
		// 10: Start game, enter only human players.
		// 20: Close to new humans players and open to enter IA players.
		// 30: Close to enter IA players.
		// 40: Start to close area.
		// 50: End game.
		int stage;
		unsigned long long tsGameStart;

		World* world;
		bool isExiting;
		
		// New-Remove players mutua exclusion
		struct PlayerEngine{
			char* name;
			int xInit, yInit;
			int equip;
			bool ok;
			Semaphore* block;
		};
		list<PlayerEngine*> newPlayers;
		list<PlayerEngine*> removePlayers;
		Semaphore* semPlayers;
		void addRemovePlayers();
		
		// Update views mutua exclusion
		list<PlayerView*> views;
		Semaphore* semViews;
		void releaseViews();
		
		// Set and cancel actions mutua exclusion
		Semaphore* semActions;
		struct ActionEngine{
			int equip;
			char name[LEN_USER];
			Action* action;
			unsigned long long idObjectCancel;
		};
		list<ActionEngine> actions;
		void applyActions();
		
		WorldResult* results;
		
		void saveReplay();
		
	public:
	
		Engine(int number);
		~Engine();

		int getWorldNumber();
		int getStage();
		int getAmountPlayers();
		
		bool newPlayer(char* name, int &xInit, int &yInit, int &equip);
		bool removePlayer(int equip, char* name);
		
		void setAction(int equip, char* name, Action* action);
		void cancelAction(int equip, char* name, unsigned long long idObject);
		
		bool isOver();
		void step(int ms);
		void stop();
		
		// Update player view.
		// Release when was updating.
		// return ok if update ok
		bool updateView(PlayerView* view, float xMin, float xMax, float yMin, float yMax, list<unsigned long long> &idSelected);
		
		WorldResult* getResult();
};

#endif
