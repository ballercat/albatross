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


#include"gameclient.h"
#include<cstdio>
#include<glm/gtc/type_ptr.hpp>

static glm::vec3 dmp;

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
	wepswitchtimer = 0.0f;

	printf("Current Time: %f\n", currentTime);

	mPlayer->ResetTiming();
	
	GameDemo::Command *cmd = NULL;
	mSpawnTimer = 0;

	glm::vec3 mp, ps;

	float		lasthp = 100.0f;
	float		dtimer = 0.0f;
	sf::String	damage;
	damage.SetColor(sf::Color(255,0,0));
	damage.SetSize(12);
	damage.SetPosition(505.0f, 300.0f);

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
						//Sprite hit = EXPLOSIONSPRITE;
						Sprite hit = gs.BulletHit[1];
						hit.pInfo.pos = bullet->Hit.Pos;
						bulletHits.push_back(hit);
					}
					Bullets.Delete();
				}
				else{
					bullet->Update();
				}
			}
			Bullets.Flush();

			if(mPlayer->pHealth > 0.0f){
				mPlayer->Step(dmp, currentTime);
				if(mPlayer->pJetCounter < 100){
					if(mPlayer->pTime.Current - mPlayer->pTime.Jet.Stamp > 1.0f)
						mPlayer->pJetCounter++;
				}
				if(lasthp != mPlayer->pHealth){
					char d[12];
					int diff = mPlayer->pHealth - lasthp;
					sprintf(d,"%d", diff);
					lasthp = mPlayer->pHealth;
					damage.SetText(d);
					dtimer = 15.0f;

					if(diff > 0)
						damage.SetColor(sf::Color(0,255,0));
					else
						damage.SetColor(sf::Color(255,0,0));
				}
			}
			else {
				if(mPlayer->pSpawned){
					char d[12];
					int diff = lasthp;
					sprintf(d,"%d", diff);
					lasthp = 0;
					damage.SetText(d);
					dtimer = 15.0f;

					if(diff > 0)
						damage.SetColor(sf::Color(0,255,0));
					else
						damage.SetColor(sf::Color(255,0,0));
					mPlayer->pHealth = 0.0f;
					mPlayer->Kill();
				}
				if(!mSpawnTimer){
					mSpawnTimer = SPAWN_TIME;
				}
				else {
					mSpawnTimer -= (currentTime - oldtime);
					if(mSpawnTimer <= 0){
						mPlayer->Spawn(glm::vec3(map->redspawn[0].x, map->redspawn[0].y,0));
						mSpawnTimer = 0.0f;
					}
				}
			}
			ps = mPlayer->pPos;

			oldtime += FRAME_TIME;
			mTicks++;
		}

		display->zoom.x += mInput->InputState().mouse.wheel * 0.005f;
		display->zoom.y += mInput->InputState().mouse.wheel * 0.005f;

		//NOTE: camera position is INTERPOLATED position of player
		//NOT physical position
		display->camera = mPlayer->pIPos;

		display->cursor.pInfo.pos = dmp + mPlayer->pIPos;
		//display->cursor.pos /= display->zoom;

		delta = currentTime - oldtime;

		//Draw everything
		display->beginScene();{
			if(map){
				gfx::Link::Instance().glsl.Default->Use();
				gfx::Link::Instance().glsl.Default->ViewMat(glm::value_ptr(display->View));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gs.map.Texture);

				glBindVertexArray(gs.map.VAO);
				//glDrawElements(GL_TRIANGLES, gs.map.Index.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
				glDrawRangeElements(	GL_TRIANGLES,
										0,
										map->hpc,
										map->hpc,
										GL_UNSIGNED_SHORT,
										BUFFER_OFFSET(0) );
				glBindVertexArray(0);

				glBindTexture(GL_TEXTURE_2D, 0);
				glUseProgram(0);
			}

			//Draw all the bullets
			_drawBullets(delta);

			#ifdef NDEBUG
			if(info.debug)
				DebugDrawPhysics();
			#endif

			//Draw the player with interpolate
			mPlayer->Draw(delta);
			if(dtimer > 0){
				damage.SetY(280+dtimer);
				display->Window->Draw(damage);
				dtimer -= delta;
			}

			_drawWeapons();

			//Draw scenery
			if(map->header.sprc){
				for(unsigned i=0;i<gs.map.Scenery.size();i++){
					gs.map.Scenery[i].Draw();
				}
			}

			//Draw bullet Hits
			_drawHits();

			//Draw Hollow Polygons
			{
				gfx::Link::Instance().glsl.Default->Use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gs.map.Texture);

				glBindVertexArray(gs.map.VAO);
				glDrawRangeElements(	GL_TRIANGLES,
										map->hpc,
										gs.map.Index.size(),
										gs.map.Index.size() - map->hpc,
										GL_UNSIGNED_SHORT,
										BUFFER_OFFSET(sizeof(GLushort)*map->hpc) );
				glBindVertexArray(0);

				glBindTexture(GL_TEXTURE_2D, 0);
				glUseProgram(0);
			}

			//Draw the cursor
			display->cursor.Draw();

			wepswitchtimer -= delta/20.0f;
			_drawHud();

			//Draw fps
			display->drawFPS();
		}display->endScene();

	}

	if(info.demoRecord)
		Demo.Save("demo.bgdf");
}

////////////////////////////////////////////////////////////
/// Handle Messages
////////////////////////////////////////////////////////////
bool MainClient::_handleMessages(void)
{
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
                glm::vec3 dest = dmp;
               	Bullet *bullet = mPlayer->Shoot(dest);	
				if(NULL != bullet)
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
				wepswitchtimer = 1.0f;
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
