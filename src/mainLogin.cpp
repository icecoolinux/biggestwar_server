
#include "mainLogin.h"

bool salir = false;

void handler(int s)
{
	printf("Exiting...\n");
	salir = true;
}

void newClient(Sock* sock)
{
	MainLogin::getInstance()->add(sock);
}

void processSock(Sock* s);

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Use: serverMain <port>\n");
		return 0;
	}
	
	Semaphore::init();
	
	int port = atoi(argv[1]);
	
	bool errorBD;
	BD* bd = new BD(errorBD);
	BD::Process process;
	process.pid = getpid();
	process.worldNumber = -1;
	process.isLogin = true;
	bd->addProcess(process);
	
	// Set Ctrl-C signal.
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	sigaction(SIGTERM, &sigIntHandler, NULL);
	sigaction(SIGQUIT, &sigIntHandler, NULL);
	
	Net* net = Net::getInstance();
	net->listenMain(port, &newClient);
	
	while(!salir)
	{
		MainLogin::getInstance()->process();
		Time::sleep(SLEEP_THREAD_MAIN, 0);
	}

	net->stop();
	delete MainLogin::getInstance();
	delete net;
	
	bd->delProcess(process.pid);
	delete bd;
}







MainLogin* MainLogin::instance = NULL;
MainLogin::MainLogin()
{
	sem = new Semaphore(1);
}

MainLogin* MainLogin::getInstance()
{
	if(MainLogin::instance == NULL)
		instance = new MainLogin();
	return MainLogin::instance;
}

MainLogin::~MainLogin()
{
	delete sem;
	
	while(users.size() > 0)
	{
		InfoUser* info = users.front();
		users.pop_front();
		info->sock->close();
		delete info->sock;
		delete info;
	}
}
		
void MainLogin::add(Sock* s)
{
	InfoUser* info = new InfoUser;
	strcpy(info->user, "");
	info->sock = s;
	
	addInfo(info);
}

void MainLogin::process()
{
	sem->P();
	
	list<InfoUser*>::iterator it = users.begin();
	while( it != users.end() )
	{
		InfoUser* info = *it;
		if( info->sock->isClosed() )
		{
			it = users.erase(it);
			delete info->sock;
			delete info;
		}
		else
		{
			bool quitarSock;
			
			processSock(info, quitarSock);
			
			if(quitarSock)
				it = users.erase(it);
			else
				it++;
		}
	}
	
	sem->V();
}



void MainLogin::processSock(InfoUser* info, bool &quitarSock)
{
	quitarSock = false;
	string msg;
	
	if(info->sock->getMessage(msg))
	{
		list<CommandNet::Command*> commands;
		CommandNet::getCommands(msg, commands);
		while(commands.size() > 0)
		{
			CommandNet::Command* c = commands.front();
			if(c->login)
			{
				strcpy(info->user_, c->user);
				strcpy(info->pass_, c->pass);
				pthread_t idHilo;
				pthread_create(&idHilo, NULL, &(MainLogin::login_thread), info);
				pthread_detach(idHilo);
				quitarSock = true;
			}
			else if(c->play)
			{
				pthread_t idHilo;
				pthread_create(&idHilo, NULL, &(MainLogin::play_thread), info);
				pthread_detach(idHilo);
				quitarSock = true;
			}
			
			commands.pop_front();
			delete c;
		}
	}
}

void MainLogin::addInfo(InfoUser* info)
{
	sem->P();
	users.push_back(info);
	sem->V();
}

void* MainLogin::login_thread(void* param_)
{
	InfoUser* info = (InfoUser*)param_;
	char buf[3000];

	// It's already logged or the username is blank.
	if(strcmp(info->user, "")!=0 || strcmp(info->user_, "")==0)
	{
		strcpy(buf, "error,login;");
		info->sock->send(buf);
	}
	else
	{
		bool errorBD;
		BD* bd = new BD(errorBD);
		BD::UserInfo* infoUser = bd->infoUser(info->user_, info->pass_);

		if(infoUser != NULL)
		{
			strcpy(info->user, info->user_);
			strcpy(info->pass, info->pass_);
			
			// Get user results.
			BD::Result* results;
			int amountResults = bd->getResults(info->user, results);
			
			// Make login message.
			if(infoUser->gamelogged)
				sprintf(buf, "login,%d,%d,%d,true,%s:%d,%s,%d", infoUser->wins, infoUser->lost, infoUser->points, infoUser->uri, infoUser->port, infoUser->token, amountResults);
			else
				sprintf(buf, "login,%d,%d,%d,false,,,%d", infoUser->wins, infoUser->lost, infoUser->points, amountResults);
			
			// Copy results info to message packet.
			list<int> worldResultSended;
			for(int i=0; i<amountResults; i++)
			{
				char buf2[100];
				char surrender_String[10];
				char teamWin_String[10];
				char winToEnd_String[10];
				strcpy(surrender_String, "false");
				strcpy(teamWin_String, "false");
				strcpy(winToEnd_String, "false");
				if(results[i].surrender)
					strcpy(surrender_String, "true");
				if(results[i].teamWin)
					strcpy(teamWin_String, "true");
				if(results[i].winToEnd)
					strcpy(winToEnd_String, "true");
				
				sprintf(buf2, ",%d,%d,%d,%s,%s,%s", results[i].equip, results[i].rank, results[i].points, surrender_String, teamWin_String, winToEnd_String);
				if( (strlen(buf)+strlen(buf2)+1) >= 3000)
					break;
				else
					worldResultSended.push_back(results[i].world);
				
				strcat(buf, buf2);
			}
			
			// Close and send packet
			strcat(buf,";");
			info->sock->send(buf);
			
			// Clear results sended.
			list<int>::iterator it = worldResultSended.begin();
			while(it != worldResultSended.end())
			{
				bd->deleteResult(info->user, *it);
				it++;
			}
			
			delete infoUser;
		}
		else
		{
			strcpy(buf, "error,login;");
			info->sock->send(buf);
		}
		
		delete bd;
	}
	
	MainLogin::getInstance()->addInfo(info);
}

void* MainLogin::play_thread(void* param_)
{
	InfoUser* info = (InfoUser*)param_;

	char buf[500];
	
	// Not logged.
	if( strcmp(info->user, "")==0 )
	{
printf("A\n");
		strcpy(buf, "error,login;");
		info->sock->send(buf);
	}
	else
	{
		bool errorBD;
		BD* bd = new BD(errorBD);
		
		BD::WorldInfo* worlds;
		int cantWorlds = bd->getWorldsInfo(worlds);
		
		
		if(cantWorlds == 0)
		{
printf("A1\n");
			strcpy(buf, "error,play;");
			info->sock->send(buf);
		}
		else
		{
			// First of all get world that only accepts agents and world that accepts both (humans and agents)
			vector<int> worldsOnlyAgents;
			vector<int> worldsBoth;
			for(int i=0; i<cantWorlds; i++)
			{
				// If  there's no free space then continue with other
				if(worlds[i].amountPlayers >= EQUIPS*MAX_PLAYERS_BY_EQUIP)
					continue;

				// Only agents
				if(worlds[i].stage >= 20 && worlds[i].stage < 30)
					worldsOnlyAgents.push_back(i);
				// Both
				else if(worlds[i].stage >= 10 && worlds[i].stage < 20)
					worldsBoth.push_back(i);
			}
			
			// Shuffle the index vectors, this do to select a world random
			random_shuffle ( worldsOnlyAgents.begin(), worldsOnlyAgents.end() );
			random_shuffle ( worldsBoth.begin(), worldsBoth.end() );
			
			char url[500];
			bool putit = false;
			// If it's an agent put it into a world that only accepts agents
			if( info->user[0] == 'A' && info->user[1] == 'g' && info->user[2] == 'e' && info->user[3] == 'n' && info->user[4] == 't')
			{
				// There's at least a world that accepts only agents
				if(worldsOnlyAgents.size() > 0)
				{
					// Get a random world
					int indexWorld = worldsOnlyAgents.back();
					sprintf(url, "%s:%d", worlds[indexWorld].uri, worlds[indexWorld].port);
					putit = true;
				}
			}
			// If i dont put it then intent to put in a world that accepts humans
			if(!putit)
			{
				// There's at least a world that accepts humans and agents
				if(worldsBoth.size() > 0)
				{
					// Get a random world
					int indexWorld = worldsBoth.back();
					sprintf(url, "%s:%d", worlds[indexWorld].uri, worlds[indexWorld].port);
					putit = true;
				}
			}

			// I cannot put it into a world, error
			if(!putit)
			{
printf("A2\n");
				strcpy(buf, "error,play;");
				info->sock->send(buf);
			}
			else
			{
				Sock* s = new Sock();
				if(!s->connectToServer(url))
				{
printf("A3\n");
					strcpy(buf, "error,play;");
					info->sock->send(buf);
				}
				else
				{
					sprintf(buf, "newplayer,%s,%s;", info->user, info->pass);
					if(!s->send(buf))
					{
printf("A4\n");
						strcpy(buf, "error,play;");
						info->sock->send(buf);
					}
					else
					{
						string msg;
						while(true)
						{
							if(s->getMessage(msg))
							{
								info->sock->send(msg);
								break;
							}
							else if(s->isClosed())
							{
printf("A5\n");
								strcpy(buf, "error,play;");
								info->sock->send(buf);
								break;
							}
							Time::sleep(SLEEP_THREAD_USER_READ_SOCK, 0);
						}
					}
				}
				delete s;
			}
			
			delete worlds;
		}
		
		delete bd;
	}
	
	MainLogin::getInstance()->addInfo(info);
}

