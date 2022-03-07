#ifndef _replayuser_h_
#define _replayuser_h_

#include "../defs.h"
#include "../config.h"
#include "../time.h"
#include <stdio.h>
#include <string.h>
#include <boost/filesystem.hpp>

class ReplayUser
{
	private:
		int worldNumber;
		char user[LEN_NAME];
		
		FILE* messageFile;
		
	public:
		ReplayUser(int worldNumber, char* userName);
		~ReplayUser();
		
		// Here not mutua exlusion control, you have to ensure to mutua exclusion
		void saveMessage(const char* line, bool serverToClient);
		void saveMiniMap(char* data, int size, bool isPNG);
};

#endif
