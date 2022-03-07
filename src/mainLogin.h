#ifndef _mainLogin_h_
#define _mainLogin_h_

#include <stdio.h>
#include <algorithm> 
#include <signal.h>
#include "net/sock.h"
#include "net/commandnet.h"
#include "semaphore.h"
#include "bd.h"
#include "config.h"
#include "net/net.h"
#include "time.h"
#include "net/sockserver.h"

using namespace std;

class MainLogin
{
	private:
		
		struct InfoUser {
			Sock* sock;
			char user[LEN_NAME+1];
			char pass[LEN_PASS+1];
			
			char user_[LEN_NAME+1];
			char pass_[LEN_PASS+1];
		};
		
		Semaphore* sem;
		list<InfoUser*> users;
		
		static void* login_thread(void* param);
		static void* play_thread(void* param);
		
		void processSock(InfoUser* info, bool &quitarSock);
		void addInfo(InfoUser* info);
		
		static MainLogin* instance;
		MainLogin();
		
	public:
		
		static MainLogin* getInstance();
		~MainLogin();
		
		void add(Sock* s);
		void process();
};

#endif
