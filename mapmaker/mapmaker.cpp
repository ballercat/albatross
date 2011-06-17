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
	map(NULL),
	change(NULL)
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
		FixTC(&map->texcoord[i], map->texture[i]);
	}
}

////////////////////////////////////////////////////////////
/// Fill in appropriate values depending
/// on the selection mode
////////////////////////////////////////////////////////////
bool MapMaker::Pick(change_struct *ch, size_t range)
{
	//Vertex selection
    if(ch->pick_vertex){
        glm::vec3 v;
        sf::Rect<float> pick;

		if(ch->poly){
			//Pick a vertex from a polygon
            v = ch->poly->data[0];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex 			= &ch->poly->data[0];
				ch->vertex_color 	= &map->color[ch->index].data[0];
                return true;
            }
            v = ch->poly->data[1];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex 			= &ch->poly->data[1];
                ch->vertex_color	= &map->color[ch->index].data[1];
                return true;
            }
            v = ch->poly->data[2];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex 			= &ch->poly->data[2];
                ch->vertex_color	= &map->color[ch->index].data[2];
                return true;
            }

			ch->vertex = NULL;
			ch->pick_vertex = false;

			return false;
		}

        for(size_t i = 0; i < map->poly.size();i++){

            v = map->poly[i].data[0];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex 			= &map->poly[i].data[0];
				ch->vertex_color 	= &map->color[i].data[0];
                ch->index = i;
                return true;
            }
            v = map->poly[i].data[1];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex 			= &map->poly[i].data[1];
                ch->vertex_color	= &map->color[i].data[1];
				ch->index = i;
                return true;
            }
            v = map->poly[i].data[2];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex 			= &map->poly[i].data[2];
                ch->vertex_color	= &map->color[i].data[2];
				ch->index = i;
                return true;
            }
        }
        ch->vertex = NULL;
        return false;
    }//Polygon selection
    else if(ch->pick_poly){
        bgmf_poly *p = NULL;
		size_t i = 0;
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
			}
		}

        for(i=0;i < map->header.pc;i++){
            p = &map->poly[i];
            if(PointInTriangle(mouse, p->data[0], p->data[1], p->data[2])){
                ch->poly = p;
                ch->index = i;

				//This is extra code for filling in the
				//polygon view field. Its used in the gui
				ch->pPolygon = &map->Polygon[i];

                return true;
            }
        }
		ch->pPolygon = NULL;
        ch->poly = NULL;
        return false;
    }
	else if(ch == &spawn){
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
	}

    return false;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void MapMaker::ApplyChange(change_struct *ch, bool leftdown)
{
    if(!ch)
        return;
	if(ch == &pick_vertex && leftdown){
		if(ch->vertex)
			ch->picked = true;
		else
			ch->picked = false;
		if(ch->picked && input->IsKeyDown(Input::Key::LShift)){
			glm::vec3 *v = findVertex(mouse.x,mouse.y,ch->vertex);
			if(v && vertex_snap)
				*ch->vertex = *v;
			else
				*ch->vertex = mouse;
		}
	}
    else if(ch == &move_vertex && ch->vertex && leftdown){
        glm::vec3 *v = findVertex(mouse.x,mouse.y,ch->vertex);
        if(v && vertex_snap)
            *ch->vertex = *v;
        else
            *ch->vertex = mouse;
    }
    else if(ch->pick_poly){
		ch->move = false;
        if(input->IsKeyDown(Input::Key::LShift) && leftdown){
            glm::vec3 mv = mouse - lastmouse;
            ch->poly->data[0] += mv;
            ch->poly->data[1] += mv;
            ch->poly->data[2] += mv;
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
	else if(ch == &spawn && leftdown){
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
			if(change){
				input->mState.mouse.left = !Pick(change, 10);
			}
			if(!change && input->mState.mouse.left){
				//rogue vertex
				vertex.push_back(mouse);
				if(vertex.size() == 3){
					//Create a new Polygon
					bgmf_poly poly = bgmf_poly(vertex[0],vertex[1],vertex[2]);
					bgmf_poly_tex tc = bgmfdefaulttc;
					uint32_t m = POLY_DEFAULT;
					bgmf_color color = bgmfdefaultcol;
					bgmfappend(map, m, tc, color, poly);

					FixTC(map->Polygon[map->header.pc-1].pTC, *map->Polygon[map->header.pc-1].pT);

					vertex.clear();
				}
			}

			input->mState.mouse.left = false;
		}

		ApplyChange(change, input->IsMouseButtonDown(Input::Mouse::Left));
	}

	glClearColor(0.5f,0.7f,0.8f,1.0f);
	display->beginScene();{

        if(map){

            //Draw all the polygons
            display->drawArray( &map->poly[0],
                                /*&map->texcoord[0],*/
								&map->texcoord[0],
                                &map->color[0],
                                map->header.pc*3,
                                GL_TRIANGLES,
                                gfx.Texture );

			if(drawgrid){
				glPushMatrix();
				glLoadIdentity();
				if(display->zoom.x > 1.0f)
					glScalef(display->zoom.x, display->zoom.y, display->zoom.z);
				glColor4f(1.0f,1.0f,1.0f,0.1f);
				display->drawArray( &grid[0], NULL, NULL, count, GL_LINES, NULL);
				glPopMatrix();
			}

            if(change == &pick_polygon)
            {
                if(change->remove)
                    glColor4f(1.0f,0.0f,0.0f,1.0f);
				else if(change->select)
					glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
				else if(change->move)
					glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

				change_struct swap;
				swap.pick_poly = true;
				Pick(&swap, 5);
				if(swap.pPolygon){
					display->drawArray(swap.pPolygon->pP, NULL, NULL, 3, GL_LINE_LOOP, 0);
				}

            }


            //Draw vertexes
            if(change == &move_vertex){
                //Draw every vertex green
                glPointSize(5.0f);
                glColor3f(0.0f,1.0f,0.0f);
                display->drawArray( &map->poly[0],
                                    NULL,
                                    NULL,
                                    map->header.pc*3,
                                    GL_POINTS,
                                    0 );
            }
            else {
					//Draw regular colors
				if(change == &color_vertex)
					glPointSize(8.0f);
				display->drawArray( &map->poly[0],
									NULL,
									&map->color[0],
									map->header.pc*3,
									GL_POINTS,
									0 );
            }

			// Draw Selection Rectangle
			if(change == &pick_vertex && change->vertex){
				DrawRectAroundPoint(change->vertex->x, change->vertex->y, 8.0f, delta);
			}

			if(change == &pick_polygon && change->pPolygon){
				if(input->IsKeyDown(Input::Key::LShift))
					glColor4f(0.0f,1.0f,0.0f, 1.0f);
				else
					glColor4f(0.0f, 1.0f, 1.0, 1.0f);
				glLineWidth(2.0f);
				display->drawArray(&change->poly->data[0], NULL, NULL, 3, GL_LINE_LOOP, 0);
				glLineWidth(1.0f);
			}

			if(!vertex.empty()){
				//Draw vertexes that are not yet a polygon
				glColor3f(0.0f,0.0f,1.0f);
				glPointSize(6.0f);
				display->drawArray(&vertex[0], NULL, NULL, vertex.size(), GL_POINTS, 0);
			}

			//Draw spawn points and selection rectangle
			{
				glBegin(GL_POINTS);{
					glColor3f(1.0f, 0.0f, 0.0f);
					for(int i = 0;i < map->redspawn.size();i++)
						glVertex2f(map->redspawn[i].x, map->redspawn[i].y);

					glColor3f(0.0f,0.0f,1.0f);
					for(int i = 0;i < map->bluespawn.size();i++)
						glVertex2f(map->bluespawn[i].x, map->bluespawn[i].y);
				};glEnd();

				if(change && change->spawn)
					DrawRectAroundPoint(change->spawn->x, change->spawn->y, 8.0f, delta);
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

