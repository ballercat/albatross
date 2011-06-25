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


#include"shader_pipeline.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

using namespace gfx;

ShaderPipeline::ShaderPipeline(	unsigned p_Width,
								unsigned p_Height,
								const char* p_Name,
								bool p_Fullscreen)
{
	_width	= p_Width;
	_height	= p_Height;
	_timer	= &timing::GlobalTime::Instance();
	unsigned window_style = (p_Fullscreen) ? sf::Style::Fullscreen : sf::Style::Close;

	Window 	= new sf::RenderWindow(sf::VideoMode(_width, _height), p_Name, window_style);

	camera	= glm::vec3(0,0,0);
	zoom	= glm::vec3(1,1,1);

	_InitGeneralSettings();

	MVP		= glm::mat4();
	View	= glm::mat4(1.0f);
	Model	= glm::mat4(1.0f);
	Projection = glm::ortho(_width/-2.0f, _width/2.0f, _height/-2.0f, _height/2.0f, -5.0f, 5.0f);
	MVP		= Model * View * Projection;

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

void ShaderPipeline::beginScene()
{
	_draw_time = _timer->GetElapsedTime();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	View = glm::scale(glm::mat4(1.0f), zoom);
	View = glm::translate(View, -camera);

	MVP = Model * View * Projection;
	gMatrix()[0] = Projection;
	gMatrix()[1] = Model;
	gMatrix()[2] = View;
	gMatrix()[3] = MVP;

	gTransform()[0] = camera;
}

void ShaderPipeline::endScene()
{
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
	glUseProgram(0);
}

void ShaderPipeline::drawArray(GLvoid *v,GLvoid *t,GLvoid *c,GLuint size,GLuint type,GLuint texid)
{

}

void ShaderPipeline::drawText(glm::vec3 pos, const char* text, size_t size)
{
    _text.SetText(text);
    _text.SetPosition(pos.x,pos.y);
    _text.SetSize(size);
    Window->Draw(_text);
}

////////////////////////////////////////////////////////////
void ShaderPipeline::drawSprite(Sprite *spr)
{
	if(NULL == spr)
		return;
	glm::mat4 view = glm::translate(glm::mat4(1.0f), spr->pos);
	spr->pShader->Use();
	spr->pShader->ProjectMat(glm::value_ptr(Projection));
	spr->pShader->ViewMat(glm::value_ptr(view));
	spr->pShader->ModelMat(glm::value_ptr(Model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spr->textureid);

	glBindVertexArray(spr->pVertexVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}
