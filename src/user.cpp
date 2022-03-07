
//pthread_create (&idHilo, NULL, funcion, parametro);
#include "user.h"
#include "view/playerview.h"
#include "net/sock.h"
#include "net/commandnet.h"
#include "semaphore.h"
#include "time.h"
#include "engine/engine.h"
#include "engine/action.h"
#include "bd.h"

User::User(Sock* sock, Engine* engine)
{
	this->mustExit = false;
	this->exited = false;
	this->sock = sock;
	this->thereIsUpdateThread = false;
		
	this->view = NULL;
	this->engine = engine;
	
	strcpy(this->user, "");
	this->equip = -1;
	this->loggedForGame = false;
	this->isBot = false;
	
	this->sem = new Semaphore(1);
}

User::~User()
{
	if(!sock->isClosed())
		sock->close();
	delete sock;
	if(this->view != NULL)
		delete this->view;
	delete this->sem;
}

void User::setMustExit()
{
	this->mustExit = true;
}

bool User::isExited()
{
	return this->exited;
}

void User::run()
{
	pthread_t idHilo;
	pthread_create(&idHilo, NULL, &(User::run_thread), this);
	pthread_detach(idHilo);
}


void* User::run_thread(void* user_)
{
	User* user = (User*)user_;
	
	float xMinUpdate = SIDE_MAP_METERS/2;
	float xMaxUpdate = SIDE_MAP_METERS/2;
	float yMinUpdate = SIDE_MAP_METERS/2;
	float yMaxUpdate = SIDE_MAP_METERS/2;
	list<unsigned long long> idsSelected;
	
	while(!user->mustExit)
	{
		if(user->sock->isClosed())
		{
			user->mustExit = true;
			continue;
		}
		
		// Update.
		user->sem->P();
		if(user->loggedForGame)
		{
			if(!user->thereIsUpdateThread)
			{
				user->thereIsUpdateThread = true;
				ParamUpdate* param = new ParamUpdate;
				param->user = user;
				param->xMin = xMinUpdate;
				param->xMax = xMaxUpdate;
				param->yMin = yMinUpdate;
				param->yMax = yMaxUpdate;
				param->idsSelected = idsSelected;
				pthread_t idHilo;
				pthread_create(&idHilo, NULL, &(User::update_thread), param);
				pthread_detach(idHilo);
			}
		}
		user->sem->V();
		
		string msg;
		CommandNet::Command** commands;
		if(user->sock->getMessage(msg))
		{
			list<CommandNet::Command*> commands;
			CommandNet::getCommands(msg, commands);
			while(commands.size() > 0)
			{
				CommandNet::Command* c = commands.front();
				if(c->updateArea)
				{
					// Only if it's logged.
					if(user->loggedForGame)
					{
						xMinUpdate = c->xMinUpdate;
						xMaxUpdate = c->xMaxUpdate;
						yMinUpdate = c->yMinUpdate;
						yMaxUpdate = c->yMaxUpdate;
						
						// Save view to bd
						int xView = (xMinUpdate+xMaxUpdate)/2;
						int yView = (yMinUpdate+yMaxUpdate)/2;
						bool errorBD;
						BD* bd = new BD(errorBD);
						bd->updateView(user->user, user->engine->getWorldNumber(), xView, yView);
						delete bd;
					}
				}
				else if(c->select)
				{
					// Only if it's logged.
					if(user->loggedForGame)
					{
						idsSelected = c->idsSelected;
					}
				}
				else if(c->intent != NULL)
				{
					// Only if it's logged.
					if(user->loggedForGame)
						user->engine->setAction(user->equip, user->user, c->intent);
				}
				else if(c->cancelAction)
				{
					// Only if it's logged.
					if(user->loggedForGame)
						user->engine->cancelAction(user->equip, user->user, c->idObjectCancelAction);
				}
				else if(c->newPlayer)
				{
					ParamNewPlayer* param = new ParamNewPlayer;
					param->user = user;
					strcpy(param->user_, c->user);
					strcpy(param->pass, c->pass);
					pthread_t idHilo;
					pthread_create(&idHilo, NULL, &(User::newplayer_thread), param);
					pthread_detach(idHilo);
				}
				else if(c->surrender)
				{
					// Only if it's logged.
					if(user->loggedForGame)
					{
						pthread_t idHilo;
						pthread_create(&idHilo, NULL, &(User::surrender_thread), user);
						pthread_detach(idHilo);
					}
				}
				else if(c->miniMap)
				{
					// Only if it's logged.
					if(user->loggedForGame)
					{
						if(user->view != NULL)
							user->view->setMiniMapSetting(c->miniMapRes, c->miniMapZoom, c->miniMapPosX, c->miniMapPosY);
					}
				}
				else if(c->loginGame)
				{
					char buf[10000];
					char buf2[2000];
					
					user->sem->P();

					// Error if he is aldready logged for game or if he want to login with another username.
					if(user->loggedForGame || (strcmp(user->user, "")!=0 && strcmp(user->user, c->user)!=0) )
					{
printf("a %d %d %d\n", user->loggedForGame, strcmp(user->user, "")!=0, strcmp(user->user, c->user)!=0 );
						strcpy(buf, "error,login_game;");
						user->sock->send(buf);
					}
					else
					{
						bool errorBD;
						BD* bd = new BD(errorBD);
						BD::UserInfo* info = bd->infoUserToken(c->user, c->token);
						delete bd;
						
						if(info != NULL)
						{
							// Set sock for record messages replay.
							user->sock->setReplayInfo(user->engine->getWorldNumber(), c->user);
	
							// Send the server config.
							sprintf(buf, "config,radio_mineral,%.1f,radio_base,%.1f,radio_barraca,%.1f,radio_torreta,%.1f,radio_recolector,%.1f,"\
											"radio_soldadoraso,%.1f,radio_soldadoentrenado,%.1f,radio_tanque,%.1f,radio_tanquepesado,%.1f,"\
											"max_amount_recollect_recolector,%.1f,visibility_distance,%.1f,vel_recolector,%.1f,"\
											"full_life_base,%d,full_life_barraca,%d,full_life_torreta,%d,full_life_recolector,%d,full_life_soldadoraso,%d,"\
											"full_life_soldadoentrenado,%d,full_life_tanque,%d,full_life_tanquepesado,%d;", 
											RADIO_MINERAL, RADIO_BASE, RADIO_BARRACA, RADIO_TORRETA, RADIO_RECOLECTOR, 
											RADIO_SOLDADORASO, RADIO_SOLDADOENTRENADO, RADIO_TANQUE, RADIO_TANQUEPESADO,
											MAX_AMOUNT_RECOLLECT_RECOLECTOR, VISIBILITY_DISTANCE, VEL_RECOLECTOR,
											FULL_LIFE_BASE, FULL_LIFE_BARRACA, FULL_LIFE_TORRETA, FULL_LIFE_RECOLECTOR, FULL_LIFE_SOLDADORASO,
											FULL_LIFE_SOLDADOENTRENADO, FULL_LIFE_TANQUE, FULL_LIFE_TANQUEPESADO);
							user->sock->send(buf);
							
							// Send login successfull.
							strcpy(user->user, c->user);
							user->equip = info->equip;
							user->isBot = user->user[0] == 'A' && user->user[1] == 'g' && user->user[2] == 'e' && user->user[3] == 'n' && user->user[4] == 't';
							user->loggedForGame = true;
							
							// Init view.
							xMinUpdate = info->xView-10;
							xMaxUpdate = info->xView+10;
							yMinUpdate = info->yView-10;
							yMaxUpdate = info->yView+10;
	
							sprintf(buf, "newplayercurrent,%s,%d,%d,%d;", user->user, info->equip, info->xView, info->yView);
							user->sock->send(buf);
							
							delete info;
						}
						else
						{
							strcpy(buf, "error,login_game;");
							user->sock->send(buf);
						}
					}
					
					user->sem->V();
				}
				
				commands.pop_front();
				delete c;
			}
		}
		else
			Time::sleep(SLEEP_THREAD_USER_READ_SOCK, 0);
	}

	// Wait until thread close.
	while(user->thereIsUpdateThread)
		Time::sleep(20, 0);
	
	if(!user->sock->isClosed())
		user->sock->close();
	
	user->exited = true;
}

void* User::update_thread(void* param_)
{
	char buf[MAX_SIZE_NET_MESSAGE];
	
	ParamUpdate* param = (ParamUpdate*)param_;
	User* user = param->user;
	float xMin = param->xMin;
	float xMax = param->xMax;
	float yMin = param->yMin;
	float yMax = param->yMax;
	list<unsigned long long> idsSelected = param->idsSelected;
	delete param;
	
	// I create view.
	if(user->view == NULL)
	{
		user->sem->P();
		user->view = new PlayerView(user->user, user->equip, user->isBot);
		user->sem->V();
	}
	
	bool gameEnd = false;
	
	if(user->engine->updateView(user->view, xMin, xMax, yMin, yMax, idsSelected))
	{
		gameEnd = user->view->isGameEnd();
		
		// Send commands update.
		if(user->view->toString(&buf[0], MAX_SIZE_NET_MESSAGE) > 0)
			user->sock->send(buf);

		// Send minimap.
		char info_png[MAX_BYTES_MINIMAP];
		strcpy(info_png, "minimap,");
		int size;
		bool isPNG;
		if( (size = user->view->getMiniMap( &(info_png[strlen(info_png)]), MAX_BYTES_MINIMAP, isPNG )) >= 0)
			user->sock->sendBinaryMiniMap(info_png, size + strlen("minimap,"), isPNG);
	}

	if(!user->mustExit && gameEnd)
		user->mustExit = true;
	
	user->sem->P();
	user->thereIsUpdateThread = false;
	user->sem->V();

	return NULL;
}

void* User::newplayer_thread(void* param_)
{
	ParamNewPlayer* param = (ParamNewPlayer*)param_;
	User* user = param->user;

	int xView, yView;
	char buf[300];
	
	bool errorBD;
	BD* bd = new BD(errorBD);
						
	if(!bd->login(param->user_, param->pass))
	{
		strcpy(buf, "error,newplayer;");
		user->sock->send(buf);
	}
	else
	{
		BD::WorldInfo* winfo = bd->getWorldInfo(user->engine->getWorldNumber());
		
		if(winfo == NULL)
		{
			strcpy(buf, "error,newplayer;");
			user->sock->send(buf);
		}
		else
		{
			int equip;
			if(user->engine->newPlayer(param->user_, xView, yView, equip))
			{
				char token[LEN_TOKEN];
				bd->newPlayer(param->user_, user->engine->getWorldNumber(), equip, xView, yView, token);

				sprintf(buf, "newplayerworld,%d,%s:%d,%d,%s;", user->engine->getWorldNumber(), winfo->uri, winfo->port, equip, token);
				user->sock->send(buf);
			}
			else
			{
				strcpy(buf, "error,newplayer;");
				user->sock->send(buf);
			}
			
			delete winfo;
		}
	}
	
	delete bd;
	delete param;
}

void* User::surrender_thread(void* param_)
{
	User* user = (User*)param_;
	
	char buf[300];
	
	if(!user->loggedForGame)
	{
		strcpy(buf, "error,removeplayer;");
		user->sock->send(buf);
	}
	else
	{
		if(user->engine->removePlayer(user->equip, user->user))
		{
			bool errorBD;
			BD* bd = new BD(errorBD);
			bd->removePlayer(user->user, user->engine->getWorldNumber());
			delete bd;
			
			sprintf(buf, "removeplayer,%s,%d;", user->user, user->equip);
			
			user->sem->P();
			user->equip = -1;
			user->sem->V();
			
			user->sock->send(buf);
		}
		else
		{
			strcpy(buf, "error,surrender;");
			user->sock->send(buf);
		}
	}
}





