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

#ifndef SHADERPIPELINE_HEADER_GUARD
#define SHADERPIPELINE_HEADER_GUARD

#include"gfx_core.h"

namespace gfx
{

class ShaderPipeline : public Core
{
    public:
        ////////////////////////////////////////////////////////////
		/// ctor
		////////////////////////////////////////////////////////////
		ShaderPipeline( unsigned p_Width,
                        unsigned p_Height,
                        const char *p_Name="humpback(0.0.5a)",
                        bool p_Fullscreen=false);

		////////////////////////////////////////////////////////////
		/// ctor with a handler from a graphics manager
		////////////////////////////////////////////////////////////
		ShaderPipeline(sf::RenderWindow *p_WindowHandle);

    public:
        virtual void beginScene();
        virtual void endScene();
        
    public:
        virtual void drawArray(GLvoid *v, GLvoid *t, GLvoid *c, GLuint size, GLuint type, GLuint texid);
		virtual void drawText(glm::vec3 pos, const char *text,size_t size);

	public:
		static void	spriteBuild(Sprite *p_sprPtr);
		static void spriteDraw(Sprite *p_sprPtr);

	private:
		VertexArray	*mArray;
};

}
#endif //SHADERPIPELINE_HEADER_GUARD
