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


#ifndef COREPROFILE_HEADER_GUARD
#define COREPROFILE_HEADER_GUARD

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include "sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gfx
{
	////////////////////////////////////////////////////////////
	///Display API
	////////////////////////////////////////////////////////////
	class Core
	{
	public:
        ///Constructor
		virtual ~Core(){
			delete this->Window;
		}
    
    public:
		void setCursor(const char *fpath)
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

			/*cursor.width = rawimage.GetWidth();
			cursor.height = rawimage.GetHeight();
			cursor.off = glm::vec3(0,0,0);
			cursor.scale = glm::vec3(1,1,0);
			cursor.textureid = Texture[0xFF];
			cursor.imgd = glm::vec2(10,10);
			cursor.Build();*/
			cursor = Sprite(Texture[0xFF]);

			Window->ShowMouseCursor(false);
		}

		virtual void beginScene() = 0;
		virtual void endScene() = 0;

	public:
		///Rendering helper functions
		virtual void drawArray(GLvoid *v, GLvoid *t, GLvoid *c, GLuint size, GLuint type, GLuint texid) = 0;
		virtual void drawText(glm::vec3 pos, const char *text,size_t size) = 0;

	public:
		///General Helper functions
		inline void drawFPS(void){
			this->Window->Draw(this->_fps);
		}

		inline glm::vec3 stw(const glm::vec3& v){
			return glm::unProject( glm::vec3(v.x, _height-v.y, 0.0f) ,
									Model,
									Projection,
									glm::vec4(0,0,_width, _height) );
		}

		inline void clearColor(float r, float g, float b, float a){
			glClearColor(r, g, b, a);
		}

	protected:
		////////////////////////////////////////////////////////////
		///Set some general settings
		////////////////////////////////////////////////////////////
		inline void _InitGeneralSettings(void)
		{
			this->camera	= glm::vec3(0,0,0);
			this->zoom		= glm::vec3(1,1,1);

			//glDisable(GL_LIGHTING);
			glDisable(GL_DITHER);
			glDisable(GL_DEPTH_TEST);

			glewInit();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glViewport(0, 0, _width, _height);

			MVP		= glm::mat4();
			View	= glm::mat4(1.0f);
			Model	= glm::mat4(1.0f);

			_timer	= &timing::GlobalTime::Instance();

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

	public:
		///Public data
		sf::RenderWindow *Window;
		GLuint Texture[256];

		Sprite cursor;
		glm::vec3 camera;
		glm::vec3 zoom;
		glm::mat4 MVP;
		float 	FrameTime;
		size_t 	SkipFrames;
		glm::mat4	Projection;
		glm::mat4	View;
		glm::mat4	Model;

	protected:
		///Protected data
		sf::String _text;
		sf::String _fps;
		int			fps;

		sf::Clock*	_timer;
		float		_draw_time;
		unsigned 	_skip_frames_counter;

		float		_height, _width;
	};

} //namespace gfx


#endif //COREPROFILE_HEADER_GUARD
