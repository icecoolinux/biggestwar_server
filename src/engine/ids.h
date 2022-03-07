#ifndef _ids_h_
#define _ids_h_

class IDS;

#include <unordered_map>
#include "objects/object.h"

using namespace std;

class IDS
{
	private:
		
		unsigned long long nextId;
		unordered_map<unsigned long long, Object*> idToObj; 
		
		Semaphore* sem;
		
	public:
		
		IDS(unsigned long long next);
		~IDS();
		
		unsigned long long getNew();
		void free(unsigned long long id);
		void setObject(unsigned long long id, Object* obj);
		
		Object* getObject(unsigned long long id);
};


#endif


