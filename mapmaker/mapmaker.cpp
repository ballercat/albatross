#include "GL/glew.h"
#include "mapmaker.h"
#include "fixed_pipeline.h"
#include "input.h"
#include "messages.h"
#include "assets.h"
#include "gui.h"
#include <glm/gtc/type_ptr.hpp>


#define CAMERA_STEP (0.8f * display->zoom.x)
enum {
    DELETE_VERTEX_ID = 2,
    MOVE_VERTEX_ID,
    SNAP_VERTEX_ID,
    COLOR_VERTEX_ID,
    OPEN_FILE_ID,
    SAVE_FILE_ID,
    EXIT_ID,
    MOVE_POLY_ID,
    DELETE_POLY_ID,
    COLOR_POLY_ID,
    EMPTY_POLY_ID,
    MAP_SPAWN_BLUE,
    MAP_SPAWN_RED,
    ID_COUNT
};

void OnRedSliderClick(guiWidget *wig, void *value);
void OnGreenSliderClick(guiWidget *wig, void *value);
void OnBlueSliderClick(guiWidget *wig, void *value);

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
                ch->vertex = &map->poly[i].data[0];
                ch->index = i;
                return true;
            }
            v = map->poly[i].data[1];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex = &map->poly[i].data[1];
                ch->index = i;
                return true;
            }
            v = map->poly[i].data[2];
            pick =  sf::Rect<float>(v.x-range,v.y-range,v.x+range,v.y+range);
            if(pick.Contains(mouse.x, mouse.y)){
                ch->vertex = &map->poly[i].data[2];
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

void MapMaker::ApplyChange(change_struct *ch, bool leftdown)
{
    if(!ch)
        return;
    if(ch->vertex && leftdown){
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
    else if(ch->vertex_color){
        *ch->vertex_color = glm::vec4(colorR,colorG,colorB,1.0f);
    }
}

int main(int argc,char** argv)
{
    message::Queue MessageQueue;
    Input* input = new Input();
    gfx::FixedPipeline* display;

    display = new gfx::FixedPipeline("map maker");
    input->Window = display->Window;

    LoadTextures("textures.ini",display->Texture);

    EventQueue EQ;
    //GUIsystem *gui = new GUIsystem(display->Window,input);
    GUIsystem gui = GUIsystem(display->Window, input, &EQ);
    std::vector<glm::vec3> vertex;
    glm::vec3 mouse_delta;

    MapMaker mm;
    mm.loadMap("assets/maps/test.bgmf");

    //Menu and gui controlls
    gui.AddOption(gui.File,OPEN_FILE_ID,"Open");
    gui.AddOption(gui.File,SAVE_FILE_ID,"Save");
    gui.AddOption(gui.File,EXIT_ID,"Exit");


    size_t menu_verts = gui.AddItem("Vertex");
    gui.AddOption(menu_verts,DELETE_VERTEX_ID,"Delete");
    gui.AddOption(menu_verts,MOVE_VERTEX_ID,"Move","Stop");
    gui.AddOption(menu_verts,SNAP_VERTEX_ID,"Snap");
    gui.AddOption(menu_verts,COLOR_VERTEX_ID,"Color");

    size_t menu_poly = gui.AddItem("Poly");
    gui.AddOption(menu_poly,DELETE_POLY_ID,"Delete");
    gui.AddOption(menu_poly,MOVE_POLY_ID,"Move");
    gui.AddOption(menu_poly,COLOR_POLY_ID,"Color");
    gui.AddOption(menu_poly,EMPTY_POLY_ID,"Empty");

    size_t menu_map = gui.AddItem("Map");
    gui.AddOption(menu_map,MAP_SPAWN_RED,"Spawn Red");
    gui.AddOption(menu_map,MAP_SPAWN_BLUE,"Spawn Blue");
    bool quit = false;
    //Vertex movement
    glm::vec3 vertex_color = glm::vec3(1,1,1);

    mm.colorR = 0.0f;
    mm.colorG = 0.0f;
    mm.colorB = 0.0f;
    size_t slider = gui.AddSlider(100,100,255);
    gui.SetWidgetCallback(slider, ONCLICK_CALLBACK, &OnRedSliderClick, &mm.colorR);
    slider = gui.AddSlider(70, 100, 255);
    gui.SetWidgetCallback(slider, ONCLICK_CALLBACK, &OnGreenSliderClick, &mm.colorG);
    slider = gui.AddSlider(40, 100, 255);
    gui.SetWidgetCallback(slider, ONCLICK_CALLBACK, &OnBlueSliderClick, &mm.colorB);

    MapMaker::change_struct *change = NULL;

	//Experimental
	//GLuint uniformMVP;


    while(!quit){

        input->Parse();
        if(input->IsKeyDown(Input::Key::Escape)){
            quit = true;
        }
        //GUI
        gui.Update();


        if(EQ.size()){
            Event* event = EQ.read();
            if(event->type == EVENT_GUI_CLICK){
                size_t* item = (size_t*)(event->data);
                switch(*item){
                    case EXIT_ID:
                        quit = true;
                        break;
                    case SAVE_FILE_ID:
                        printf("Saving map.....");
                        mm.saveMap("assets/maps/test.bgmf");
                        printf("done\n");
                        break;
                    case DELETE_VERTEX_ID:
                        break;
                    case MOVE_VERTEX_ID:
                        if(change == &mm.move_vertex){
                            change->vertex = NULL;
                            change = NULL;
                        }
                        else {
                            change = &mm.move_vertex;
                            change->vertex = NULL;
                        }
                        break;
                    case SNAP_VERTEX_ID:
                        mm.vertex_snap = !mm.vertex_snap;
                        if(change == &mm.move_vertex){
                            change->vertex = NULL;
                        }
                        break;
                    case COLOR_VERTEX_ID:
                        if(change == &mm.color_vertex){
                            change->vertex_color = NULL;
                            change = NULL;
                        }
                        else {
                            change = &mm.color_vertex;
                            change->vertex_color = NULL;
                        }
                        break;
                    case DELETE_POLY_ID:
                        if(change == &mm.remove_poly){
                            change->poly = NULL;
                            change = NULL;
                        } else {
                            change = &mm.remove_poly;
                            change->poly = NULL;
                            change->remove = true;
                        }
                        break;
                    case MOVE_POLY_ID:
                        if(change == &mm.move_poly){
                            change->poly = NULL;
                            change = NULL;
                        }
                        else {
                            change = &mm.move_poly;
                            change->poly = NULL;
                            change->color = false;
                        }

                        break;
                    case COLOR_POLY_ID:
                        if(change == &mm.color_poly){
                            change->poly = NULL;
                            change->index = 0;
                            change = NULL;
                        }
                        else {
                            change = &mm.color_poly;
                            change->poly = NULL;
                            change->color = true;
                        }
                        break;
                }
            }
            EQ.pop();
        }

        mm.mouse = display->stw(input->mState.mouse.pos);
		mm.mouse /= display->zoom;
		mm.mouse += display->camera;

        if(input->mState.StaticKey[Input::Key::Q]){
            change = NULL;
            gui.resetStates();
        }
        if(input->mState.mouse.pos.x > 1000)
            display->camera.x+=CAMERA_STEP;
        if(input->mState.mouse.pos.y > 580)
            display->camera.y-=CAMERA_STEP;
        if(input->mState.mouse.pos.x < 5)
            display->camera.x-=CAMERA_STEP;
        if(input->mState.mouse.pos.y < 5)
            display->camera.y+=CAMERA_STEP;

        if(!gui.block){
            //picks
            if(input->mState.mouse.left){
                if(change){
                    input->mState.mouse.left = !mm.Pick(change, 5);
                }
                if(!change && input->mState.mouse.left){
                    //rogue vertex
                    vertex.push_back(mm.mouse);
                    if(vertex.size() == 3){
                        //Create a new Polygon
                        bgmf_poly poly = bgmf_poly(vertex[0],vertex[1],vertex[2]);
                        bgmf_poly_tex tc = bgmfdefaulttc;
                        uint32_t m = POLY_SOLID | POLY_VISIBLE;
                        bgmf_color color = bgmfdefaultcol;
                        bgmfappend(mm.map, m, tc, color, poly);

                        vertex.clear();
                    }
                }

                input->mState.mouse.left = false;
            }

            mm.ApplyChange(change, input->IsMouseButtonDown(Input::Mouse::Left));
		}

        if(input->mState.mouse.wheel){
            display->zoom.x += input->mState.mouse.wheel * 0.04f;
            display->zoom.y += input->mState.mouse.wheel * 0.04f;
        }

		glClearColor(0.5f,0.7f,0.8f,1.0f);
        display->beginScene();

        if(mm.map){

            //Draw all the polygons
            display->drawArray( &mm.map->poly[0],
                                &mm.map->texcoord[0],
                                &mm.map->color[0],
                                mm.map->header.pc*3,
                                GL_TRIANGLES,
                                display->Texture[WORLD_LUMPYBARK] );

            if(change == &mm.move_poly || change == &mm.remove_poly ||
               change == &mm.color_poly)
            {
                glm::vec4 c;
                if(change->move)
                    c = glm::vec4(0.0f,1.0f,0.0f,1.0f);
                else if(change->remove)
                    c = glm::vec4(1.0f,0.0f,0.0f,1.0f);
                else if(change == &mm.color_poly)
                   c = glm::vec4(1.0f,0.0f,1.0f,1.0f);

                bgmf_color color(c,c,c);
				bgmf_poly *p;

                for(size_t k = 0; k < mm.map->header.pc;k++){
					p = &mm.map->poly[k];
                    if(PointInTriangle(mm.mouse, p->data[0], p->data[1], p->data[2])){

						glColor4f(c.x,c.y,c.z,c.w);/*
						glBegin(GL_LINE_LOOP);
							glVertex2f(p->data[0].x,p->data[0].y);
							glVertex2f(p->data[1].x,p->data[1].y);
							glVertex2f(p->data[2].x,p->data[2].y);
						glEnd();*/
						display->drawArray( &p->data[0], NULL, &color.data[0], 3, GL_LINE_LOOP, 0);
                    }
                }
            }

			
            //Draw vertexes
            if(change == &mm.move_vertex){
                //Draw every vertex green
                glPointSize(5.0f);
                glColor3f(0.0f,1.0f,0.0f);
                display->drawArray( &mm.map->poly[0],
                                    NULL,
                                    NULL,
                                    mm.map->header.pc*3,
                                    GL_POINTS,
                                    0 );
            }
            else {
                //Draw regular colors
            if(change == &mm.color_vertex)
                glPointSize(8.0f);
            display->drawArray( &mm.map->poly[0],
                                NULL,
                                &mm.map->color[0],
                                mm.map->header.pc*3,
                                GL_POINTS,
                                0 );
            }
        }



        if(!vertex.empty()){
            //Draw vertexes that are not yet a polygon
			glColor3f(0.0f,0.0f,1.0f);
			glPointSize(6.0f);
			display->drawArray(&vertex[0], NULL, NULL, 3, GL_POINTS, 0);
        }

        //Center
		glm::mat4 Identity = glm::ortho(-512.0f,512.0f,-300.0f,300.0f,-1.0f,1.0f);

		glPushMatrix();{
			glLoadIdentity();
			glPointSize(7.0f);
			glColor3f(0.0f,1.0f,0.0f);
			display->drawArray(&glm::vec3(0,0,0)[0], NULL, NULL, 1, GL_POINTS, 0);

			bgmf_poly polygon(glm::vec3(-435,-100,0),glm::vec3(-480,-55,0),glm::vec3(-390,-55,0));
			glColor3f(mm.colorR,mm.colorG,mm.colorB);
			drawTexPolygon(&polygon, display->Texture[WORLD_LUMPYBARK]);
			glPointSize(1.0f);
		}glPopMatrix();

        mm.mode_string = "Mode: Normal";
        if(change == &mm.move_poly)
            mm.mode_string = "Mode: Move Polygon";
        else if(change == &mm.color_vertex)
            mm.mode_string = "Mode: Color Vertex";
        else if(change == &mm.move_vertex)
            mm.mode_string = "Mode: Move Vertex";
        else if(change == &mm.remove_poly)
            mm.mode_string = "Mode: Delete Polygon";
        else if(change == &mm.color_poly)
            mm.mode_string = "Mode: Color Polygon";


        display->drawText(glm::vec3(0,580,0),mm.mode_string.c_str(),12);
        gui.drawMenu();
        gui.drawPopupMenu();
        gui.drawWidgets();

		//glUseProgram(program);

        display->endScene();

        mm.lastmouse = mm.mouse;

    }

    return 0;
}

void OnRedSliderClick(guiWidget *wig, void *value)
{
    guiSlider *slider = (guiSlider*)(wig);
    float *red = (float*)(value);

    *red = slider->val/255.0f;
}

void OnGreenSliderClick(guiWidget *wig, void *value)
{
    guiSlider *slider = (guiSlider*)(wig);
    float *green = (float*)(value);

    *green = slider->val/255.0f;
}

void OnBlueSliderClick(guiWidget *wig, void *value)
{
    guiSlider *slider = (guiSlider*)(wig);
    float *blue = (float*)(value);

    *blue = slider->val/255.0f;
}
