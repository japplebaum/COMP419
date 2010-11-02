#ifndef _ARTIST_H
#define _ARTIST_H

class Artist;


#include "IwUtil.h"
#include "Iw2D.h"

#include "game.h"
#include "unit.h"
#include "gridcell.h"


class Artist {

    private:
    
        Game* game;
        UIManager* ui;
        CIwArray<GridCell*>* changeList;
    
    public:
        
        Artist(Game* _game, UIManager* _ui);
		~Artist();
        
        /**
        Alert the Artist object of all cells that changed, and hence
        need to be re-rendered.
        
        @param changeList the list of cells that have changed
        */
        void updateChangeList(CIwArray<GridCell*>* _changeList);
        
        /**
        Render the entire screen.
		 
		@param frameNumber the number of the current frame - gets passed down to Units' display() methods
        */
        void render(int frameNumber);
};

#endif