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


#ifndef FIXEDPIPELINE_HEADER_GUARD
#define FIXEDPIPELINE_HEADER_GUARD

#include "gfx_core.h"

namespace gfx
{
	////////////////////////////////////////////////////////////
	///Fixed pipeline display core
	///does not use anything past OGL 2.0
	////////////////////////////////////////////////////////////
	class FixedPipeline : public Core
	{
	public:
		////////////////////////////////////////////////////////////
		///Default Constructor
		////////////////////////////////////////////////////////////
		FixedPipeline(	unsigned pWidth,
						unsigned pHeight,
						const char *name="finback(0.0.4a)",
						bool fullscreen=false);

		////////////////////////////////////////////////////////////
		/// ctor from a window handle
		////////////////////////////////////////////////////////////
		FixedPipeline( sf::RenderWindow *p_Window);

	public:
		////////////////////////////////////////////////////////////
		///Scene managment
		////////////////////////////////////////////////////////////
		virtual void beginScene();
		virtual void endScene();

	public:
		///Rendering helper functions
		virtual void drawArray(GLvoid *v, GLvoid *t, GLvoid *c, GLuint size, GLuint type, GLuint texid);
		virtual void drawText(glm::vec3 pos, const char *text,size_t size);

		static void setupBuffers();
		static void spriteBuild(Sprite *spr);
		static void spriteDraw(Sprite *spr);
	};
}

#endif //FIXEDPIPELINE_HEADER_GUARD
