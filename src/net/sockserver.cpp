
#include "sockserver.h"
#include "sock.h"
#include "../semaphore.h"
#include "../time.h"

SockServer::SockServer(void (*newClient)(Sock*))
{
	this->sem = new Semaphore(1);
	this->newClient = newClient;
	this->connectionsClossed = false;
}

SockServer::~SockServer()
{
	delete this->sem;
}

bool SockServer::start(int port)
{
	this->server.clear_access_channels(websocketpp::log::alevel::all);
	// Register the message handlers.
	/*
	this->server.set_validate_handler(&WebsocketServer::on_validate);
	this->server.set_fail_handler(&WebsocketServer::on_fail);
*/
	this->server.set_close_handler(bind(&SockServer::on_close,this,::_1));
	this->server.set_message_handler(bind(&SockServer::on_message,this,::_1,::_2));
	
	// Initialising WebsocketServer.
    this->server.init_asio();
	
	// For dev.
	this->server.set_reuse_addr(true);
	
	// Listen on port.
	try {
		this->server.listen(port);
	} 
	catch(websocketpp::exception const &e) 
	{
		return false;
	}
	
	// Starting Websocket accept.
    websocketpp::lib::error_code ec;
    this->server.start_accept(ec);
    if (ec) {
        // Can log an error message with the contents of ec.message() here.
        return false;
    }
    
    pthread_t idHilo;
    pthread_create(&idHilo, NULL, &(SockServer::run_thread), this);
	pthread_detach(idHilo);
    
    return true;
}

void* SockServer::run_thread(void* sockServer_)
{
	SockServer* sockServer = (SockServer*)sockServer_;
	try {
        sockServer->server.run();
		sockServer->connectionsClossed = true;
printf("gone\n");
    } catch(websocketpp::exception const &e) {
        return NULL;
    }

    return NULL;
}

bool SockServer::stop()
{
	// Stopping the Websocket listener and closing outstanding connections.
    websocketpp::lib::error_code ec;
    this->server.stop_listening(ec);
    if (ec) {
        // Failed to stop listening. Log reason using ec.message().
        return false;
    }

    // Close all existing websocket connections.
    while(this->websockets.size() > 0)
	{
		/*
		unordered_map<string, connection_hdl>::iterator it = this->websockets.begin();
		Sock* s = *it;
		this->websockets.erase(it);
		*/
		Sock* s = this->websockets.front();
		//this->websockets.pop_front();
		s->close();
//		delete s;
    }

    // Wait until all connection are closed.
    while(!this->connectionsClossed)
		Time::sleep(20, 0);

    // Force stop the endpoint.
    //this->server.close();
	
	return true;
}


bool SockServer::closeSock(Sock* sock)
{
//	this->server.pause_reading(sock->getConnection());
	
	string data = "close";
	websocketpp::lib::error_code ec;
	
	this->server.close(sock->getConnection(), websocketpp::close::status::normal, data, ec);
	
	// Remove websocket from the map.
	this->sem->P();
	//this->websockets.erase(sock->getConnection());
	this->websockets.remove(sock);
	this->sem->V();
		
	if (ec)
		return false;
	else	
		return true;
}

bool SockServer::send(Sock* sock, string data)
{
	websocketpp::lib::error_code ec;
	this->server.send(sock->getConnection(), data, websocketpp::frame::opcode::text, ec);
	
	if (ec)
		return false;
	else
		return true;
}

bool SockServer::sendBinary(Sock* sock, char* data, int size)
{
	websocketpp::lib::error_code ec;
	this->server.send(sock->getConnection(), data, size, websocketpp::frame::opcode::binary, ec);
	
	if (ec)
		return false;
	else
		return true;
}

void SockServer::on_message(SockServer* sockServer, connection_hdl hdl, Server::message_ptr msg) 
{
	string m = msg->get_payload();

	Sock* sock = NULL;

/*websocketpp::lib::error_code ec;
sockServer->server.close(hdl, websocketpp::close::status::normal, "close", ec);
return;	*/
	sockServer->sem->P();
	
	list<Sock*>::iterator it;
	for(it = sockServer->websockets.begin(); it != sockServer->websockets.end(); it++)
	{
		//if((*it)->getConnection() == hdl)
		connection_hdl hdlSock = (*it)->getConnection();
		if(!hdlSock.owner_before(hdl) && !hdl.owner_before(hdlSock))
		{
			sock = *it;
			break;
		}
	}
	
	if(sock == NULL)
	{
		sock = new Sock(hdl, sockServer);
		sockServer->websockets.push_front(sock);
		sock->setMessage(m);
		sockServer->newClient(sock);
	}
	else
	{
		sock->setMessage(m);
	}
	/*
	if(this->websockets.find(hdl) == this->websockets.end())
	{
		Sock* s = new Sock(hdl, this);
		this->websockets[hdl] = s;
		s->setMessage(m);
		this->newClient(s);
	}
	else
		this->websockets[hdl]->setMessage(m);
	*/

	sockServer->sem->V();
}

void SockServer::on_close(SockServer* sockServer, connection_hdl hdl)
{
	Sock* sock = NULL;
	
	sockServer->sem->P();
	
	list<Sock*>::iterator it;
	for(it = sockServer->websockets.begin(); it != sockServer->websockets.end(); it++)
	{
		//if((*it)->getConnection() == hdl)
		connection_hdl hdlSock = (*it)->getConnection();
		if(!hdlSock.owner_before(hdl) && !hdl.owner_before(hdlSock))
		{
			sock = *it;
			break;
		}
	}
	sockServer->sem->V();
	
	// Remove websocket from the map.
	//this->websockets.erase(sock->getConnection());
	if(sock != NULL)
		sock->close();
}



/*
bool WebsocketServer::on_validate(connection_hdl hdl) {
    websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(hdl);
    websocketpp::uri_ptr uri = con->get_uri();
    string query = uri->get_query(); // returns empty string if no query string set.
    if (!query.empty()) {
        // Split the query parameter string here, if desired.
        // We assume we extracted a string called 'id' here.
    }
    else {
        // Reject if no query parameter provided, for example.
        return false;
    }
     
    this->sem->P();
     
    websockets.insert(std::pair<string, connection_hdl>(id, hdl));
    
	this->sem->V();
 
    return true;
}




void WebsocketServer::on_fail(connection_hdl hdl) {
    websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(hdl);
    websocketpp::lib::error_code ec = con->get_ec();
    // Websocket connection attempt by client failed. Log reason using ec.message().
}
*/ 





