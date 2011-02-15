#ifndef _LEADER_H
#define _LEADER_H

#include "unit.h"
#include "IwRandom.h"

class Leader : public Unit {
	private:
		short framesUntilUpdate;
	
	public:
		Leader(Player* owner, CIwFVec2 position, Game* game);
	
		~Leader(){};

		virtual void update();
	
		virtual unit_type getType();
	
		virtual float getAngle();
};

#endif
