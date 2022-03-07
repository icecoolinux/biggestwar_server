
#include "replayworld.h"


ReplayWorld::ReplayWorld(int worldNumber)
{
	this->worldNumber = worldNumber;
	
	char buf[500];
	sprintf(buf, "%s/replays/%d/saves/", DATA_DIRECTORY, worldNumber);
	boost::filesystem::create_directories(buf);
}

ReplayWorld::~ReplayWorld()
{
	
}

void ReplayWorld::saveWorld(World* world)
{
	char buf[500];
	sprintf(buf, "%s/replays/%d/saves/%llu.map", DATA_DIRECTORY, this->worldNumber, Time::currentMs());
	FILE* f = fopen(buf, "w");
	fputs(buf, f);//TODO
	fclose(f);
}

void ReplayWorld::saveMiniMap(char* data, int size)
{
	int pos = strlen("minimap,");
	size -= pos;
	
	char buf[500];
	sprintf(buf, "%s/replays/%d/saves/minimap_%llu.png", DATA_DIRECTORY, this->worldNumber, Time::currentMs());
	FILE* f = fopen(buf, "wb");
	fwrite(&data[pos], 1, size, f);
	fclose(f);
}

