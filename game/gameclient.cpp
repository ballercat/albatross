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
		/*display = new gfx::FixedPipeline(	info.window_width,
		display		= new gfx::ShaderPipeline( info.window_width,
											info.window_height,
											"v0.0.5a(Humpback)",
											info.fullscreen	);
		*/
		display = gfx::Manager::NewContext(glm::vec2(info.window_width,info.window_height),
														"v0.0.5a(Humpback)",
														info.fullscreen );
    	GLuint *Texture = display->Texture;
		display->clearColor(0.5f, 0.7f, 0.8f, 1.0f);
		display->clearColor(0,0,0,0);
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
		cpSpace* space = mPhysics->GetWorldHandle();

		map = NULL;

		//Bind bullet collision handlers
		_initCollisionHandlers(space);
	
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
		weapon->pInfo.pos = mPlayer->pWeaponPos;
		weapon->pInfo.angle = mPlayer->pAngle;

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

			if(hit->pInfo.aniPosition == hit->pInfo.aniLength){
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
		spr->pInfo.angle.z = bullet->pAngle;
		spr->pInfo.pos = bullet->pos;
		drawSpriteInter(spr, bullet->pos, delta, bullet->pVelocity);
	}
}

////////////////////////////////////////////////////////////
/// Draw Hud
////////////////////////////////////////////////////////////
void MainClient::_drawHud(void)
{
	glm::mat4 save_matrix = gMatrix()[2];
	gMatrix()[2] = glm::mat4(1.0f);

	//Draw actual bars for values

	float length;
	Sprite *bar = &gs.hud.Object[GameSprites::HUD::CIRCLEBAR];
	glm::vec4 color = glm::vec4(1.0f, 0.3f, 0.3f, 1.0f);
	if(mPlayer->pWeapon.pInfo.Clip){
		length = float(mPlayer->pWeapon.pInfo.Clip)/mPlayer->pWeapon.pInfo.Ammo * 100.0f;
	}
	else {
		length = (mPlayer->pTime.Diff(mPlayer->pTime.Reload.Stamp))/mPlayer->pWeapon.pInfo.Reload * 100.0f;
	}

	//Ammo bar
	bar->pInfo.scale = glm::vec3(length/3, 1.0f, 1.0f);
	bar->pInfo.angle.z = 0.0f;
	bar->pInfo.color = glm::vec4(1.0f, 0.3f, 1.0f, 0.5f);
	bar->pInfo.pos = glm::vec3(-50.0f-length/3, -288.0f, 0.0f);
	bar->Draw();

	//Health bar
	mPlayer->pHealth = (mPlayer->pHealth < 0.0f) ? 0.0f : mPlayer->pHealth;
	bar->pInfo.scale.x = mPlayer->pHealth/3;
	bar->pInfo.color = glm::vec4(1.0f, 0.3f, 0.3f, 0.5f);
	bar->pInfo.pos = glm::vec3(-50.0f-mPlayer->pHealth/3, -263.0f, 0.0f);
	bar->Draw();

	gs.hud.Object[GameSprites::HUD::FRAME].pInfo.pos.y = -250.0f;
	gs.hud.Object[GameSprites::HUD::FRAME].Draw();
	glm::vec3 hscale = glm::vec3(mPlayer->pHealth/100.0f,mPlayer->pHealth/100.0f,mPlayer->pHealth/100.0f);
	gs.hud.Object[GameSprites::HUD::HEALTH].pInfo.scale = hscale;
	gs.hud.Object[GameSprites::HUD::HEALTH].Draw();

	gs.hud.Object[GameSprites::HUD::FRAME].pInfo.pos.y = -275.0f;
	gs.hud.Object[GameSprites::HUD::FRAME].Draw();
	gs.hud.Object[GameSprites::HUD::AMMO].Draw();

	//Jets, spawn circle bars

	bar->pInfo.pos = glm::vec3(80.0f, -265.0f, 0.0f);
	bar->pInfo.scale = glm::vec3(1.2f, 1.2f, 1.2f);
	bar->pInfo.angle.z = 0.0f;
	bar->pInfo.color = glm::vec4(0.2, 1.0f, 0.5f, 0.7f);
	if(mPlayer->pJetCounter < (75))
		bar->pInfo.color = glm::vec4(0.0f, 1.0f, 1.0f, 0.7f);
	if(mPlayer->pJetCounter < 40)
		bar->pInfo.color = glm::vec4(1.0f, 0.3f, 0.4f, 0.7f);

	float step = 360.0f / 100.0f; // replace 100.0f with the map default

	//We manualy draw the circle bar here to save fps
	bar->pInfo.shdProgram->Use();

	glm::mat4 barview(1.0f);
	glm::mat4 barscale = glm::translate(barview, bar->pInfo.pos);

	glUniform4fv(bar->pInfo.shdColor_loc, 1, &bar->pInfo.color.r);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bar->pInfo.txrId);

	glBindVertexArray(bar->pInfo.vaoVertex);

	//Draw the bars
	for(int i=0;i<mPlayer->pJetCounter;i++){
		bar->pInfo.angle.z+=step;
		barview = glm::rotate(barscale, bar->pInfo.angle.z, glm::vec3(0,0,1));
		barview = glm::scale(barview, bar->pInfo.scale * glm::vec3(bar->pInfo.w, bar->pInfo.h,1));
		bar->pInfo.shdProgram->ViewMat(glm::value_ptr(barview));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	}

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	//On switch weapon
	if(wepswitchtimer > 0.0f){
		Sprite *weapon;
		glm::vec3 scale;
		if(mPlayer->pWeaponID > 0){
			weapon = &gs.Weapon[mPlayer->pWeaponID-1];
			scale = weapon->pInfo.scale;
			weapon->pInfo.pos = glm::vec3(-80,100,0);
			weapon->pInfo.scale = glm::vec3(3,3,3);
			weapon->pInfo.color.a = wepswitchtimer/5.0f;
			weapon->pInfo.angle = glm::vec3(0,0,0);
			weapon->Draw();

			weapon->pInfo.color.a = 1.0f;
			weapon->pInfo.scale = scale;
		}
		weapon = &gs.Weapon[mPlayer->pWeaponID];
		scale = weapon->pInfo.scale;
		weapon->pInfo.pos = glm::vec3(0,100,0);
		weapon->pInfo.scale = glm::vec3(4,4,4);
		weapon->pInfo.color.a = wepswitchtimer;
		weapon->pInfo.angle = glm::vec3(0,0,0);
		weapon->Draw();

		weapon->pInfo.color.a = 1.0f;
		weapon->pInfo.scale = scale;
	}

	gMatrix()[2] = save_matrix;
}

