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

#include "GL/glew.h"
#include "mapmaker.h"
#include "fixed_pipeline.h"
#include "input.h"
#include "messages.h"
#include "assets.h"
#include <glm/gtc/type_ptr.hpp>
#include<cassert>
#include<cmath>

#define CAMERA_STEP (0.8f)

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

////////////////////////////////////////////////////////////
/// Draw a selection rectangle around a point
////////////////////////////////////////////////////////////
static void DrawRectAroundPoint(float p_X, float p_Y, float p_Size, float &p_Delta)
{
	p_Delta = (p_Delta < 360.0f) ? (p_Delta + 0.1f) : 0.0f;

	glColor3f(0.0f, 1.0f, 0.0f);

	glPushMatrix();{
		glTranslatef(p_X, p_Y, 0.0f);
		glRotatef(p_Delta, 0.0f, 0.0f, 1.0f);

		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);{
			glVertex2f(-p_Size,-p_Size);
			glVertex2f(-p_Size, p_Size);
			glVertex2f( p_Size, p_Size);
			glVertex2f( p_Size,-p_Size);
		}glEnd();
		glLineWidth(1.0f);

	}glPopMatrix();
}

////////////////////////////////////////////////////////////
/// ctor
////////////////////////////////////////////////////////////
MapMaker::MapMaker() :
	drawgrid(false),
	count(0),
	map(NULL)
{
	for(int y =0; y<76;y++){
		grid[count] = glm::vec3(-512.0f,(y-38)*8.0f,0.0f);
		grid[count+1] = glm::vec3(512.0f,(y-38)*8.0f,0.0f);
		count+=2;
	}

	for(int x = 0; x < 128; x++){
		grid[count] = glm::vec3((x-64)*8.0f, -300.0f, 0.0f);
		grid[count+1] = glm::vec3((x-64)*8.0f, 300.0f, 0.0f);
		count+=2;
	}

	_DefaultTC[0] = glm::vec2(0.0f, 0.0f);
	_DefaultTC[1] = glm::vec2(0.0f, 1.0f);
	_DefaultTC[2] = glm::vec2(1.0f, 0.0f);
	_DefaultTC[3] = glm::vec2(1.0f, 1.0f);
}

////////////////////////////////////////////////////////////
/// Initialize
////////////////////////////////////////////////////////////
void MapMaker::Init()
{
	//NOTE: if display isnt innitialized we got some issues here
	assert(NULL == input);
	assert(NULL == display);
	assert(NULL == map);

	gfx.Texture = 0;
	loadTextureData();
	loadSpriteData();

	//
	SpawnPointTexture = 0;
	glGenTextures(1, &SpawnPointTexture);
	LoadTex(SpawnPointTexture, "assets/mapmaker/spawn.png");
	SpawnPoint = new Sprite("assets/mapmaker/spawn.sprh", SpawnPointTexture);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void MapMaker::loadTextureData()
{
	//Generate polygon texture(s)
	float	tsz	= map->texpath.size();
	size_t	psz	= map->header.pc;
	float tw	= tsz*128.0f;
	float th	= 128.0f;
	sf::Image	texturedata(tw, th);

	//Create the actual OpenGL texture
	{
		sf::Image	rawImg;
		std::string fpath;

		for(size_t i=0;i<tsz;i++){
			fpath = pWorkingDir + "/assets/texture/" + map->texpath[i];
			//fpath = map->texpath[i];
			rawImg.LoadFromFile(fpath);
			texturedata.Copy(rawImg, 128*i, 0);
		}

		glDeleteTextures(1, &gfx.Texture);
		glGenTextures(1, &gfx.Texture);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gfx.Texture);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th,
					   0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata.GetPixelsPtr());

		glDisable(GL_TEXTURE_2D);
	}

	for(size_t i=0;i<psz;i++){
		// Adjust texture coordinates
		FixTC(i, map->texture[i]);

	}
}

void MapMaker::loadSpriteData()
{
	//Check for existing data, clear if true
	if(!gfx.SpriteTextures.empty()){
		glDeleteTextures(gfx.SpriteTextures.size(), &gfx.SpriteTextures[0]);

		gfx.SpriteTextures.clear();
		gfx.Scenery.clear();
	}

	int i = map->sprpath.size();

	if(i){
		gfx.SpriteTextures.resize(map->sprpath.size());
		glGenTextures(i, &gfx.SpriteTextures[0]);

		std::string fpath;

		//Load Texture data
		for(int k=0;k<i;k++){
			fpath = pWorkingDir + "/assets/scenery/" + map->sprpath[k];
			LoadTex(gfx.SpriteTextures[k], fpath.c_str());
		}

		for(int k=0;k<map->header.sprc;k++){
			i = map->sprite[k].id;
			fpath = pWorkingDir + "/assets/scenery/" + map->sprheader[i];
			Sprite scnr(fpath.c_str(), gfx.SpriteTextures[i]);
			scnr.pInfo.pos = map->sprite[k].pos;

			spriteView spr;
			spr.ptr = &map->sprite[k];
			spr.spr = scnr;

			//every sprite is static in here
			gfx.Scenery.push_back(spr);
		}
	}
}

////////////////////////////////////////////////////////////
/// Fill in appropriate values depending
/// on the selection mode
////////////////////////////////////////////////////////////
bool MapMaker::Pick(change_struct *ch, size_t range)
{
	int i = 0;
	//Vertex selection
    if(ch->pick_vertex){
        glm::vec3 v;
        sf::Rect<float> pick;

		if(ch->poly){
			//Pick a vertex from a polygon
			for(int i=0;i<ch->poly->data.size();i++){
				v = ch->poly->data[i];
				pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
				if(pick.Contains(mouse.x, mouse.y)){
					ch->vertex 			= &ch->poly->data[i];
					ch->vertex_color 	= &map->color[ch->index].data[i];
					return true;
				}
			}

			ch->vertex = NULL;
			ch->pick_vertex = false;

			return true;
		}

        for( i = 0; i < map->poly.size();i++){
			for(int k=0;k<map->poly[i].data.size();k++){
				v = map->poly[i].data[k];
				pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
				if(pick.Contains(mouse.x, mouse.y)){
					ch->vertex 			= &map->poly[i].data[k];
					ch->vertex_color 	= &map->color[i].data[k];
					ch->index = k;
					return true;
				}
			}
        }
        ch->vertex = NULL;
        return true;
    }//Polygon selection
    else if(ch->pick_poly){
        bgmf_poly *p = NULL;
		if(ch->pPolygon){
			i = ch->pPolygon->pID + 1;
			for(i; i < map->header.pc;i++){
				p = &map->poly[i];
				if(PointInTriangle(mouse, p->data[0], p->data[1], p->data[2])){
					ch->poly = p;
					ch->index = i;

					ch->pPolygon = &map->Polygon[i];
					return true;
				}
				for(int k=3;k<p->data.size();k++){
					if(PointInTriangle(mouse, p->data[k], p->data[k-1], p->data[k-2])){
						ch->poly = p;
						ch->index = i;

						ch->pPolygon = &map->Polygon[i];
						return true;
					}
				}
			}
		}

        for(i=0;i < map->header.pc;i++){
            p = &map->poly[i];
            if(PointInTriangle(mouse, p->data[0], p->data[1], p->data[2])){
                ch->poly = p;
                ch->index = i;

				ch->pPolygon = &map->Polygon[i];

                return true;
            }

			for(int k=3;k<p->data.size();k++){
				if(PointInTriangle(mouse, p->data[k], p->data[k-1], p->data[k-2])){
					ch->poly = p;
					ch->index = i;

					ch->pPolygon = &map->Polygon[i];

					return true;
				}
			}
        }
		ch->pPolygon = NULL;
        ch->poly = NULL;
        return true;
    }
	else if(ch->pick_spawn){
		sf::Rect<float> pick;

		if(ch->spawna && !map->redspawn.empty()){
			int s = map->redspawn.size();
			glm::vec2 *v;

			for(int i = 0; i < s; i++){
				v = &map->redspawn[i];
				pick = sf::Rect<float>(v->x-range, v->y-range, v->x+range, v->y+range);
				if(pick.Contains(mouse.x, mouse.y)){
					ch->spawn = v;
					return true;
				}
			}
		}

		return true;
	}
	else if(ch->pick_sprite){
		sf::Rect<float> pick;
		Sprite *sp = NULL;
		for(int i=0;i<gfx.Scenery.size();i++){
			sp = &gfx.Scenery[i].spr;

			pick = sf::Rect<float>(	sp->pInfo.pos.x+(-sp->pInfo.pivot.x*sp->pInfo.w),
									sp->pInfo.pos.y+(-sp->pInfo.pivot.y*sp->pInfo.h),
									sp->pInfo.pos.x+( sp->pInfo.pivot.x*sp->pInfo.w),
									sp->pInfo.pos.y+( sp->pInfo.pivot.y*sp->pInfo.h) );
			if(pick.Contains(mouse.x, mouse.y)){
				ch->sprite = &gfx.Scenery[i];
				return true;
			}
		}

		ch->sprite = NULL;
		return true;
	}

    return false;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void MapMaker::ApplyChange(change_struct *ch, bool leftdown)
{
    if(!ch)
        return;
	if(ch->pick_vertex && leftdown){
		if(ch->vertex)
			ch->picked = true;
		else
			ch->picked = false;
		if(ch->vertex && input->IsKeyDown(Input::Key::LShift)){
			glm::vec3 *v = findVertex(mouse.x,mouse.y,ch->vertex);
			if(v && vertex_snap)
				*ch->vertex = *v;
			else
				*ch->vertex = mouse;
		}
	}
    else if(ch->pick_poly){
		ch->move = false;
        if(input->IsKeyDown(Input::Key::LShift) && leftdown){
            glm::vec3 mv = mouse - lastmouse;
            for(int i=0;i<ch->poly->data.size();i++)
				ch->poly->data[i] += mv;

			ch->move = true;
        }
        else if(ch->remove && ch->poly){
            bgmfremovepoly(map,*ch->poly);
            ch->poly = NULL;
			ch->pPolygon = NULL;
            ch = NULL;
        }
		else if(ch->vertex){
			if(leftdown && input->IsKeyDown(Input::Key::LShift)){
				glm::vec3 *v = findVertex(mouse.x,mouse.y,ch->vertex);
				if(v && vertex_snap)
					*ch->vertex = *v;
				else
					*ch->vertex = mouse;
			}
		}
		else if(ch->select && leftdown){
			if(ch->poly)
				ch->picked	= true;
			else
				ch->picked	= false;
		}
    }
	else if(ch->pick_spawn && leftdown){
		if(ch->spawn){
			if(input->IsKeyDown(Input::Key::LShift)){
				*ch->spawn = glm::vec2(mouse.x, mouse.y);
				return;
			}

			return;
		}
		if(ch->spawna){
			map->redspawn.push_back(glm::vec2(mouse.x, mouse.y));
			map->header.rsc++;
		}else if(ch->spawnb){
			map->bluespawn.push_back(glm::vec2(mouse.x, mouse.y));
			map->header.bsc++;
		}

		ch->Clear();
		ch = NULL;
	}
	//Sprite Mode
	else if(ch->pick_sprite && leftdown){
		if(ch->sprite && input->IsKeyDown(Input::Key::LShift)){
			glm::vec3 mv = mouse - lastmouse;
			ch->sprite->ptr->pos += mv;
		}
	}
}

////////////////////////////////////////////////////////////
/// Step
////////////////////////////////////////////////////////////
void MapMaker::Step()
{
	static float delta = 0.0f;

	input->Parse();

	mouse = display->stw(input->mState.mouse.pos);
	mouse /= display->zoom;
	mouse += display->camera;

	if(input->IsKeyDown(Input::Key::LShift)){
		if(input->mState.mouse.pos.x > 1000)
			display->camera.x+=CAMERA_STEP;
		if(input->mState.mouse.pos.y > 580)
			display->camera.y-=CAMERA_STEP;
		if(input->mState.mouse.pos.x < 5)
			display->camera.x-=CAMERA_STEP;
		if(input->mState.mouse.pos.y < 5)
			display->camera.y+=CAMERA_STEP;
	}

	if(input->mState.mouse.wheel){
		display->zoom.x += input->mState.mouse.wheel * 0.04f;
		display->zoom.y += input->mState.mouse.wheel * 0.04f;
	}

	{
		//picks
		if(input->mState.mouse.left){
			input->mState.mouse.left = !Pick(&change, 10);

			if(input->mState.mouse.left){

				polynew.data.push_back(mouse);
				texcoord.data.push_back(_DefaultTC[polynew.data.size()%4]);
			}

			input->mState.mouse.left = false;
		}
		//Add created polygon
		if(input->mState.mouse.right && polynew.data.size() > 2){
			bgmfappend(map, polynew, texcoord);
			polynew.data.clear();
			texcoord.data.clear();
		}

		ApplyChange(&change, input->IsMouseButtonDown(Input::Mouse::Left));
	}

	glClearColor(0.5f,0.7f,0.8f,1.0f);
	display->beginScene();{

        if(map){

            //Draw all the polygons
			for(int i=0;i<map->header.pc;i++){
				display->drawArray( &map->poly[i].data[0],
									&map->texcoord[i].data[0],
									&map->color[i].data[0],
									map->poly[i].data.size(),
									GL_TRIANGLE_STRIP,
									gfx.Texture);
			}


			if(polynew.data.size() > 2){
				glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
				display->drawArray( &polynew.data[0], NULL, NULL, polynew.data.size(), GL_TRIANGLE_STRIP, 0);
			}

			for(int i=0;i<gfx.Scenery.size();i++){
				gfx.Scenery[i].spr.pInfo.pos = gfx.Scenery[i].ptr->pos;
				gfx.Scenery[i].spr.Draw();
			}

			if(drawgrid){
				glPushMatrix();
				glLoadIdentity();
				if(display->zoom.x > 1.0f)
					glScalef(display->zoom.x, display->zoom.y, display->zoom.z);
				glColor4f(1.0f,1.0f,1.0f,0.1f);
				display->drawArray( &grid[0], NULL, NULL, count, GL_LINES, NULL);
				glPopMatrix();
			}

            if(change.pick_poly)
            {
                if(change.remove)
                    glColor4f(1.0f,0.0f,0.0f,1.0f);
				else if(change.select)
					glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
				else if(change.move)
					glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

				change_struct swap;
				swap.pick_poly = true;
				Pick(&swap, 5);
				if(swap.pPolygon){
					display->drawArray(&swap.pPolygon->pP->data[0], NULL, NULL, swap.pPolygon->pP->data.size(), GL_LINE_STRIP, 0);
				}

            }


            //Draw vertexes
            if(change.pick_vertex && change.move){
                //Draw every vertex green
                glPointSize(3.0f);
                glColor3f(0.0f,1.0f,0.0f);
				for(int i=0;i<map->poly.size();i++){
					display->drawArray( &map->poly[i].data[0],
										NULL,
										NULL,
										map->poly[i].data.size(),
										GL_POINTS,
										0 );
				}
            }
            else {
				//Draw regular colors
				for(int i=0;i<map->poly.size();i++){
					display->drawArray( &map->poly[i].data[0],
										NULL,
										NULL,
										map->poly[i].data.size(),
										GL_POINTS,
										0 );
				}
            }

			// Draw Selection Rectangle
			if(change.vertex){
				DrawRectAroundPoint(change.vertex->x, change.vertex->y, 8.0f, delta);
			}

			if(change.pPolygon){
				if(input->IsKeyDown(Input::Key::LShift))
					glColor4f(0.0f,1.0f,0.0f, 1.0f);
				else
					glColor4f(0.0f, 1.0f, 1.0, 1.0f);
				glLineWidth(2.0f);
				display->drawArray(&change.poly->data[0], NULL, NULL, change.poly->data.size(), GL_LINE_STRIP, 0);
				glLineWidth(1.0f);
			}

			if(change.sprite){
				glColor3f(0.0f, 1.0f, 0.0f);
				glLineWidth(2.0f);
				Sprite *sp = &change.sprite->spr;
				float x = change.sprite->ptr->pos.x;
				float y = change.sprite->ptr->pos.y;
				float w = sp->pInfo.pivot.x*sp->pInfo.w;
				float h = sp->pInfo.pivot.y*sp->pInfo.h;
				glPushMatrix();{
					glTranslatef(change.sprite->ptr->pos.x, change.sprite->ptr->pos.y, 0.0f);
					glBegin(GL_LINE_LOOP);{
						glVertex2f(x - w, y - h);
						glVertex2f(x - w, y + h);
						glVertex2f(x + w, y + w);
						glVertex2f(x + w, y - h);
					}glEnd();

				}glPopMatrix();
			}

			if(!polynew.data.empty()){
				//Draw vertexes that are not yet a polygon
				glColor3f(0.0f,0.0f,1.0f);
				glPointSize(6.0f);
				display->drawArray(&polynew.data[0], NULL, NULL, polynew.data.size(), GL_POINTS, 0);

			}

			//Draw spawn points and selection rectangle
			{

				for(int i = 0;i < map->redspawn.size();i++){
					SpawnPoint->pInfo.pos = glm::vec3(map->redspawn[i].x, map->redspawn[i].y, 0.0f);
					SpawnPoint->pInfo.color = glm::vec4(5.0f, 0.0f, 0.0f, 1.0f);
					SpawnPoint->Draw();

				}

				glBegin(GL_POINTS);{
					glColor3f(0.0f,0.0f,1.0f);
					for(int i = 0;i < map->bluespawn.size();i++)
						glVertex2f(map->bluespawn[i].x, map->bluespawn[i].y);
				};glEnd();

				if(change.spawn)
					DrawRectAroundPoint(change.spawn->x, change.spawn->y, 8.0f, delta);
			}
        }

	glPushMatrix();{
		glLoadIdentity();
		glPointSize(7.0f);
		glColor3f(0.0f,1.0f,0.0f);
		display->drawArray(&glm::vec3(0,0,0)[0], NULL, NULL, 1, GL_POINTS, 0);
	}glPopMatrix();

	}display->endScene();

	lastmouse = mouse;
}

