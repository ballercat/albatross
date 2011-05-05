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

static sf::Clock Time;

//Timing constants
#define FPS100			(0.01f)
#define FPS60			(0.016666666666666666666666666666667f)
#define FPS45			(0.022222222222222222222222222222222f)
#define FPS40			(0.025f)
#define FPS25			(0.04f)
#define FPS20			(0.05f)
#define FRAME_TIME		FPS40
#define PHYSICS_STEPS	(10)
#define PHYSICS_DELTA	FPS40
#define PHYSICS_PERSTEP	(0.0025f)


//Inplace game loop function
void MainClient::Run(void)
{
	//Timing
	static int steps = 0;
	static float delta = 0.0f;
	float curtime = Time.GetElapsedTime();
	float oldtime = curtime;

	glm::vec3 dmp, mp, ps;

	//state
	bool quit = false;
	bool updated = false;
	Bullet* bullet;

	//Main game loop
	while(!quit){
		//Update time
		curtime = Time.GetElapsedTime();

		//game updates
		while((curtime - oldtime) > FRAME_TIME)
		{
			for(size_t k=0;k<PHYSICS_STEPS;k++)
				mPhysics->Step(PHYSICS_PERSTEP);

			for(Bullets.Begin();Bullets.Spot() != Bullets.End();Bullets.Fetch()){
				bullet = Bullets.Get();
				if(bullet->Status == BulletStatus::Dead){
					if(bullet->Type == EXPLOSIVE){
						Sprite hit = EXPLOSIONSPRITE;
						hit.pos = bullet->Hit.Pos;
						bulletHits.push_back(hit);
					}
					Bullets.Delete();
				}
				else{
					bullet->Update();
				}
			}
			Bullets.Flush();

			mPlayer->Step(dmp);

			ps = mPlayer->ps;

			oldtime += FRAME_TIME;
			updated = true;
		}

		//Handle Input
		mInput->Parse();
		mInput->HandleKeyboard();
		mInput->HandleMouse();

		mp = mInput->InputState().mouse.pos;
		dmp = display->stw(mp);

		display->zoom.x += mInput->InputState().mouse.wheel * 0.05f;
		display->zoom.y += mInput->InputState().mouse.wheel * 0.05f;

		//NOTE: camera position is INTERPOLATED position of player
		//NOT physical position
		display->camera = mPlayer->ipos;

		display->cursor.pos = dmp + mPlayer->ipos;
		//display->cursor.pos /= display->zoom;

		delta = curtime - oldtime;

		//Draw everything
		display->beginScene();{
			if(map){
				//Draw map polygons
				display->drawArray( &map->poly[0],
									&map->texcoord[0],
									&map->color[0],
									map->header.pc*3,
									GL_TRIANGLES,
									display->Texture[WORLD_PLAIN] );
			}
			Tree->Draw();

			//Draw all the bullets
			_drawBullets(delta);

			if(info.debug)
				DebugDrawPhysics();
			
			//Draw the player with interpolate
			mPlayer->Draw(delta);

			//Draw bullet Hits
			_drawHits();

			//Draw the cursor
			glPushMatrix();
			display->cursor.Draw();
			glPopMatrix();

			//Draw fps
			display->drawFPS();
		}display->endScene();

		quit = _handleMessages();
		updated = false;
	}
}

bool MainClient::_handleMessages(void)
{
	float t;
    if(mMessageQueue.Size())
    {
        while(mMessageQueue.Size()){
        int msgType = mMessageQueue.Read();
        switch(msgType)
        {
            case message::GMSG_DEFAULT:
                break;
            case message::GMSG_SPAWNOBJECT:
                break;
            case message::GMSG_QUITGAME:
                return true;
                break;
            case message::GMSG_JUMPBEGIN:
                mPlayer->jumpBegin();
                break;
			case message::GMSG_JUMPFINISH:
				mPlayer->jumpEnd();
				break;
            case message::GMSG_MOVE_RIGHT:
                mPlayer->Right();
                break;
            case message::GMSG_MOVE_LEFT:
                mPlayer->Left();
                break;
            case message::GMSG_MOVE_JETS:
                mPlayer->Jet();
                break;
            case message::GMSG_FIRE:
            {
                t = Time.GetElapsedTime();
                if(t - lastbullet < 0.10f)
                    break;
                lastbullet = t;
                glm::vec3 dest = display->stw(mInput->InputState().mouse.pos);
               	Bullet *bullet = mPlayer->Shoot(dest);	
				if(bullet != NULL)
					Bullets.Add(bullet); 

				break;
            }
			case message::GMSG_THROW:
				mPlayer->Throw();
				break;
			case message::GMSG_PICKW0:
			case message::GMSG_PICKW1:
			{
				int i = msgType - message::GMSG_PICKW0;
				std::string weapon = WeaponList[i] + "/weapon.ini";
				mPlayer->PickWeapon(weapon, i);
				break;
			}

		}
        mMessageQueue.Pop();
        }
    }

	return false;
}
