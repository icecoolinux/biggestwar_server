
#include "ids.h"
#include "../semaphore.h"

IDS::IDS(unsigned long long next)
{
	this->nextId = next;
	this->sem = new Semaphore(1);
}

IDS::~IDS()
{
	delete this->sem;
}
	
unsigned long long IDS::getNew()
{
	this->sem->P();
	
	this->idToObj[this->nextId] = NULL;
	this->nextId++;
	unsigned long long ret = this->nextId-1;
	
	this->sem->V();
	
	return ret;
}

void IDS::free(unsigned long long id)
{
	this->sem->P();
	this->idToObj.erase(id);
	this->sem->V();
}

void IDS::setObject(unsigned long long id, Object* obj)
{
	this->sem->P();
	this->idToObj[id] = obj;
	this->sem->V();
}

Object* IDS::getObject(unsigned long long id)
{
	Object* ret = NULL;
	
	this->sem->P();
	
	if(this->idToObj.find(id) != this->idToObj.end())
		ret = this->idToObj[id];
	
	this->sem->V();
	
	return ret;
}
