#ifndef _ICING_H_
#define _ICING_H_

#define ICING_FINAL_SCALE 0.12f

#include "IwRandom.h"
#include "worldobject.h"
#include "player.h"

class Icing : public WorldObject {
	
	private:
		Player* owner;
        float angle;
		float scale;
	
	public:	
		Icing(const CIwFVec2 position, Game* game, Player* owner);
	
		Icing(const Icing& newIcing);
	
		Player* getOwner();
		
		virtual void display();	
};

#endif
