#ifndef _objectview_h_
#define _objectview_h_

#include "../defs.h"
#include "../config.h"
#include "../engine/vec2.h"

class ObjectView
{
	protected:
		
		
	public:
		
		// This flag show that is object is in view.
		// Then the object will be updated.
		bool inTheView;
		
		// Check if the object is into selection.
		// Then the object will be updated.
		bool inTheSelection;
		
		unsigned long long id;
		enum ObjectType type;
		vec2 pos;
		
		char playerName[LEN_NAME];
		
		bool mine;
		bool enemy;
		bool aliade;
		bool map;
		
		float life;
		float fullLife;
		float creada;
		float construccionCreando;
		float construccionFull;
		
		ActionView* action;
		
		unsigned long long tsLastAttack;
		
		float collected; // If it's a recolector.
		
		float amount; // If it's a mineral.
		
		ObjectView(Object* obj, char* myName, int myEquip);
		~ObjectView();
		
		bool thereAreChange(Object *obj);
		
		// return true if there are changes.
		bool getChanges(Object* obj, PlayerViewUpdate &update);
		
		void getNew(PlayerViewUpdate &update);
};

#endif

