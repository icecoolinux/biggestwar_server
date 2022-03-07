#ifndef _sock_h_
#define _sock_h_

#include "../defs.h"
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <list>
#include "replayuser.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

using websocketpp::connection_hdl;
using namespace std;

class Sock
{
	private:
		
		// Alone is when connect to a uri server.
		// If it's false then is a socket from sockserver.
		bool isAlone;
		
		// When it's created alone.
		client endpoint;
		bool threadAloneInited;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> manageThread;
		string uri;
		static void on_open(Sock* sock, websocketpp::connection_hdl hdl);
		static void on_fail(Sock* sock, websocketpp::connection_hdl hdl);
		static void on_message(Sock* sock, websocketpp::connection_hdl hdl, message_ptr msg);
		static void on_close(Sock* sock, websocketpp::connection_hdl hdl);
		
		// When it's created from the sock server.
		SockServer* server;
		
		connection_hdl hdl;
		list<string> messages;
		bool closed;
		bool connected;
		bool isError;
		ReplayUser* replay;
		Semaphore* sem;
		
	public:
		
		Sock();
		Sock(connection_hdl hdl, SockServer* server);
		~Sock();
		
		bool connectToServer(string uri);
		
		connection_hdl getConnection();
		
		bool isConnected();
		bool isClosed();
		
		bool getMessage(string &msg);
		void setMessage(string msg);// Back for default
		void setMessageFront(string msg);
		
		bool send(string data);
		bool sendBinary(char* data, int size);
		// It's similar to sendBinary but it's just for save the minimap file with correct extension.
		bool sendBinaryMiniMap(char* data, int size, bool isPNG);
		
		bool close();
		
		// World name and user for record sent and recived messages.
		void setReplayInfo(int worldNumber, char* user);
};

#endif

