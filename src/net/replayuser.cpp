
#include "replayuser.h"


ReplayUser::ReplayUser(int worldNumber, char* userName)
{
	this->worldNumber = worldNumber;
	strcpy(this->user, userName);
	
	char buf[500];
	sprintf(buf, "%s/replays/%d/%s/", DATA_DIRECTORY, worldNumber, userName);
	boost::filesystem::create_directories(buf);

	sprintf(buf, "%s/replays/%d/%s/messages.txt", DATA_DIRECTORY, worldNumber, userName);
	this->messageFile = fopen(buf, "a");
	sprintf(buf, "%llu, START\n", Time::currentMs());
	fputs(buf, this->messageFile);
	fflush(this->messageFile);
}

ReplayUser::~ReplayUser()
{
	char buf[500];
	sprintf(buf, "%llu, END\n", Time::currentMs());
	fputs(buf, this->messageFile);
	fclose(this->messageFile);
}

void ReplayUser::saveMessage(const char* line, bool serverToClient)
{
	char buf[100];
	if(serverToClient)
		sprintf(buf, "%llu, fromServer: ", Time::currentMs());
	else
		sprintf(buf, "%llu, fromClient: ", Time::currentMs());
	fputs(buf, this->messageFile);
	
	fputs(line, this->messageFile);
	fputc('\n', this->messageFile);
	
	fflush(this->messageFile);
}

void ReplayUser::saveMiniMap(char* data, int size, bool isPNG)
{
	int pos = strlen("minimap,");
	size -= pos;
	
	char format[10];
	if(isPNG)
		strcpy(format, "png");
	else
		strcpy(format, "jpeg");
	
	char buf[500];
	sprintf(buf, "%s/replays/%d/%s/minimap_%llu.%s", DATA_DIRECTORY, this->worldNumber, this->user, Time::currentMs(), format);
	FILE* f = fopen(buf, "wb");
	fwrite(&data[pos], 1, size, f);
	fclose(f);
}

