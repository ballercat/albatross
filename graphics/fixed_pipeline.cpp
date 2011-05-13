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


#include "fixed_pipeline.h"
#include "timing.h"
#include <cstdio>

using namespace gfx;

////////////////////////////////////////////////////////////
/// Create render contex
////////////////////////////////////////////////////////////
FixedPipeline::FixedPipeline(	unsigned pWidth,
								unsigned pHeight,
								const char* name,
								bool fullscreen	)
{
	_timer	= &timing::GlobalTime::Instance();
	_width	= pWidth;
	_height = pHeight;

	//Window style
	unsigned long	windowstyle = (fullscreen) ? sf::Style::Fullscreen : sf::Style::Close;

	Window = new sf::RenderWindow(sf::VideoMode(_width,_height), name, windowstyle);

	Window->PreserveOpenGLStates(true);

	camera = glm::vec3(0,0,0);
	zoom = glm::vec3(1,1,1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);

	GLenum err = glewInit();
	if(GLEW_OK != err){
		exit(0x20202);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Set Viewport
	if(!fullscreen)
		glViewport(0,0,_width,_height);
	else
		glViewport(0,0,sf::VideoMode::GetDesktopMode().Width,sf::VideoMode::GetDesktopMode().Height);

	//Enviornment
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(_width/2),(_width/2),-(_height/2),(_height/2),-5.0f, 5.0f);

	//Matrices
	Model = View = glm::mat4();
	Projection = glm::ortho(-(_width/2),(_width/2),-(_height/2),(_height/2),-5.0f, 5.0f);

	//general text
	_text.SetText("NULL");
	_text.SetPosition(0,0);
	_text.SetColor(sf::Color(0,0,0));
	//Fps
	_fps.SetText("0");
	_fps.SetPosition(0,0);
	_fps.SetColor(sf::Color(0,255,0));
	_fps.SetSize(14);

	FrameTime = 0.0f;
	_skip_frames_counter = 0;
	_draw_time = _timer->GetElapsedTime();
}

////////////////////////////////////////////////////////////
void FixedPipeline::resizeWindow(int w, int h)
{
	//void
}

////////////////////////////////////////////////////////////
void FixedPipeline::setCursor(const char* fpath)
{
	sf::Image rawimage;
	if(!rawimage.LoadFromFile(fpath)){
		fprintf(stderr, "Could not load cursor file:%s\n",fpath);
		exit(0x205);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[GAME_CURSOR]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawimage.GetWidth(), rawimage.GetHeight(),
					0, GL_RGBA, GL_UNSIGNED_BYTE, rawimage.GetPixelsPtr());

	glDisable(GL_TEXTURE_2D);

    cursor.width = rawimage.GetWidth();
    cursor.height = rawimage.GetHeight();
    cursor.off = glm::vec3(0,0,0);
    cursor.scale = glm::vec3(1,1,0);
    cursor.textureid = Texture[0xFF];
	cursor.imgd = glm::vec2(10,10);
	cursor.Build();

	Window->ShowMouseCursor(false);
}

////////////////////////////////////////////////////////////
void FixedPipeline::beginScene()
{
	_draw_time = _timer->GetElapsedTime();

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(zoom.x, zoom.y, zoom.z);
	glTranslated(-camera.x, -camera.y, 0.0f);

}

////////////////////////////////////////////////////////////
void FixedPipeline::endScene()
{
	glFinish();

	if(SkipFrames){
		if(_skip_frames_counter > SkipFrames ){
			Window->Display();
			_skip_frames_counter = 0;
		}
		else {
			_skip_frames_counter++;
		}
	}
	else {
		Window->Display();
	}

	float t = _timer->GetElapsedTime();
	FrameTime = t - _draw_time;
	_draw_time = t;

	fps = int(1.0f / FrameTime);
	char a[12];
	sprintf(a,"%i", fps);
	_fps.SetText(a);
}

////////////////////////////////////////////////////////////
void
FixedPipeline::drawArray(	GLvoid *v, GLvoid *t,
							GLvoid *c, GLuint size,
							GLuint type, GLuint texid)
{
    if(t){
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_TEXTURE_2D);
        glTexCoordPointer(2, GL_FLOAT, 0, t);
        glBindTexture(GL_TEXTURE_2D, texid);
    }
    if(c){
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, c);
    }

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, v);
    glDrawArrays(type, 0, size);

    glDisableClientState(GL_VERTEX_ARRAY);

    if(t){
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    if(c)
        glDisableClientState(GL_COLOR_ARRAY);
}

////////////////////////////////////////////////////////////
void FixedPipeline::drawText(glm::vec3 pos, const char* text, size_t size)
{
    _text.SetText(text);
    _text.SetPosition(pos.x,pos.y);
    _text.SetSize(size);
    Window->Draw(_text);
}

////////////////////////////////////////////////////////////
void FixedPipeline::drawSprite(Sprite *spr)
{
	glEnable(GL_TEXTURE_2D);{
		glBindTexture(GL_TEXTURE_2D, spr->textureid);
		//Translate
		glPushMatrix();{
			//glLoadIdentity();
			glTranslated(-spr->pos.x, -spr->pos.y, 0.0f);
			glScalef(spr->scale.x, spr->scale.y, spr->scale.z);

			//Draw
			glm::vec2 *tex = spr->texdata;
			glm::vec3 *vert = spr->vertdata;

			//Color
			glColor3f(spr->color.x, spr->color.y, spr->color.z );

			//Vertexes
			glBegin(GL_TRIANGLES);{
				glTexCoord2i( tex[0].x, tex[1].y );
				glVertex3f( vert[0].x, vert[0].y, vert[0].z );

				glTexCoord2i( tex[1].x, tex[1].y );
				glVertex3f( vert[1].x, vert[1].y, vert[1].z );

				glTexCoord2i( tex[2].x, tex[2].y );
				glVertex3f( vert[2].x, vert[2].y, vert[2].z );

				glTexCoord2i( tex[3].x, tex[3].y );
				glVertex3f( vert[3].x, vert[3].y, vert[3].z );

				glTexCoord2i( tex[4].x, tex[4].y );
				glVertex3f( vert[4].x, vert[4].y, vert[4].z );

				glTexCoord2i( tex[5].x, tex[5].y );
				glVertex3f( vert[5].x, vert[5].y, vert[5].z );
			}glEnd();

		}glPopMatrix();
	}glDisable(GL_TEXTURE_2D);
}
