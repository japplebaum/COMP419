#include "game.h"
#include "unit.h"
#include "spreader.h"

Game::Game() : numUnits(0), rotation(0), innerRadius(112*.85), outerRadius(358*.85), timesteps(0) {

    IwGetResManager()->LoadGroup("resource_groups/game.group");
	sprites = IwGetResManager()->GetGroupNamed("Sprites");
	game = IwGetResManager()->GetGroupNamed("Game");
	initRenderState();
                    
	CIwResList* resources = sprites->GetListHashed(IwHashString("CIwTexture"));
	for(CIwManaged** itr = resources->m_Resources.GetBegin(); itr != resources->m_Resources.GetEnd(); ++itr) {
		unitBucket[(*itr)->m_Hash] = new std::set<Unit*>();
	}
}  

Game::~Game(){
	
	for (UnitBucket::iterator itr = unitBucket.begin(); itr != unitBucket.end(); ++itr) {
		(*itr).second->clear();
		delete (*itr).second;
	}
	
	for(std::list<Unit*>::iterator itr = (!units)->begin(); itr != (!units)->end(); ++itr) {
		delete *itr;
	}
	
	
 	(!units)->clear();
	unitBuffer.clear();
    unitBucket.clear();
    
    sprites->Finalise();
    game->Finalise();
}

void Game::setLocalPlayer(Player* local) {
    localPlayer = local;
}

void Game::setOpponentPlayer(Player* opponent) {
    opponentPlayer = opponent;
}

void Game::setLeaderPositions(CIwFVec2 &local, CIwFVec2 &opponent) {
    localLeaderPos = local;
    opponentLeaderPos = opponent;
}

CIwFVec2 Game::getLocalLeaderPos() {
    return localLeaderPos;
}

CIwFVec2 Game::getOpponentLeaderPos() {
    return opponentLeaderPos;
}

UnitBucket* Game::getUnitBucket() {
	return &unitBucket;
}

void Game::initRenderState() {
	//set up the camera position and view transform
    int w = IwGxGetScreenWidth();

	IwGxSetPerspMul(9);
	IwGxSetFarZNearZ(12, 8);
	view = CIwMat::g_Identity;
	view.SetTrans(CIwVec3(innerRadius+w/2-30, 0, -9));
	IwGxSetViewMatrix(&view);
}

CList<Unit>* Game::getUnits(){
	return &units;
}

void Game::addUnit(Unit *u, bool pay){
    if(pay){
        CIwFVec2 pos = u->getPosition();
        int cost = unitCosts[u->getType()];
        std::list<Spreader *> spreaders;
        
        
        for(std::list<Unit*>::iterator itr = (!units)->begin(); itr != (!units)->end(); ++itr){
            Player *p = (*itr)->owner,
				   *q = u->owner;
            if(&p == &q && (*itr)->getType() == SPREADER){
                cost -= ((Spreader *)(*itr))->nearbyIceCount(pos);
                spreaders.push_back((Spreader *)(*itr));
            }
        }
        
        if(cost <= 0){
            cost = unitCosts[u->getType()];
            for(std::list<Spreader*>::iterator itr = spreaders.begin(); itr != spreaders.end(); ++itr){
                cost -= (*itr)->useNearbyIce(pos, cost);
            }
        } else {
            delete u;
            return;
        }
    }

    u->uid = numUnits++;
    unitBuffer.push_back(u);

	//mirror player
	if (u->getType() != WRECKER && u->getType() != PROJECTILE) {
		Unit* mirror = u->spawnCopy();
		if (mirror != NULL) {
			s3eDebugOutputString("mirror unit");
			mirror->setOwner(opponentPlayer);
			mirror->setPolarPosition(u->getR(), PI - u->getTheta());
			unitBuffer.push_back(mirror);
		}
	}
				
}

void Game::tick(){

	for(std::list<Unit*>::iterator itr = (!units)->begin(); itr != (!units)->end(); ++itr) {
        (*itr)->update();
    }
    
    for(std::list<Unit*>::iterator itr = (!units)->begin(); itr != (!units)->end(); ++itr) {
        if((*itr)->hp <= 0){
			// Remove the unit from all data structures
			unitBucket[(*itr)->getTextureName()]->erase(unitBucket[(*itr)->getTextureName()]->find((*itr)));
			delete *itr;
            itr = (!units)->erase(itr);
        }
    }  
    
	for (std::list<Unit*>::iterator itr = unitBuffer.begin(); itr != unitBuffer.end(); ++itr) {
		Unit* u = *itr;
		if(unitBucket.find(u->getTextureName()) == unitBucket.end())
			unitBucket[u->getTextureName()] = new std::set<Unit*>();
		(unitBucket[u->getTextureName()])->insert(u);
	}
	
    unitBuffer.sort();
    (!units)->merge(unitBuffer);
    
    ++timesteps;
}

void Game::render() {		    
	renderWorld();
   	renderSprites();
}

void Game::renderSprites() {
	unsigned int curTexture = IwHashString("");
	CIwMaterial* mat = new CIwMaterial();
	
	for (UnitBucket::iterator itr = unitBucket.begin(); itr != unitBucket.end(); ++itr) {
        
		if (curTexture != (*itr).first != 0) {
			curTexture = (*itr).first;
			mat->SetTexture((CIwTexture*)sprites->GetResHashed(curTexture, IW_GX_RESTYPE_TEXTURE));
			mat->SetModulateMode(CIwMaterial::MODULATE_RGB);
			mat->SetAlphaMode(CIwMaterial::ALPHA_DEFAULT);
			IwGxSetMaterial(mat);
		}
		
		std::set<Unit*>* renderUnits = (*itr).second;
		
		for (std::set<Unit*>::iterator u_it = renderUnits->begin(); u_it != renderUnits->end(); ++u_it) {
            if((*u_it) != NULL && (*u_it)->hp <= 0){
                renderUnits->erase(u_it);
            }
            
			(*u_it)->display();
		}
	}
	
	delete mat;
}

void Game::renderWorld() {

	CIwMaterial* mat = new CIwMaterial();
	mat->SetTexture((CIwTexture*)game->GetResNamed("paper-world", IW_GX_RESTYPE_TEXTURE));
	mat->SetModulateMode(CIwMaterial::MODULATE_NONE);
	mat->SetAlphaMode(CIwMaterial::ALPHA_DEFAULT);
	IwGxSetMaterial(mat);

	renderImageWorldSpace(CIwFVec2::g_Zero, 0.0, 0.85, 960, rotation, 0, 1, 0.0f);
	
	delete mat;
}

CIwFVec2 Game::getWorldRadius() {
	return CIwFVec2(innerRadius, outerRadius);
}

CIwMat* Game::getViewMatrix(){
    return &view;
}

float Game::getRotation(){
    return rotation;
}

float Game::rotate(float rot) {
    return rotation += rot;
}

CIwResGroup* Game::getSprites(){
    return sprites;
}

Player *Game::getLocalPlayer(){
    return localPlayer;
}

Player *Game::getOpponentPlayer(){
    return opponentPlayer;
}
