#ifndef _action_h_
#define _action_h_

#include "../defs.h"
#include "vec2.h"
#include <unordered_map>
#include <list>

using namespace std;

struct ParamAction{
	enum ActionType type;
	enum ActionSubType subType;

	unsigned long long unitID;
	unsigned long long unit2ID;
	unsigned long long buildID;
	unsigned long long build2ID;
	unsigned long long objectMapID;
	
	vec2 pos;
	
	enum ObjectType make;
};

class Action
{
	private:
		static unordered_map<Object*, list<Action*>> actionsOfObject;
		
		Unit* unit;
		Unit* unit2;
		Building* build;
		Building* build2;
		Object* objectMap;
		
		unsigned long long unitID;
		unsigned long long unit2ID;
		unsigned long long buildID;
		unsigned long long build2ID;
		unsigned long long objectMapID;
		
		// Unset the action to the object.
		static void unsetAction(Object* obj, Action* action);
	public:
		
		enum ActionType type;
		enum ActionSubType subType;
		
		vec2 pos;
		
		enum ObjectType make;
		
		
		
		Action(ParamAction& params);
		~Action();
		
		bool setIds(IDS* ids);
		
		unsigned long long getUnitID();
		unsigned long long getUnit2ID();
		unsigned long long getBuildID();
		unsigned long long getBuild2ID();
		unsigned long long getObjectMapID();
		Unit* getUnit();
		Unit* getUnit2();
		void setUnit2(Unit* unit2);
		Building* getBuild();
		Building* getBuild2();
		void setBuild2(Building* build2);
		Object* getObjectMap();
		void setObjectMap(Object* obj);
		
		bool isMoveAction();
		
		// Remove and unset actions that have this object.
		// Notice to subject to delete the action.
		static void removeLinkActions(Object* obj);
		
		// Return all the actions that has the object (as subject or passive).
		static void getActionFromObject(Object* obj, list<Action*> &actions);
		
		void print();
};

#endif














