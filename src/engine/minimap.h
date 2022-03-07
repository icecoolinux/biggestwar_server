
// It's used for make the minimap.

#ifndef _minimap_h_
#define _minimap_h_

#include "../defs.h"
#include "../config.h"
#include "../time.h"
#include "vec2.h"
#include <list>


using namespace std;

#define RES_MINIMAP_UI_256 256
#define RES_MINIMAP_UI_512 512

#define MS_GENERATE_MINIMAP 5000

#define COLOR_CODE_DARK 0x00

#define COLOR_R_CODE_KNOWN 0x30
#define COLOR_G_CODE_KNOWN 0x30
#define COLOR_B_CODE_KNOWN 0x30

#define COLOR_R_CODE_VISIBLE 0x70
#define COLOR_G_CODE_VISIBLE 0x70
#define COLOR_B_CODE_VISIBLE 0x70

#define COLOR_R_CODE_MAP 0xff
#define COLOR_G_CODE_MAP 0xff
#define COLOR_B_CODE_MAP 0x00

#define COLOR_R_CODE_ALIADE 0x00
#define COLOR_G_CODE_ALIADE 0x00
#define COLOR_B_CODE_ALIADE 0xff

#define COLOR_R_CODE_ENEMY 0xff
#define COLOR_G_CODE_ENEMY 0x00
#define COLOR_B_CODE_ENEMY 0x00

#define COLOR_R_CODE_MINE 0x00
#define COLOR_G_CODE_MINE 0xff
#define COLOR_B_CODE_MINE 0x00

class MiniMap {
	private:
		
		Players* players;
		Map* map;
		
		// For make mini map.
		unsigned long long lastPreMiniMap;
		struct MiniMapVars{
			int res;
			unsigned char* miniMapCommon[EQUIPS];
			bool* knowZones[EQUIPS];
			unsigned char* knowZonesGray[EQUIPS];
			
			bool* knowZonesAllFalse_;
		};
		struct MiniMapVars miniMap256;
		struct MiniMapVars miniMap512;
		struct MiniMapVars miniMap1024;
		struct MiniMapVars miniMap2048;
		
		void createMiniMapVars(struct MiniMapVars* m, int res);
		void destroyMiniMapVars(struct MiniMapVars* m);
		// It's expensive, preMakeMiniMap and doCommonWork do common job to all users.
		void doCommonWork();
		void preMakeMiniMap(struct MiniMapVars* m);
		void putObjectToMap(int j, int i, bool invertY, float radio, unsigned char* m, int res, unsigned char colorR, unsigned char colorG, unsigned char colorB);
		
		// Players info.
		list<vec2> posList[EQUIPS];
		list<float> radioList[EQUIPS];
		list<enum ObjectType> typeList[EQUIPS];
		list<char*> nameList[EQUIPS];
		
		// Map objects info.
		list<vec2> mapPosList;
		list<enum ObjectType> mapTypeList;
		list<bool> mapObjectWasSeenByEquip[EQUIPS];
	
	public:
		
		MiniMap(Players* players, Map* map);
		~MiniMap();
		
		void update(int ms);

		unsigned long long getLastPreMiniMap();
		//void getMiniMap(char* playerName, int equip, unsigned char** map, int res, int posx, int posy, int dimx, int dimy, bool invertY);
		void getMiniMap(char* playerName, int equip, unsigned char* map, int res, int zoom, int posx, int posy, bool invertY);
};

#endif
