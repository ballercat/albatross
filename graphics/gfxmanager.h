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
#include<glm/gtc/type_ptr.hpp>

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
			renderPointers *hooks = &Link::Instance();
			if(GLEW_VERSION_3_1){

				pipeline = new ShaderPipeline(context);

				//Setup Sprite function pointers
				hooks->spriteBuild = &ShaderPipeline::spriteBuild;
				hooks->spriteDraw = &ShaderPipeline::spriteDraw;

				//Setup Shaders
				hooks->glsl.Default = new Shader("assets/shader/default.vert", "assets/shader/default.frag");
				hooks->glsl.sprDefault = new Shader("assets/shader/sprite.vert", "assets/shader/sprite.frag");
				hooks->glsl.sprAnimated = new Shader("assets/shader/sprite.vert", "assets/shader/animated.frag");

				hooks->glsl.Default->Use();
				hooks->glsl.Default->ProjectMat(glm::value_ptr(pipeline->Projection));

				hooks->glsl.sprDefault->Use();
				hooks->glsl.sprDefault->ModelMat(glm::value_ptr(pipeline->Model));
				hooks->glsl.sprDefault->ProjectMat(glm::value_ptr(pipeline->Projection));

				hooks->glsl.sprAnimated->Use();
				hooks->glsl.sprAnimated->ModelMat(glm::value_ptr(pipeline->Model));
				hooks->glsl.sprAnimated->ProjectMat(glm::value_ptr(pipeline->Projection));

				ShaderPipeline::setupBuffers();
				glUseProgram(0);
			} else if(GLEW_VERSION_1_3){
				pipeline = new FixedPipeline(context);
			}

			return pipeline;
		}
	};
}//namespace gfx

#endif //GFXMANAGER_HEADER_GUARD
