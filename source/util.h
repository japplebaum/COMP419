#ifndef _UTIL_H_
#define _UTIL_H_

#define TO_RADIANS(X) ((X) * (PI / 180.0f))
#define SQ(x) ((x)*(x))
#define THETA_DIFF(X, Y) (min(abs((X)-(Y)), 2*PI - abs((X) - (Y))))

#include "IwGx.h"
#include "IwGeom.h"
#include "worldobject.h"
#include <list>

float min(float x, float y);

void renderImageWorldSpace(CIwFVec2& position, float angle, float scaleFactor, int textureSize, float worldRot = 0.0, int frameNumber = 0, int numFrames = 1, float z = 0.0f);

/**
 * Converts a 2-Float vector from cartesian coordinates to polar coordinates.
 * The x component represents the radius.
 * The y component represents the theta.
 */
void polarize(CIwFVec2& v);

void polarToXY(CIwFVec2& v);

CIwFVec2 worldify(int32 x, int32 y, float innerRadius, float rotation);
float angle_diff(const CIwFVec2& pos1, const CIwFVec2& pos2);

/**
Check if a position sits within the world. The position is in x, y coordinates.
*/
bool isInWorld(CIwFVec2 position, float innerRad, float outerRad);

template <class T>
class CList {
    typedef std::list<T*> TList;
    typedef typename TList::iterator TItr; 
    
    public:
        //CList();
        //~CList();
        
        TList *getRange(TItr *i, float radius);
        TList *getRange(CIwFVec2 *pos, float radius);
    
        TList backingList; 
    
        TList *operator!() {
            return &backingList;
        }
};

#endif

