#ifndef _net_h_
#define _net_h_

#include "../defs.h"
#include <pthread.h>
#include <list>

using namespace std;

class Net
{
	private:
		
		SockServer* sockServer;
		
		static Engine* engine;
		
		list<User*> users;
		Semaphore* semUser;
		
		static Net* instance;
		Net();
		
		static void newClient(Sock* sock);
		
	public:
		
		static Net* getInstance();
		~Net();
		
		void setEngine(Engine* engine);
		
		// New connections lunch a User thread.
		bool listenWorld(int port);
		
		// New connections call the funcNewClient
		bool listenMain(int port, void (*funcNewClient)(Sock*));
		
		void stop();
};

#endif
