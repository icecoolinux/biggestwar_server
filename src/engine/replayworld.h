#ifndef _replayworld_h_
#define _replayworld_h_

#include "../defs.h"
#include "../config.h"
#include "../time.h"
#include <stdio.h>
#include <string.h>
#include <boost/filesystem.hpp>

class ReplayWorld
{
	private:
		int worldNumber;
		
	public:
		ReplayWorld(int worldNumber);
		~ReplayWorld();
		
		void saveWorld(World* world);
		void saveMiniMap(char* data, int size);
};

#endif
