
#include <stdio.h>
#include <signal.h>
#include "engine/engine.h"
#include "engine/worldresult.h"
#include "config.h"
#include "net/net.h"
#include "time.h"
#include "net/sockserver.h"
#include "semaphore.h"

bool salir = false;

void handler(int s)
{
	printf("Exiting...\n");
	salir = true;
}

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("Use: serverWorld <uri> <port>\n");
		return 0;
	}
	
	srand(time(NULL));
	Semaphore::init();
	
	char uri[500];
	int port;
	strcpy(uri, argv[1]);
	port = atoi(argv[2]);

	// Set Ctrl-C signal.
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	sigaction(SIGTERM, &sigIntHandler, NULL);
	sigaction(SIGQUIT, &sigIntHandler, NULL);

	// Add world to database.
	bool errorBD;
	BD* bd = new BD(errorBD);
	int worldNumber = bd->newWorld(uri, port, 0);

	// Make the world.
	Engine* engine = new Engine(worldNumber);

	// Add this process to database.
	BD::Process process;
	process.pid = getpid();
	process.worldNumber = engine->getWorldNumber();
	process.isLogin = false;
	bd->addProcess(process);

	Net* net = Net::getInstance();
	net->setEngine(engine);
	net->listenWorld(port);
	
	
	int ms = MS_UPDATE_STEP;
	Time* t = new Time();
	while(!salir)
	{
		t->start();
		
		int stage = engine->getStage();
		int amountPlayers = engine->getAmountPlayers();
		
		engine->step(ms);
		
		if(engine->isOver())
			break;
		
		// Update world info.
		if(stage != engine->getStage())
			bd->setStageWorld(worldNumber, engine->getStage());
		if(amountPlayers != engine->getAmountPlayers())
			bd->setAmountPlayersWorld(worldNumber, engine->getAmountPlayers());
		
		ms = t->getMs();
		t->sleep(MS_UPDATE_STEP-ms, 0);
		
		ms = t->stop();
	}

	
	// Delete world from database.
	bd->removeWorldAndPlayers(process.worldNumber);
	
	// Delete this process from database.
	bd->delProcess(process.pid);
	
	
	
	// If the game end normally.
	if(engine->isOver())
	{
		// Get result and save it to database.
		WorldResult* result = engine->getResult();
		result->saveToDB(bd);
		
		// Wait some seconds to send results to players .
		unsigned long long tsGameEnd = Time::currentMs();
		while( (Time::currentMs()-tsGameEnd) < 10000)
		{
			t->start();
			
			engine->step(ms);
			
			ms = t->getMs();
			t->sleep(MS_UPDATE_STEP-ms, 0);
			ms = t->stop();
		}
	}
	
	
	
	// Stop and delete world.
	engine->stop();
	net->stop();
	delete engine;
	delete t;
	delete net;
	
	delete bd;
}












