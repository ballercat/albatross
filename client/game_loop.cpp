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
#include <cstdio>

static glm::vec3 dmp;

//Timing constants
#define FPS100			(0.01f)
#define FPS60			(0.016666666666666666666666666666667f)
#define FPS50			(0.02f)
#define FPS45			(0.022222222222222222222222222222222f)
#define FPS40			(0.025f)
#define FPS25			(0.04f)
#define FPS20			(0.05f)
#define FRAME_TIME		FPS40
#define PHYSICS_STEPS	(40)
#define PHYSICS_DELTA	FPS40
#define PHYSICS_PERSTEP	(0.000625f)
#define INPUT_LATENCY	(FPS50)

//#define PHYSICS_PERSTEP	(PHYSICS_DELTA/PHYSICS_STEPS)

//Inplace game loop function
void MainClient::Run(const char *p_DemoFile)
{
	if(info.demoPlay)
	{
		Demo.ReadFile(info.demoFile.c_str());
		printf("%i\n", Demo[Demo.PC].Tick);
	}
	//Timing
	mTime->Reset();
	mTicks = 0;
	currentTime = mTime->GetElapsedTime();
	float oldtime = currentTime;
	float inputlag = currentTime;
	float delta = 0.0f;

	printf("Current Time: %f\n", currentTime);

	mPlayer->ResetTiming();

	GameDemo::Command *cmd = NULL;

	glm::vec3 mp, ps;

	//state
	bool quit = false;
	Bullet* bullet;

	//Main game loop
	while(!quit){
		//Update time
		currentTime = mTime->GetElapsedTime();

		//Input updates
		while((currentTime - inputlag) >= INPUT_LATENCY){
			mp = mInput->InputState().mouse.pos;

			if(info.demoPlay){


				while(Demo.PC < Demo.Data.size() && Demo[Demo.PC].Tick == mTicks)
				{
					cmd = &Demo[Demo.PC];
					mMessageQueue.Push(cmd->ID);

					Demo.PC++;
				}

				if(Demo.PC >= Demo.Data.size())
				{
					mMessageQueue.Push(message::GMSG_QUITGAME);
				}

				if(cmd){
					dmp = cmd->Cursor;
				}
				else {
					mInput->Parse();
					dmp = display->stw(mp);
				}
			}
			else{
				//Handle Input
				mInput->Parse();
				mInput->HandleKeyboard();
				mInput->HandleMouse();

				dmp = display->stw(mp);
			}

			quit = _handleMessages();

			inputlag += INPUT_LATENCY;
		}

		//Physics updates and world updates
		while((currentTime - oldtime) >= FRAME_TIME)
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

			mPlayer->Step(dmp, currentTime);

			ps = mPlayer->pPos;

			oldtime += FRAME_TIME;
			mTicks++;
		}

		display->zoom.x += mInput->InputState().mouse.wheel * 0.005f;
		display->zoom.y += mInput->InputState().mouse.wheel * 0.005f;

		//NOTE: camera position is INTERPOLATED position of player
		//NOT physical position
		display->camera = mPlayer->pIPos;

		display->cursor.pos = dmp + mPlayer->pIPos;
		//display->cursor.pos /= display->zoom;

		delta = currentTime - oldtime;

		//Draw everything
		display->beginScene();{
			Tree->Draw();

			//Draw all the bullets
			_drawBullets(delta);

			#ifdef NDEBUG
			if(info.debug)
				DebugDrawPhysics();
			#endif

			//Draw the player with interpolate
			mPlayer->Draw(delta);

			_drawWeapons();

			if(map){
				//Draw map polygons
				display->drawArray( &map->poly[0],
									&map->texcoord[0],
									&map->color[0],
									map->header.pc*3,
									GL_TRIANGLES,
									gs.map.Texture );
			}

			//Draw bullet Hits
			_drawHits();

			//Draw the cursor
			display->cursor.Draw();

			//Draw fps
			display->drawFPS();
		}display->endScene();

	}

	if(info.demoRecord)
		Demo.Save("demo.bgdf");
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
				mMessageQueue.Clear();
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
                //t = mTime->GetElapsedTime();
                //if(t - lastbullet < 0.10f)
                    //break;
                //lastbullet = t;
                glm::vec3 dest = dmp;
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
				mPlayer->PickWeapon(WeaponInfo[i], i);
				break;
			}

		}

		if(info.demoRecord){
			Demo.PushCommand(msgType, dmp, mTicks);
		}
        mMessageQueue.Pop();
        }
    }

	return false;
}
