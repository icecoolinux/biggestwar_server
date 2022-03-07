#ifndef _building_h_
#define _building_h_

struct ParamBuilding;
class Building;

#include "../world.h"
#include "../vec2.h"
#include "recolector.h"
#include "soldadoraso.h"
#include "soldadoentrenado.h"
#include "tanque.h"
#include "tanquepesado.h"

struct ParamBuilding{
	World* world;
	Player* player;
	enum ObjectType type;
	unsigned long long id;
	vec2 pos;
	float radio;
	int life;
	int creada;
};
		
class Building: public Object
{
	private: 
		void getResourceBuildCost(Action* action, int &minerals, int &oil);
		
	protected:
		
		
	public:
		
		Player* player;
		float life;
		float fullLife;
		float creada;
		float construccionCreando;
		float construccionFull;
		
		Building(ParamBuilding& params);
		~Building();
		
		virtual bool setAction(Action* action);
		virtual bool cancelAction();
		virtual void update(int ms);
};

#endif

