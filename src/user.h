#ifndef _user_h_
#define _user_h_

#include "defs.h"
#include "config.h"
#include <pthread.h>
#include <string>
#include <list>
#include "bd.h"

using namespace std;

class User
{
	private:
		Semaphore* sem;
		
		bool mustExit;
		bool exited;

		Sock* sock;
		char user[LEN_USER];
		int equip;
		bool loggedForGame;
		bool isBot; // If the username starts with 'Agent' then is bot.
		
		PlayerView* view;
		Engine* engine;
		
		bool thereIsUpdateThread;
		
		struct ParamUpdate{
			User* user;
			float xMin, xMax, yMin, yMax;
			list<unsigned long long> idsSelected;
		};
		struct ParamNewPlayer{
			User* user;
			char user_[LEN_NAME];
			char pass[LEN_PASS];
		};
		static void* run_thread(void* user);
		static void* update_thread(void* param);
		static void* newplayer_thread(void* param);
		static void* surrender_thread(void* param);
		
	public:
		
		User(Sock* sock, Engine* engine);
		~User();
		
		void setMustExit();
		bool isExited();
		
		void run();
		
};

#endif

