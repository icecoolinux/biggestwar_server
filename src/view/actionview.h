#ifndef _actionview_h_
#define _actionview_h_

#include "../defs.h"
#include "../config.h"
#include "../engine/vec2.h"

class ActionView
{
	private:
		
		
	public:
		
		enum ActionType type;
		enum ActionSubType subType;
		
		unsigned long long unitID;
		unsigned long long unit2ID;
		unsigned long long buildID;
		unsigned long long build2ID;
		unsigned long long objectMapID;
		
		vec2 pos;
		
		enum ObjectType make;
		
		
		
		ActionView(Action* action);
		~ActionView();
		
		bool isDifferent(Action* action);
		
		void getNew(PlayerViewUpdate &update);
};

#endif














