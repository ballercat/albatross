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
#include <string>
#include <cstdio>
#include<glm/gtc/type_ptr.hpp>

static bool
SameSide(glm::vec3 p1,glm::vec3 p2,glm::vec3 a, glm::vec3 b)
{
    glm::vec3 cp1 = glm::cross(b-a,p1-a);
    glm::vec3 cp2 = glm::cross(b-a,p2-a);
    if(glm::dot(cp1,cp2) >= 0)
        return true;
    return false;
}

static bool
PointInTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b,glm::vec3 c)
{
    if( SameSide(p,a, b,c) && SameSide(p, b, a, c) &&
        SameSide(p, c, a, b) )
        return true;

    return false;
}

static int _HitTest(glm::vec3 &p_Loc, bgmf_poly* p_Poly, const size_t p_Sz)
{
	for(size_t i=0; i < p_Sz;i++){
		if(PointInTriangle(p_Loc, p_Poly->data[0], p_Poly->data[1], p_Poly->data[2]))
			return i;
		p_Poly++;
	}

	return -1;
}

////////////////////////////////////////////////////////////
/// Main menu loop
////////////////////////////////////////////////////////////
void MainClient::MainMenu()
{
	glm::vec3 mouse = glm::vec3(0,0,0);

	_loadMap("assets/maps/mainmenu.bgmf");
	mPlayer = new Player(display->Texture);
	mPlayer->Spawn(glm::vec3(map->redspawn[0].x, map->redspawn[0].y, 0));
	mPlayer->PickWeapon(WeaponInfo[0],0);
	mPlayer->SetJetAmmount(100);

	currentTime = mTime->GetElapsedTime();
	float oldtime = currentTime;
	float delta = 0.0f;

    while(true)
    {

		currentTime = mTime->GetElapsedTime();

		while((currentTime - oldtime) >= FRAME_TIME){
			for(size_t k=0;k<PHYSICS_STEPS;k++)
				mPhysics->Step(PHYSICS_PERSTEP);

			mPlayer->Step(mouse, currentTime);
			oldtime += FRAME_TIME;
		}
        display->beginScene();{

			gfx::Link::Instance().glsl.Default->Use();
			gfx::Link::Instance().glsl.Default->ViewMat(glm::value_ptr(display->View));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gs.map.Texture);

			glBindVertexArray(gs.map.VAO);
			glDrawElements(GL_TRIANGLES, gs.map.Index.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D, 0);
			glUseProgram(0);

			//Draw scenery
			if(map->header.sprc){
				for(int i=0;i<gs.map.Scenery.size();i++){
					//gs.map.Scenery[i].pInfo.color = glm::vec4(1,1,1,1);
					gs.map.Scenery[i].Draw();
				}
			}


			delta = currentTime - oldtime;
			mPlayer->Draw(delta);

			if(mInput->mState.mouse.left){
				int ret = _HitTest(mouse, &map->poly[0], map->poly.size());
				switch(ret){
					case 0:
					case 1:
					{
						display->setCursor(info.cursorfile.c_str());
						_loadMap(info.mapfile.c_str());
						mPlayer->Kill();
						mPlayer->Spawn(glm::vec3(map->redspawn[0].x, map->redspawn[0].y,0));
						Run();
						//Clear graphics data
						display->Window->ShowMouseCursor(true);
						_loadMap("assets/maps/mainmenu.bgmf");
						display->camera = glm::vec3(0,0,0);
						display->zoom = glm::vec3(1, 1, 1);
						mPlayer->Kill();
						mPlayer->Spawn(glm::vec3(map->redspawn[0].x, map->redspawn[0].y, 0));
						mPlayer->ResetTiming();
						mTime->Reset();
						currentTime = mTime->GetElapsedTime();
						oldtime = currentTime;
						delta = 0.0f;

						break;
					}
					case 3:
					case 4:
						return;
				};
			}
        }display->endScene();
        
        //Handle Input
        mInput->Parse();

		mouse = mInput->InputState().mouse.pos;
		mouse = display->stw(mouse);
    }

}
