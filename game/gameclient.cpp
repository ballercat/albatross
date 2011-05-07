/*
    ********************************************
    *   Albatross - A 2D multiplayer shooter   *
    ********************************************
    Copyright (C) 2011  Arthur Buldauskas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */


#include "gameclient.h"
#include "assets.h"
#include "collision.h"
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

static sf::Clock g_timer;

////////////////////////////////////////////////////////////
///Constructor
////////////////////////////////////////////////////////////
MainClient::MainClient(void) :
    mPhysics(&physics::Simulator::Instance())
{
	try
	{
		//Parse the settings file
		_loadGameSettings();


    	//Create the display
		display = new gfx::FixedPipeline(	info.window_width,
											info.window_height,
											"v0.0.4a(Finback)",
											info.fullscreen	);

    	GLuint *Texture = display->Texture;
		display->clearColor(0.5f, 0.7f, 0.8f, 1.0f);

   		mInput = new InputHandlerMixin<Input>(&mMessageQueue);
    	mInput->Window = display->Window;

    	mIP = network::GetLocalIP();

    	//Quick and dirty texture loading
    	LoadTextures("textures.ini", Texture);
		_populateSprites();

		//Display settings
		display->setCursor(info.cursorfile.c_str());
		display->Window->SetFramerateLimit(info.fpslimit);

		//Physics
    	cpShape *shape;
		cpSpace* space = mPhysics->GetWorldHandle();
    	cpBody *staticBody = &space->staticBody;
		
		//Load the map, plug in polygon values into the game
		map = bgmfopen(info.mapfile.c_str());
    	glm::vec3 v0,v1,v2;
    	for(bgmf_poly *p = &map->poly[0];p!=&map->poly[map->header.pc];p++)
    	{
        	v0 = p->data[0];
        	v1 = p->data[1];
        	v2 = p->data[2];

        	//Add map polygon to physics world
        	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(v0.x,v0.y),cpv(v1.x,v1.y),0.0f));
        	shape->e = 1.0f; shape->u = 1.0f;
        	shape->collision_type = MAPPOLYGON;
        	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(v1.x,v1.y),cpv(v2.x,v2.y),0.0f));
        	shape->e = 1.0f; shape->u = 1.0f;
       		shape->collision_type = MAPPOLYGON;
        	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(v2.x,v2.y),cpv(v0.x,v0.y),0.0f));
        	shape->e = 1.0f; shape->u = 1.0f;
        	shape->collision_type = MAPPOLYGON;
    	}

		//Bind bullet collision handlers
		_initCollisionHandlers(space);
		
    	//Create a player object
    	mPlayer = new Player(glm::vec3(500,200,0),Texture);
		mPlayer->PickWeapon(WeaponInfo[0],0);

		//Timing(NOTE: improve/move this)
    	lastupdate = g_timer.GetElapsedTime();
    	lastbullet = lastupdate;
		objectupdate = lastupdate;

		//Sprite hack
		Tree = new Sprite("assets/sprite/tree/tree1.sprh",Texture[TREE_PLAIN]);
		Tree->pos = glm::vec3(200,300,0);

		Explosion = new Sprite("assets/explosions/default.sprh", Texture[EXPL]);
	}

	//Fatal error handling
	catch(const char* p_Err){
		std::cerr << "Error: " << p_Err << std::endl; 
		exit(1);
	}
	catch(...){
		std::cerr << "Unknow Error Occured while initializing\n";
		std::cerr << "Exiting...\n";
		exit(1);
	}

	mInput->Parse();
	mInput->Parse();
	mInput->HandleKeyboard();
	mInput->HandleMouse();
	
	mMessageQueue.Clear();
} //MainClient::MainClient

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
MainClient::~MainClient(void)
{
    if(display)
    {
        delete display;
        display = NULL;
    }
} //MainClient::~MainClient(void)

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
int MainClient::Connect(std::string server_ip)
{
    if(server_ip == network::GetLocalIP())
    {
        return 1;
    }

    return 0;
}

void MainClient::_drawWeapons()
{
	if(mPlayer->pHasWeapon){
		Sprite* weapon = &gs.Weapon[mPlayer->pWeaponID];
		weapon->pos = mPlayer->pWeaponPos;
		weapon->angle = mPlayer->pAngle;

		weapon->Draw();
	}
}

////////////////////////////////////////////////////////////
/// Draw Hits
////////////////////////////////////////////////////////////
void MainClient::_drawHits(void)
{
	if(!bulletHits.empty()){
		//Draw hits
		std::list<Sprite>::iterator iter;
		Sprite *hit;
		std::vector< std::list<Sprite>::iterator> eraser;

		for(iter = bulletHits.begin(); iter != bulletHits.end(); ++iter){
			hit = &(*iter);

			if(hit->mPosition == hit->mLength){
				//No looping so remove the hit spot
				eraser.push_back(iter);
			}
			else {
				hit->Step();
				hit->Draw();
			}


		}

		//Remove 'finished' animations
		if(!eraser.empty()){
			for(size_t k = 0; k < eraser.size();k++){
				bulletHits.erase(eraser[k]);
			}
		}
	}
}

////////////////////////////////////////////////////////////
/// Draw bullets
////////////////////////////////////////////////////////////
void MainClient::_drawBullets(float& delta)
{
	static Bullet* bullet = NULL;
	static Sprite* spr = NULL;

	for(Bullets.Begin();Bullets.Spot() != Bullets.End();Bullets.Fetch()){
		bullet = Bullets.Get();
		spr = &gs.Bullet[bullet->pID];
		spr->angle.z = bullet->pAngle;
		spr->pos = bullet->pos;
		drawSpriteInter(spr, bullet->pos, delta, bullet->pVelocity);
	}
}

////////////////////////////////////////////////////////////
/// Collision handlers
////////////////////////////////////////////////////////////
void MainClient::_initCollisionHandlers(cpSpace *space)
{	
	mPhysics->AddCollision(MAPPOLYGON, BULLET, this, collision::Begin::BulletWorld);
	mPhysics->AddCollision(EXPLOSIVE, MAPPOLYGON, this, collision::Begin::ExplosiveWorld);
	mPhysics->AddCollision(EXPLOSION, PLAYER, this, collision::Begin::ExplosionObject);
	mPhysics->AddCollision(EXPLOSIVE, PLAYER, this, collision::Begin::ExplosiveObject);

	mPhysics->AddCollision(WEAPON, PLAYER, this, w2p_beginCollision);	
}

cpBool MainClient::w2p_beginCollision(cpArbiter *arb, cpSpace *space, void *p_Client)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);
	object::Weapon *weapon = (object::Weapon*)(a->data);
	MainClient *client = (MainClient*)(p_Client);

	client->mPlayer->Pickup(*weapon);

	return cpFalse;
}
