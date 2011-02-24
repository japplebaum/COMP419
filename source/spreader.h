#ifndef _SPREADER_H
#define _SPREADER_H

#include <stdlib.h>
#include <hash_map>
#include <utility>

#include "unit.h"
#include <cmath>

static int iceTiers[3] = {6, 12, 18};

class Spreader : public Unit {
    private:
        bool icings[36];
        static const int spreadDelay = 12;
    
	public:
		Spreader(Player* owner, CIwFVec2 position, Game* game);
	
		Spreader(const Spreader& newSpreader);
	
		~Spreader();

		virtual void update();

		virtual unit_type getType();
	
		virtual Unit* spawnCopy();
	    
        int nearbyIceCount(CIwFVec2 loc);
        int useNearbyIce(CIwFVec2 loc, int max);
        CIwFVec2 *closestIce(CIwFVec2 loc);
};

#endif
