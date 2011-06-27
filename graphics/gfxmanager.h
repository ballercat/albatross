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

#ifndef GFXMANAGER_HEADER_GUARD
#define GFXMANAGER_HEADER_GUARD

#include"generics.h"
#include"gfx_core.h"
#include"fixed_pipeline.h"
#include"shader_pipeline.h"
#include"renderhooks.h"

namespace gfx
{
	struct Manager
	{
	public:
		////////////////////////////////////////////////////////////
		/// Create a proper contex
		////////////////////////////////////////////////////////////
		static Core*
		NewContext(glm::vec2 p_WindowDim, const char *p_WindowName, bool p_FullScreen = false)
		{
			//Create a context
			sf::RenderWindow *context = NULL;

			unsigned windowstyle = (p_FullScreen) ? sf::Style::Fullscreen : sf::Style::Close;

			context = new sf::RenderWindow(	sf::VideoMode(p_WindowDim.x, p_WindowDim.y),
											p_WindowName,
											windowstyle );

			GLuint glew_error = glewInit();
			if(GLEW_OK != glew_error){
				//failed to init glew exiting
				exit(0x20202);
			}

			//Create the rendering pipleine
			Core *pipeline = NULL;
			renderFunctionPointers *hooks = &Link::Instance();
			if(GLEW_VERSION_3_0){
				pipeline = new ShaderPipeline(context);
				hooks->spriteBuild = &ShaderPipeline::spriteBuild;
				hooks->spriteDraw = &ShaderPipeline::spriteDraw;
			} else if(GLEW_VERSION_1_3){
				pipeline = new FixedPipeline(context);
			}

			return pipeline;
		}
	};
}//namespace gfx

#endif //GFXMANAGER_HEADER_GUARD