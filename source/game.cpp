#include "game.h"
#include "unit.h"

 
Game::Game(Player* _local, Player* opponent) : localPlayer(_local), opponentPlayer(opponent), numUnits(0), rotation(0), innerRadius(72), outerRadius(288) {
	ai = new AI(this);
	IwGetResManager()->LoadGroup("resource_groups/game.group");
	sprites = IwGetResManager()->GetGroupNamed("Sprites");
	game = IwGetResManager()->GetGroupNamed("Game");
	initRenderState();
}  

Game::~Game(){
	
	for (UnitBucket::iterator itr = unitBucket.begin(); itr != unitBucket.end(); ++itr) {
		(*itr).second->clear();
		delete (*itr).second;
	}
	
	for(std::list<Unit*>::iterator itr = units.begin(); itr != units.end(); ++itr) {
		delete *itr;
	}
	
	delete ai;
 	units.clear();
	unitBuffer.clear();
    unitBucket.clear();
    
    sprites->Finalise();
    game->Finalise();
}

void Game::initRenderState() {
	//set up the camera position and view transform
    int w = IwGxGetScreenWidth();

	IwGxSetPerspMul(9);
	IwGxSetFarZNearZ(10, 8);
	view = CIwMat::g_Identity;
	view.SetTrans(CIwVec3(w/2 + innerRadius - 10, 0, -9));
	IwGxSetViewMatrix(&view);
}

std::list<Unit*>* Game::getUnits(){
	return &units;
}
 
void Game::addIcing(Icing* i) {
	//icing->
}


void Game::addUnit(Unit *u){
	
    u->setId(numUnits++);

	if(units.empty()) {
		units.push_back(u);
	} else {
		unitBuffer.push_back(u);
	}
    
    unitBuffer.sort();
	
	if(unitBucket.find(u->getTextureName()) == unitBucket.end())
		unitBucket[u->getTextureName()] = new std::set<Unit*>();
	
	(unitBucket[u->getTextureName()])->insert(u);

	//have the opponent mirror the local player
	if(&(u->getOwner()) == localPlayer) {
		Unit* newUnit = u->spawnCopy();
		newUnit->setOwner(opponentPlayer);
		newUnit->setPolarPosition(u->getR(), u->getTheta() + .75);
		addUnit(newUnit);
	}
}

void Game::tick(){

	for(std::list<Unit*>::iterator itr = units.begin(); itr !=units.end(); ++itr) {
		(*itr)->update();
	}
    
    units.merge(unitBuffer);
    
    ++timesteps;
	render();
}

void Game::render() {		    
	renderWorld();
	renderSprites();
}

void Game::renderSprites() {
	const char* curTexture = "";
	CIwMaterial* mat = new CIwMaterial();
	
	for (UnitBucket::iterator itr = unitBucket.begin(); itr != unitBucket.end(); ++itr) {
		if (strcmp((*itr).first, curTexture) != 0) {
			curTexture = (*itr).first;
			mat->SetTexture((CIwTexture*)sprites->GetResNamed(curTexture, IW_GX_RESTYPE_TEXTURE));
			mat->SetModulateMode(CIwMaterial::MODULATE_RGB);
			mat->SetAlphaMode(CIwMaterial::ALPHA_DEFAULT);
			IwGxSetMaterial(mat);
		}
		
		std::set<Unit*>* renderUnits = (*itr).second;
		
		for (std::set<Unit*>::iterator u_it = renderUnits->begin(); u_it != renderUnits->end(); ++u_it) {
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

	renderImageWorldSpace(CIwFVec2::g_Zero, 0.0, 0.6, 960, rotation);
	
	delete mat;
}

CIwFVec2 Game::getWorldRadius() {
	return CIwFVec2(innerRadius, outerRadius);
}

AI *Game::getAI(){ return ai; }

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
