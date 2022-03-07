#ifndef _sockserver_h_
#define _sockserver_h_

#include "../defs.h"
#include <unordered_map>
#include <list>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>


// https://mayaposch.wordpress.com/2015/09/16/creating-a-websocket-server-with-websocket/

// namespace merging
using websocketpp::connection_hdl;
using namespace std;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef websocketpp::server<websocketpp::config::asio> Server;

class SockServer
{
	private:
		
		Server server;
		
		/*unordered_map<connection_hdl, Sock*> websockets; */
		list<Sock*> websockets;
		Semaphore* sem;
		
		bool connectionsClossed;
		
		// callbacks
		/*
		static bool on_validate(websocketpp::connection_hdl hdl);
		static void on_fail(websocketpp::connection_hdl hdl);
		
	*/
		static void on_message(SockServer* sockServer, connection_hdl hdl, Server::message_ptr msg);
		static void on_close(SockServer* sockServer, connection_hdl hdl);
		
		static void* run_thread(void* sockServer_);
		void (*newClient)(Sock*) ;
		
	public:
		
		SockServer(void (*newClient)(Sock*));
		~SockServer();
		
		bool start(int port);
		bool stop();
		
		bool closeSock(Sock* sock);
		bool send(Sock* sock, string data);
		bool sendBinary(Sock* sock, char* data, int size);
};

#endif
