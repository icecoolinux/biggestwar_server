#ifndef _recolector_h_
#define _recolector_h_

#include "../../defs.h"
#include "../../config.h"
#include "unit.h"


class Recolector: public Unit
{
	private:
		float collected;
		
	public:

		Recolector(ParamUnit& params);
		
		float getCollected();
		
		bool setAction(Action* action);
		void update(int ms);
};

#endif
