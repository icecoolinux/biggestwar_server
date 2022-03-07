
#ifndef _playerviewupdate_h_
#define _playerviewupdate_h_

#include "../defs.h"
#include "../config.h"
#include "../engine/vec2.h"
#include <list>

using namespace std;

class PlayerViewUpdate
{
	private:
		
		
	public:
		
		bool newPlayer;
		bool removedPlayer;
		
		
		char playerName[LEN_NAME];
		int equip;
		unsigned long long id;
		
		// Delete.
		bool wasDeleted;
		bool wasDestroyed;
		
		bool updateResource;
		int mineral;
		int oil;
		
		// Change or new.
		bool wasChanged;
		bool isNew;
		enum ObjectType type;
		
		bool updatePos;
		vec2 pos;
		
		bool updateLife;
		float life;

		bool updateFullLife;
		float fullLife;

		bool updateCreada;
		float creada;

		bool updateConstruccionCreando;
		float construccionCreando;
		float construccionFull;

		bool updateTsLastAttack;
		unsigned long long tsLastAttack;
		
		bool updateCollected;
		float collected;

		bool updateAmount;
		float amount;

		// Action.
		bool actionDeleted;
		bool newAction;
		
		enum ActionType actionType;
		enum ActionSubType actionSubType;
		unsigned long long actionUnitID;
		unsigned long long actionUnit2ID;
		unsigned long long actionBuildID;
		unsigned long long actionBuild2ID;
		unsigned long long actionObjectMapID;
		vec2 actionPos;
		enum ObjectType actionMake;
		
		// Bases zones
		bool basesZones;
		list<bool> isAddBaseZone;
		list<int> equipBaseZone;
		list<vec2> posBaseZone;
		list<int> radioBaseZone;
		
		// Area
		bool updateArea;
		bool areaIsClosing;
		int msToClose;
		vec2 areaFutureCenter;
		int areaCurrentBottom, areaCurrentTop, areaCurrentLeft, areaCurrentRight;
		int areaFutureBottom, areaFutureTop, areaFutureLeft, areaFutureRight;
		float areaSpeedCloseSecBottom, areaSpeedCloseSecTop, areaSpeedCloseSecLeft, areaSpeedCloseSecRight;
		
		// End game.
		bool endGame;
		bool surrenderEndGame;
		bool winEndGame;
		bool loseEndGame;
		bool gameFinishEndGame;
		int rankEquipEndGame;
		
		
		PlayerViewUpdate();
		~PlayerViewUpdate();
		
		void init();
		
		int toString(char* msg, int max);
		// int toCompressBinary(char* msg, int max);
};

#endif
