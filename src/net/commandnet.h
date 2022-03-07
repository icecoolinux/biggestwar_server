#ifndef _command_h_
#define _command_h_

#include "../defs.h"
#include "../config.h"
#include <string>
#include <list>

using namespace std;

class CommandNet
{
	private:
		
		
	public:
		
		struct Command{
			Action* intent;
			
			bool cancelAction;
			unsigned long long idObjectCancelAction;
			
			bool updateArea;
			int xMinUpdate;
			int xMaxUpdate;
			int yMinUpdate;
			int yMaxUpdate;
			
			bool select;
			list<unsigned long long> idsSelected;
			
			bool newPlayer;
			
			bool play;
			
			bool surrender;
			
			bool login;
			bool loginGame;
			char user[LEN_USER];
			char pass[LEN_PASS];
			char world[50];
			char token[LEN_TOKEN];
			
			bool miniMap;
			int miniMapRes;
			int miniMapZoom;
			int miniMapPosX;
			int miniMapPosY;
		};
		
		static void initCommand(CommandNet::Command* c);
		static bool getCommands(string &msg, list<Command*> &commands);
		
		
		
		
		static bool getString(const char* msg, char* str, int &pos);
		static bool getFloat(const char* msg, float &f, int &pos);
		static bool getInt(const char* msg, int &i, int &pos);
		static bool getLong(const char* msg, unsigned long long &l, int &pos);
};

#endif
