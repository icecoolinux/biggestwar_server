#ifndef _worldresult_h_
#define _worldresult_h_

#include "vec2.h"
#include "../time.h"
#include "../defs.h"
#include "../config.h"
#include "../bd.h"
#include <stdio.h>
#include <list>
#include <string.h>

using namespace std;

class WorldResult
{
	private:
		
		World* world;
		
		struct PlayerInfoResult{
			char name[LEN_NAME];
			int equip;
			int rank;
			bool surrender;
			int teamWin; // -1 don't know, 0 not, 1 yes
			int winToEnd; // -1 don't know, 0 not, 1 yes
		};
		list<PlayerInfoResult> playerInfo[EQUIPS];
		
		Semaphore* sem;
		
	public:
		WorldResult(World* world);
		~WorldResult();
		
		bool checkEnd();
		void addSurrender(char* name, int equip);
		void clearDestroyedPlayers();
		void checkWinners();
		
		// teamWin and winToEnd: -1 don't know, 0 not, 1 yes
		bool getInfo(char* player, int equip, int &rank, bool &surrender, int &teamWin, int &winToEnd);
		
		// Save info to database.
		bool saveToDB(BD* bd);
};

#endif
