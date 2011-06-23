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
											"v0.0.5a(Humpback)",
											info.fullscreen	);

    	GLuint *Texture = display->Texture;
		display->clearColor(0.5f, 0.7f, 0.8f, 1.0f);

   		mInput = new InputHandlerMixin<Input>(&mMessageQueue);
    	mInput->Window = display->Window;

    	mIP = network::GetLocalIP();

    	//Quick and dirty texture loading
    	LoadTextures("textures.ini", Texture);
		_populateSprites();
		_loadHudSprites();

		//Display settings
		display->Window->SetFramerateLimit(info.fpslimit);

		//Physics
    	cpShape *shape;
		cpSpace* space = mPhysics->GetWorldHandle();
    	cpBody *staticBody = &space->staticBody;


		map = NULL;

		//Bind bullet collision handlers
		_initCollisionHandlers(space);
		

		//Timing(NOTE: improve/move this)
    	lastupdate = g_timer.GetElapsedTime();
		objectupdate = lastupdate;

		//aExplosion = new Sprite("assets/explosions/default.sprh", Texture[EXPL]);
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
	currentTime = 0.0f;

	mTime	= &timing::GlobalTime::Instance();
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
	if(mPlayer->pHasWeapon && mPlayer->pHealth > 0.0f){
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
/// Draw Hud
////////////////////////////////////////////////////////////
void MainClient::_drawHud(void)
{
	glPushMatrix();{
		glLoadIdentity();
		//Draw actual bars for values
		{
			float length;
			if(0.0f == mPlayer->pTime.Reload.Stamp){
				length = float(mPlayer->pWeapon.pInfo.Clip)/mPlayer->pWeapon.pInfo.Ammo * 100.0f;
			}
			else {
				length = (mPlayer->pTime.Diff(mPlayer->pTime.Reload.Stamp))/mPlayer->pWeapon.pInfo.Reload * 100.0f;
			}
			glBegin(GL_TRIANGLES);{
				//Ammo
				glVertex2f(-50.0f, -270.0f);
				glVertex2f(-50.0f + length, -282.0f);
				glVertex2f(-50.0f + length, -270.0f);

				glVertex2f(-50.0f, -282.0f);
				glVertex2f(-50.0f, -270.0f);
				glVertex2f(-50.0f + length, -282.0f);

				//Health
				mPlayer->pHealth = (mPlayer->pHealth < 0.0f) ? 0.0f : mPlayer->pHealth;
				glColor3f(1.0f, 0.3f, 0.3f);
				glVertex2f(-50.0f, -243.0f);
				glVertex2f(-50.0f + mPlayer->pHealth, -259.0f);
				glVertex2f(-50.0f + mPlayer->pHealth, -243.0f);

				glVertex2f(-50.0f, -259.0f);
				glVertex2f(-50.0f, -243.0f);
				glVertex2f(-50.0f + mPlayer->pHealth, -259.0f);
			}glEnd();

		}
		gs.hud.Object[GameSprites::HUD::FRAME].pos.y = -250.0f;
		gs.hud.Object[GameSprites::HUD::FRAME].Draw();
		gs.hud.Object[GameSprites::HUD::HEALTH].Draw();

		gs.hud.Object[GameSprites::HUD::FRAME].pos.y = -275.0f;
		gs.hud.Object[GameSprites::HUD::FRAME].Draw();
		gs.hud.Object[GameSprites::HUD::AMMO].Draw();

		//Jets, spawn circle bars
		{
			Sprite *bar = &gs.hud.Object[GameSprites::HUD::CIRCLEBAR];
			bar->pos = glm::vec3(80.0f, -265.0f, 0.0f);
			bar->scale = glm::vec3(1.2f, 1.2f, 1.2f);
			bar->angle.z = 0.0f;
			bar->color = glm::vec4(0.2, 1.0f, 0.5f, 0.7f);
			if(mPlayer->pJetCounter < (75))
				bar->color = glm::vec4(0.0f, 1.0f, 1.0f, 0.7f);
			if(mPlayer->pJetCounter < 40)
				bar->color = glm::vec4(1.0f, 0.3f, 0.4f, 0.7f);

			float step = 360.0f / 100.0f; // replace 100.0f with the map default

			for(int i=0;i<mPlayer->pJetCounter;i++){
				bar->angle.z+=step;
				bar->Draw();
			}
		}

		//On switch weapon
		if(wepswitchtimer > 0.0f){
			Sprite *weapon;
			glm::vec3 scale;
			if(mPlayer->pWeaponID > 0){
				weapon = &gs.Weapon[mPlayer->pWeaponID-1];
				scale = weapon->scale;
				weapon->pos = glm::vec3(-80,100,0);
				weapon->scale = glm::vec3(3,3,3);
				weapon->color.a = wepswitchtimer/5.0f;
				weapon->angle = glm::vec3(0,0,0);
				weapon->Draw();

				weapon->color.a = 1.0f;
				weapon->scale = scale;
			}
			weapon = &gs.Weapon[mPlayer->pWeaponID];
			scale = weapon->scale;
			weapon->pos = glm::vec3(0,100,0);
			weapon->scale = glm::vec3(4,4,4);
			weapon->color.a = wepswitchtimer;
			weapon->angle = glm::vec3(0,0,0);
			weapon->Draw();

			weapon->color.a = 1.0f;
			weapon->scale = scale;
		}

	}glPopMatrix();
}

