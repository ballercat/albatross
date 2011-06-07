#include "GL/glew.h"
#include "mapmaker.h"
#include "fixed_pipeline.h"
#include "input.h"
#include "messages.h"
#include "assets.h"
#include <glm/gtc/type_ptr.hpp>


#define CAMERA_STEP (0.8f * display->zoom.x)

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
////////////////////////////////////////////////////////////
static void drawTexPolygon(bgmf_poly *p,GLuint texid)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texid);
    glBegin(GL_TRIANGLES);{
        glTexCoord2f(0.0f,0.0f);
        glVertex2f(p->data[0].x,p->data[0].y);
        glTexCoord2f(0.0f,1.0f);
        glVertex2f(p->data[1].x,p->data[1].y);
        glTexCoord2f(1.0f,1.0f);
        glVertex2f(p->data[2].x,p->data[2].y);
    }glEnd();
    glDisable(GL_TEXTURE_2D);
}

////////////////////////////////////////////////////////////
/// Fill in appropriate values depending
/// on the selection mode
////////////////////////////////////////////////////////////
bool MapMaker::Pick(change_struct *ch, size_t range)
{
    //Color selection
    if(ch->pick_color){
        glm::vec3 v;
        sf::Rect<float> pick;
        for(size_t i = 0; i < map->color.size();i++){

            v = map->poly[i].data[0];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex_color = &map->color[i].data[0];
                ch->index = i;
                return true;
            }
            v = map->poly[i].data[1];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex_color = &map->color[i].data[1];
                ch->index = i;
                return true;
            }
            v = map->poly[i].data[2];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex_color = &map->color[i].data[2];
                ch->index = i;
                return true;
            }
        }

		//Nothing found
        ch->vertex_color = NULL;
        return false;
    }//Vertex selection
    else if(ch->pick_vertex){
        glm::vec3 v;
        sf::Rect<float> pick;
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
        for(size_t i = 0;i < map->poly.size();i++){
            p = &map->poly[i];
            if(PointInTriangle(mouse, p->data[0], p->data[1], p->data[2])){
                ch->poly = p;
                ch->index = i;
                return true;
            }
        }
        ch->poly = NULL;
        return false;
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
    else if(ch->poly){
        if(ch->move && leftdown){
            glm::vec3 mv = mouse - lastmouse;
            ch->poly->data[0] += mv;
            ch->poly->data[1] += mv;
            ch->poly->data[2] += mv;
        }
        else if(ch->color){
            glm::vec4 c(colorR,colorG,colorB,1.0f);
            map->color[ch->index] = bgmfsetcolorall(c);
        }
        else if(ch->remove){
            bgmfremovepoly(map,*ch->poly);
            ch->poly = NULL;
            ch = NULL;
        }
    }
    else if(ch == &color_vertex){
        *ch->vertex_color = glm::vec4(colorR,colorG,colorB,1.0f);
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
				input->mState.mouse.left = !Pick(change, 5);
			}
			if(!change && input->mState.mouse.left){
				//rogue vertex
				vertex.push_back(mouse);
				if(vertex.size() == 3){
					//Create a new Polygon
					bgmf_poly poly = bgmf_poly(vertex[0],vertex[1],vertex[2]);
					bgmf_poly_tex tc = bgmfdefaulttc;
					uint32_t m = POLY_SOLID | POLY_VISIBLE;
					bgmf_color color = bgmfdefaultcol;
					bgmfappend(map, m, tc, color, poly);

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
                                &map->texcoord[0],
                                &map->color[0],
                                map->header.pc*3,
                                GL_TRIANGLES,
                                display->Texture[WORLD_PLAIN] );

            if(change == &move_poly || change == &remove_poly ||
               change == &color_poly)
            {
                glm::vec4 c;
                if(change->move)
                    c = glm::vec4(0.0f,1.0f,0.0f,1.0f);
                else if(change->remove)
                    c = glm::vec4(1.0f,0.0f,0.0f,1.0f);
                else if(change == &color_poly)
                   c = glm::vec4(1.0f,0.0f,1.0f,1.0f);

                bgmf_color color(c,c,c);
				bgmf_poly *p;

                for(size_t k = 0; k < map->header.pc;k++){
					p = &map->poly[k];
                    if(PointInTriangle(mouse, p->data[0], p->data[1], p->data[2])){

						glColor4f(c.x,c.y,c.z,c.w);
						display->drawArray( &p->data[0], NULL, &color.data[0], 3, GL_LINE_LOOP, 0);
                    }
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
				const float sz = 8.0f;
				glm::vec3* p = change->vertex;

				delta = (delta < 360.0f) ? (delta + 0.1f) : 0.0f;

				glColor3f(0.0f, 1.0f, 0.0f);

				glPushMatrix();{
					glTranslatef(p->x, p->y, 0.0f);
					glRotatef(delta, 0.0f, 0.0f, 1.0f);

					glLineWidth(2.0f);
					glBegin(GL_LINE_LOOP);{
						glVertex2f(-sz,-sz);
						glVertex2f(-sz, sz);
						glVertex2f( sz, sz);
						glVertex2f( sz,-sz);
					}glEnd();
					glLineWidth(1.0f);

				}glPopMatrix();
			}

			if(!vertex.empty()){
				//Draw vertexes that are not yet a polygon
				glColor3f(0.0f,0.0f,1.0f);
				glPointSize(6.0f);
				display->drawArray(&vertex[0], NULL, NULL, vertex.size(), GL_POINTS, 0);
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

