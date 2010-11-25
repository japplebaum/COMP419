#include "s3e.h"
#include "IwUtil.h"
#include "IwGx.h"
#include "IwGeomMat.h"

#include "game.h"

#include "muncher.h"
#include "shooter.h"
#include "wrecker.h"
#include "spreader.h"

#define	MS_PER_FRAME (1000 / 12)

enum gesture_t { CREATE_UNIT, DRAG_WORLD };

// Structure to track touches.
struct CTouch {
    gesture_t gesture_type; // type of gesture activated by this touch
	int32 x;	        	// position
	int32 y;	        	// position
	bool active;        	// whether touch is currently active
	int32 id;	         	// touch's unique identifier
    Unit* unit;             // unit created by this touch if it's a create_unit gesture
    int32 start_y;          // initial y position of a world_drag gesture
    int32 end_y;            // end y position of a world_drag gesture
};

Game* game = NULL;
Player* localPlayer = NULL;


#define MAX_TOUCHES 10
CTouch touches[MAX_TOUCHES];

// find an active touch with the specified id, or allocate a free one from the list.
CTouch* GetTouch(int32 id) {
	CTouch* pInActive = NULL;
    
	for(uint32 i=0; i < MAX_TOUCHES; i++) {
		// if we've found a touch with the specified id, return its memory address.
		if(id == touches[i].id) return &touches[i];
		// if no touches are found with the id, pInActive will end up being set to
		// last touch in g_Touches.
		if(!touches[i].active) pInActive = &touches[i];
	}
    
	// return last inactive touch and assign its id to the specified id.
	if (pInActive) {
		pInActive->id = id;
		return pInActive;
	}
    
	// no more touches; give up.
	return NULL;
}


bool update() {    
	return true;
}

bool renderUnitCreation(CTouch* touch) {
    if(!touch->unit)
        return false;
    
    CIwFVec2 radii = game->getWorldRadius();
    CIwFVec2 *modelCoords = worldify(touch->x, touch->y, radii.x, game->getRotation());
    
    float dist_sq = SQ(modelCoords->x) + SQ(modelCoords->y);
    if(dist_sq > SQ(radii.y) || dist_sq < SQ(radii.x)){
        delete touch->unit;
        delete modelCoords;
        return false;
    }
    
    touch->unit->setPosition(*modelCoords);
	game->addUnit(touch->unit);
    
    delete modelCoords;
    
    return true;
}

void renderDragUnit(CTouch* touch){
    if(touch->unit)
        touch->unit->displayOnScreen(touch->x, touch->y);
}

bool renderDragWorld(CTouch* touch) {
    // this is VERY naive at this point, doesn't actually do angles correctly.
    game->rotate((touch->start_y - touch->end_y)/120.0f);
    return true;
}


// assign activity and position info to the touch struct associated with an event
// for a multitouch click.
void MultiTouchButtonCB(s3ePointerTouchEvent* event) {
	CTouch* touch = GetTouch(event->m_TouchID);
	if (touch) {
        touch->active = event->m_Pressed != 0;
		touch->x = event->m_x;
		touch->y = event->m_y;

        // if it's the beginning of a touch, then determine what kind of gesture it is and set initial info.
        if (touch->active) {
            if (touch->x > (int32) IwGxGetScreenWidth() - 60) {
                touch->gesture_type = CREATE_UNIT;
                
                int y = touch->y - 110; // Palate offset
                if(y < 0) return;
                
                switch (y / 60) { // 60px is size of icons
                    //case 0: touch->unit = new Thrower(NULL,  game, CIwFVec2(0,0)); break;
                    case 1: touch->unit = new Wrecker(localPlayer,  game, CIwFVec2(0,0)); break;
                    case 2: touch->unit = new Muncher(localPlayer,  game, CIwFVec2(0,0)); break;
                    case 3: touch->unit = new Shooter(localPlayer,  game, CIwFVec2(0,0)); break;
                    case 4: touch->unit = new Spreader(localPlayer, game, CIwFVec2(0,0)); break;
                    //case 5: touch->unit = new Invader(NULL,  game, CIwFVec2(0,0)); break;
                    default: break;
                }
                   
            } else {
                touch->gesture_type = DRAG_WORLD;
                touch->start_y = touch->y;
            }
        // if it's the end of a touch, check what kind of gesture it and render.
        } else {
            if (touch->gesture_type == CREATE_UNIT) {
                renderUnitCreation(touch);
                touch->unit = NULL;
            }
        }
	}
}

// assign position info to the touch struct associated with an event for
// multitouch motion.
void MultiTouchMotionCB(s3ePointerTouchMotionEvent* event) {
    if(event->m_x < 0) return;
    
	CTouch* touch = GetTouch(event->m_TouchID);
	if (touch) {
		touch->x = event->m_x;
		touch->y = event->m_y;
        
        if (touch->gesture_type == DRAG_WORLD) {
            // sent new start to the old end, and the new end to the new pos
            touch->start_y = touch->end_y;
            touch->end_y = touch->y;
        }   
	}
}

void SingleTouchButtonCB(s3ePointerEvent* event){
    s3ePointerTouchEvent *e2 = (s3ePointerTouchEvent*) malloc(sizeof(s3ePointerTouchEvent));
    e2->m_x = event->m_x;
    e2->m_y = event->m_y;
    e2->m_Pressed = event->m_Pressed;
    e2->m_TouchID = 0;
    MultiTouchButtonCB(e2);
    free(e2);   
}

void SingleTouchMotionCB(s3ePointerMotionEvent* event){
    s3ePointerTouchMotionEvent *e2 = (s3ePointerTouchMotionEvent*) malloc(sizeof(s3ePointerTouchMotionEvent));
    e2->m_x = event->m_x;
    e2->m_y = event->m_y;
    e2->m_TouchID = 0;
    MultiTouchMotionCB(e2);
    free(e2);
}

void doMain() {
    
    s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouchButtonCB, NULL);
    s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)SingleTouchButtonCB, NULL);

	s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MultiTouchMotionCB, NULL);
    s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)SingleTouchMotionCB, NULL);

    
    IwGetResManager()->LoadGroup("resource_groups/palate.group");
    CIwResGroup* palateGroup = IwGetResManager()->GetGroupNamed("Palate");
    CIwMaterial* mat = new CIwMaterial();
    mat->SetTexture((CIwTexture*)palateGroup->GetResNamed("palate", IW_GX_RESTYPE_TEXTURE));
    mat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    mat->SetAlphaMode(CIwMaterial::ALPHA_DEFAULT);
        
	static CIwSVec2 xy(260, 0);
	static CIwSVec2 wh(60, 480);
	static CIwSVec2 uv(0, 0);
	static CIwSVec2 duv(IW_GEOM_ONE, IW_GEOM_ONE);
    
    CIwColour col = {255, 0, 0, 255};
	localPlayer = new Player(col);
    game = new Game(localPlayer);

	IwGxLightingOff();

	while (1) {
        int64 start = s3eTimerGetMs();
	
		s3eDeviceYield(0);
		s3eKeyboardUpdate();
		s3ePointerUpdate();
		
		if ((s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN)
				|| (s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN)       
				|| (s3eDeviceCheckQuitRequest())) {
			
		    break;
		}
        
        IwGxSetColClear(255, 255, 255, 255);
        IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
        
        IwGxSetMaterial(mat);
        IwGxSetScreenSpaceSlot(-1);
        IwGxDrawRectScreenSpace(&xy, &wh, &uv, &duv);
        
        game->tick();
        
        for(int i = 0; i < MAX_TOUCHES; ++i)
            if(touches[i].active)
                if(touches[i].gesture_type == CREATE_UNIT) {
                    renderDragUnit(&touches[i]);
				}
                else {
                    renderDragWorld(&touches[i]);
				}
        
		
        IwGxFlush();
        
        IwGxSwapBuffers();
		
		// Attempt frame rate
		while ((s3eTimerGetMs() - start) < MS_PER_FRAME)
		{
			int32 yield = (int32) (MS_PER_FRAME - (s3eTimerGetMs() - start));
			if (yield < 0) {
				break;
			}
				
			s3eDeviceYield(yield);
		}
	}
    
    s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouchButtonCB);
    s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)MultiTouchButtonCB);

	s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, (s3eCallback)MultiTouchMotionCB);
	s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)MultiTouchMotionCB);
    
	delete game;
	delete localPlayer;
	delete mat;
}

int main() {
	IwGxInit();
	IwResManagerInit();
 
	doMain();
	
	IwResManagerTerminate();
	IwGxTerminate();
		
	return 0;
}
