
#include "net.h"
#include "sockserver.h"
#include "../semaphore.h"
#include "../user.h"
#include "../time.h"

Net* Net::instance = NULL;
Engine* Net::engine = NULL;

Net::Net()
{
	this->semUser = new Semaphore(1);
}

Net* Net::getInstance()
{
	if(instance == NULL)
		instance = new Net();
	return instance;
}

Net::~Net()
{
	delete this->semUser;
}

void Net::setEngine(Engine* engine)
{
	Net::engine = engine;
}

// New client.
void Net::newClient(Sock* sock)
{
	User* user = new User(sock, Net::engine);
	
	Net* net = Net::getInstance();
	
	net->semUser->P();
	net->users.push_back(user);
	net->semUser->V();
	
	user->run();
}


bool Net::listenWorld(int port)
{
	// Start sock server.
	this->sockServer = new SockServer(&(Net::newClient));
	this->sockServer->start(port);
}

bool Net::listenMain(int port, void (*funcNewClient)(Sock*))
{
	// Start sock server.
	this->sockServer = new SockServer(funcNewClient);
	this->sockServer->start(port);
}

void Net::stop()
{
	this->semUser->P();
	
	// Close users.
	list<User*>::iterator it;
	for(it = this->users.begin(); it != this->users.end(); it++)
		(*it)->setMustExit();
	
	// Wait to close and delete users.
	while(this->users.size() > 0)
	{
		User* p = this->users.front();
		this->users.pop_front();
		while(!p->isExited())
			Time::sleep(10,0);
		delete p;
	}

	this->semUser->V();
	
	// Delete sock server.
	this->sockServer->stop();
	delete this->sockServer;
	this->sockServer = NULL;
}










