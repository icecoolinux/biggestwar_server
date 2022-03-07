
#include "sock.h"
#include "sockserver.h"
#include "../semaphore.h"

Sock::Sock()
{
	this->isAlone = true;
	this->server = NULL;
	this->isError = false;
	this->threadAloneInited = false;
	this->connected = false;
	this->closed = false;
	this->sem = new Semaphore(1);
	this->replay = NULL;
}

Sock::Sock(connection_hdl hdl, SockServer* server)
{
	this->isAlone = false;
	this->server = server;
	this->hdl = hdl;
	this->isError = false;
	this->threadAloneInited = false;
	this->connected = true;
	this->closed = false;
	this->sem = new Semaphore(1);
	this->replay = NULL;
}

Sock::~Sock()
{
	if(!this->closed)
		this->close();
	
	if(this->isAlone && this->threadAloneInited)
	{
		this->endpoint.stop_perpetual();
		this->manageThread->join();
	}
	
	delete this->sem;
	
	if(this->replay != NULL)
		delete this->replay;
}

bool Sock::connectToServer(string uri)
{
	// Init the connections manager if it's alone
	if(this->isAlone)
	{
		this->sem->P();
		if(!this->threadAloneInited)
		{
			// Initialize ASIO
			this->endpoint.clear_access_channels(websocketpp::log::alevel::all);
			this->endpoint.init_asio();
			this->endpoint.start_perpetual(); // For still run the thread if there's not connections
			
			// Register our handlers
			this->endpoint.set_open_handler(bind(&Sock::on_open, this,::_1));
			this->endpoint.set_fail_handler(bind(&Sock::on_fail, this,::_1));
			this->endpoint.set_message_handler(bind(&Sock::on_message, this,::_1,::_2));
			this->endpoint.set_close_handler(bind(&Sock::on_close, this,::_1));
		
			// Lunch the thread
			this->manageThread.reset(new websocketpp::lib::thread(&client::run, &(this->endpoint) ));
		
			this->threadAloneInited = true;
		}
		this->sem->V();
	}

	this->uri = uri;
	
	// Create a connection to the given URI and queue it for connection once
	// the event loop starts
	websocketpp::lib::error_code ec;
	client::connection_ptr con = this->endpoint.get_connection(this->uri, ec);
	if(ec)
	{
		printf("Error here!!!!!!!!!!!!!!!\n");
		return false;
	}
	this->endpoint.connect(con);

	// Wait to connect or error.
	int timeToWaiting = 0;
	while(!this->connected && !this->isError)
	{
		Time::sleep(10,0);
		timeToWaiting += 10;
		if(timeToWaiting > TIMEOUT_CONNECT_TO_SERVER_MS)
			break;
	}

	return this->connected;
}

connection_hdl Sock::getConnection()
{
	return this->hdl;
}

bool Sock::isConnected()
{
	return connected;
}

bool Sock::isClosed()
{
	return this->closed;
}

bool Sock::getMessage(string &msg)
{
	this->sem->P();
	
	if(this->messages.size() == 0)
	{
		this->sem->V();
		return false;
	}
	
	msg = this->messages.front();
	this->messages.pop_front();
	
	this->sem->V();
	return true;
}

// Put back for default.
void Sock::setMessage(string msg)
{
	this->sem->P();
	if(this->replay != NULL)
		this->replay->saveMessage(msg.c_str(), false);
	
	this->messages.push_back(msg);
	this->sem->V();
}
void Sock::setMessageFront(string msg)
{
	this->sem->P();
	this->messages.push_front(msg);
	this->sem->V();
}

bool Sock::send(string data) 
{
	if(this->replay != NULL)
	{
		this->sem->P();
		this->replay->saveMessage(data.c_str(), true);
		this->sem->V();
	}
	
	if(isAlone)
	{
		websocketpp::lib::error_code ec;
		this->endpoint.send(this->hdl, data, websocketpp::frame::opcode::text);
		if(ec)
			return false;
		else
			return true;
	}
	else
		return this->server->send(this, data);
}

bool Sock::sendBinary(char* data, int size) 
{
	if(isAlone)
	{
		websocketpp::lib::error_code ec;
		this->endpoint.send(this->hdl, data, size, websocketpp::frame::opcode::binary, ec);
		if(ec)
			return false;
		else
			return true;
	}
	else
		return this->server->sendBinary(this, data, size);
}

bool Sock::sendBinaryMiniMap(char* data, int size, bool isPNG)
{
	if(this->replay != NULL)
	{
		this->sem->P();
		this->replay->saveMiniMap(data, size, isPNG);
		this->sem->V();
	}
	
	return this->sendBinary(data, size);
}

bool Sock::close() 
{
	this->closed = true;
	
	if(isAlone)
	{
		if(connected)
			this->endpoint.close(this->hdl, websocketpp::close::status::normal,"");
	}
	else
	{
		if(!this->server->closeSock(this))
			return false;
	}
	return true;
}

// World name and user for record sent and recived messages.
void Sock::setReplayInfo(int worldNumber, char* user)
{
	this->replay = new ReplayUser(worldNumber, user);
}




// Handlers for alone sock.
void Sock::on_open(Sock* sock, websocketpp::connection_hdl hdl)
{
	sock->hdl = hdl;
	sock->connected = true;
}
void Sock::on_fail(Sock* sock, websocketpp::connection_hdl hdl)
{
    sock->isError = true;
}
void Sock::on_message(Sock* sock, websocketpp::connection_hdl hdl, message_ptr msg)
{
	sock->setMessage(msg->get_payload());
}
void Sock::on_close(Sock* sock, websocketpp::connection_hdl hdl)
{
	sock->closed = true;
	sock->connected = false;
}



