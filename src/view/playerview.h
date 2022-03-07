#ifndef _playerview_h_
#define _playerview_h_

#include "../defs.h"
#include "../config.h"
#include "../compress_image.h"
#include <list>
#include "playerviewupdate.h"
#include "../engine/minimap.h"

#include <unordered_map>

using namespace std;

class PlayerView
{
	private:
		
		char nameCurrent[LEN_NAME];
		int equipCurrent;
		bool bot;
		
		int mineral;
		int oil;
		
		list<char*> playersName;
		list<int> playersEquip;
		
		list<ObjectView*> objects;
		unordered_map<unsigned long long, bool> objectsMineAndAliadeNotifyDestroyed; 
	
		bool enabledBasesZones[EQUIPS][MAX_PLAYERS_BY_EQUIP];
		
		unsigned long long lastTsUpdateArea;
		bool areaIsClosing;

		bool gameEnd;
		
		list<PlayerViewUpdate> updates;
		
		// Minimap.
		int miniMapRes, miniMapZoom, miniMapPosX, miniMapPosY;
		bool settingsChanged;
		unsigned long long timeLastMiniMap;
		unsigned char* miniMap;
		char* compress_minimap;
		int size_minimap;
		bool isPNG;
		bool nextUsePNG;
		
		Semaphore* semWaitUpdate;
		Semaphore* semWaitFinish;
		
	public:
		PlayerView(char* nameCurrent, int equipCurrent, bool bot);
		~PlayerView();
		
		void getName(char* name);
		int getEquip();
		bool isBot();
		
		// Block waiting for update.
		void block();
		
		// Release for update.
		void release();
		
		// Update view, return true if update ok.
		bool update(World* world, float xMin, float xMax, float yMin, float yMax, list<unsigned long long> idsSelected, bool end, int rank, bool surrender, int teamWin, int winToEnd);
		
		// Block until update finish.
		void waitFinish();
		
		
		// Return true if game endend.
		bool isGameEnd();
		
		// Return updates to send.
		// return length or -1 if there isn't updates.
		int toString(char* msg, int max);
		
		
		// Set minimap settings.
		void setMiniMapSetting(int res, int zoom, int posx, int posy);
		void setMiniMapFormat(bool usePNG);
		
		// Return -1 if there isn't or positive integer with the info length.
		// Return if it's png or jpeg otherwise
		int getMiniMap(char* info_minimap, int max, bool &png);
};

#endif
