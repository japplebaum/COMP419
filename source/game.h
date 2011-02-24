#ifndef _GAME_H
#define _GAME_H

#include <map>
#include <set>
#include <list>
#include <algorithm>

#include "string.h"
#include "IwResManager.h"
#include "IwResGroup.h"
#include "IwManagedList.h"
#include "IwGx.h"
#include "IwArray.h"
#include "IwRandom.h"

class Game;

#include "unit.h"
#include "player.h"

typedef std::map<unsigned int, std::set<Unit*>* > UnitBucket;

class Game {
	
    private:
        CIwFVec2 localLeaderPos, opponentLeaderPos;
	
		Player *localPlayer, *opponentPlayer;
	
        int numPlayers;

        CList<Unit> units;
        int numUnits;

		// Queue of units we're set to 
		std::list<Unit*> unitBuffer;

		CIwResGroup *sprites, *game;

		
		// Map from texture name to sets of unit pointers.
		// Used to optimize rendering by grouping units with the same textures
		// into adjacent draw calls.
		UnitBucket unitBucket;
    
        CIwMat view;
	
		long timesteps;

		float innerRadius, outerRadius, rotation;
	
		void initRenderState();
		
		void renderSprites();
		
		void renderWorld();
	
		void renderUI();
	                    
    public:
	
        Game();
		~Game();
        
        void addUnit(Unit *u, bool pay = false);
		CList<Unit>* getUnits();

		UnitBucket* getUnitBucket();
		
		/**
		 * Gets the inner and outer radii of the world donut.
		 *
		 * Returns a CIwFVec2 where the x coord is the inner radius
		 * and the y coord is the outer radius.
		 */
		CIwFVec2 getWorldRadius();
        	
		void tick();

        long getTimesteps();
    
        CIwMat* getViewMatrix();
        float getRotation();
        float rotate(float increment);
    
        CIwResGroup* getSprites();
	
		void render();
    
        Player *getLocalPlayer();
		Player *getOpponentPlayer();

        void setLocalPlayer(Player* local);
        void setOpponentPlayer(Player* opponent);
        void setLeaderPositions(CIwFVec2 &local, CIwFVec2 &opponent);
        CIwFVec2 getLocalLeaderPos();
        CIwFVec2 getOpponentLeaderPos();

};

#endif
