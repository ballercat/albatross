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
        ///Scene managment
        virtual void resizeWindow(int w, int h) = 0;
        virtual void setCursor(const char *fpath) = 0;

		virtual void beginScene() = 0;
		virtual void endScene() = 0;

	public:
		///Rendering helper functions
		virtual void drawArray(GLvoid *v, GLvoid *t, GLvoid *c, GLuint size, GLuint type, GLuint texid) = 0;
		virtual void drawText(glm::vec3 pos, const char *text,size_t size) = 0;
		virtual void drawSprite(Sprite *spr) = 0;

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

	public:
		///Public data
		sf::RenderWindow *Window;
		GLuint Texture[256];

		Sprite cursor;
		glm::vec3 camera;
		glm::vec3 zoom;
		glm::mat4 MVP;
		float FrameTime;
		int SkipFrames;

	protected:
		///Protected data
		sf::String _text;
		sf::String _fps;
		int			fps;

		glm::mat4	Projection;
		glm::mat4	View;
		glm::mat4	Model;

		sf::Clock	_timer;
		float		_draw_time;
		unsigned 	_skip_frames_counter;

		float		_height, _width;
	};

} //namespace gfx


#endif //COREPROFILE_HEADER_GUARD
