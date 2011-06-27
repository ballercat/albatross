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

////////////////////////////////////////////////////////////
/// default ctor
////////////////////////////////////////////////////////////
ShaderPipeline::ShaderPipeline(	unsigned p_Width,
								unsigned p_Height,
								const char* p_Name,
								bool p_Fullscreen)
{
	_width	= p_Width;
	_height	= p_Height;
	unsigned window_style = (p_Fullscreen) ? sf::Style::Fullscreen : sf::Style::Close;

	Window 	= new sf::RenderWindow(sf::VideoMode(_width, _height), p_Name, window_style);

	_InitGeneralSettings();

	Projection = glm::ortho(_width/-2.0f, _width/2.0f, _height/-2.0f, _height/2.0f, -5.0f, 5.0f);
	MVP		= Model * View * Projection;
}

////////////////////////////////////////////////////////////
/// ctor with a window handle from a manager
////////////////////////////////////////////////////////////
ShaderPipeline::ShaderPipeline(sf::RenderWindow *p_WindowHandle)
{
	Window 	= p_WindowHandle;

	_width 	= Window->GetWidth();
	_height = Window->GetHeight();

	_InitGeneralSettings();

	Projection = glm::ortho(_width/-2.0f, _width/2.0f, _height/-2.0f, _height/2.0f, -5.0f, 5.0f);
	MVP		= Model * View * Projection;
}

void ShaderPipeline::beginScene()
{
	_draw_time = _timer->GetElapsedTime();

	glClear(GL_COLOR_BUFFER_BIT);

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

void ShaderPipeline::spriteBuild(Sprite *p_sprPtr)
{
	VBOVertex vbodata[6];
	GLushort vboindx[] = { 0, 1, 2, 3, 4, 5 };
	float px = p_sprPtr->pInfo.pivot.x;
	float py = p_sprPtr->pInfo.pivot.y;
	float w = p_sprPtr->pInfo.w;
	float h = p_sprPtr->pInfo.h;

	vbodata[0].v = glm::vec3(-px * w, -py * h, 0.0f);
	vbodata[1].v = glm::vec3(-px * w,(1 - py) * h,0.0f);
	vbodata[2].v = glm::vec3((1 - px) * w, (1 - py) * h,0.0f);
	//Triangle 1
	vbodata[3].v = glm::vec3(-px * w, -py * h, 0.0f);
	vbodata[4].v = glm::vec3((1 - px) * w, (1 - py) * h,0.0f);
	vbodata[5].v = glm::vec3((1 - px) * w, -py * h,0.0f);

	//Texture coordinates
	float zrx = 0.0f;
	float zry = 0.0f;
	h = h/p_sprPtr->pInfo.imgHeight;
	w = w/p_sprPtr->pInfo.imgWidth;
	//Triangle 0
	vbodata[0].t = glm::vec2(zrx, zry + h);
	vbodata[1].t = glm::vec2(zrx, zry);
	vbodata[2].t = glm::vec2(zrx + w, zry);
	//Triangle 1
	vbodata[3].t = glm::vec2(zrx, zry + h);
	vbodata[4].t = glm::vec2(zrx + w, zry);
	vbodata[5].t = glm::vec2(zrx + w, zry + h);

	if(p_sprPtr->pInfo.animated){
		p_sprPtr->pInfo.shdProgram = new Shader("assets/shader/default.vert", "assets/shader/animated.frag");
	}
	else{
		p_sprPtr->pInfo.shdProgram = new Shader("assets/shader/default.vert", "assets/shader/sprite.frag");
	}
	glGenVertexArrays(1, &p_sprPtr->pInfo.vaoVertex);
	glBindVertexArray(p_sprPtr->pInfo.vaoVertex);

	glGenBuffers(1, &p_sprPtr->pInfo.vboVertex);
	glBindBuffer(GL_ARRAY_BUFFER, p_sprPtr->pInfo.vboVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vbodata), vbodata, GL_STATIC_DRAW);

	glGenBuffers(1, &p_sprPtr->pInfo.vboIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_sprPtr->pInfo.vboIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vboindx), vboindx, GL_STATIC_DRAW);

	glVertexAttribPointer(	p_sprPtr->pInfo.shdProgram->GetAttrib("vPosition"),
							3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
							BUFFER_OFFSET(0));
	glEnableVertexAttribArray(p_sprPtr->pInfo.shdProgram->GetAttrib("vPosition"));

	glVertexAttribPointer(	p_sprPtr->pInfo.shdProgram->GetAttrib("vUV"),
							2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
							BUFFER_OFFSET(sizeof(glm::vec3)));
	glEnableVertexAttribArray(p_sprPtr->pInfo.shdProgram->GetAttrib("vUV"));

	p_sprPtr->pInfo.shdAniStep_loc  = (*p_sprPtr->pInfo.shdProgram)["StepSize"];
	p_sprPtr->pInfo.shdColor_loc = (*p_sprPtr->pInfo.shdProgram)["SpriteColor"];
}

void ShaderPipeline::spriteDraw(Sprite *p_sprPtr)
{
	glm::mat4 view = glm::translate(gMatrix()[2], p_sprPtr->pInfo.pos);
	view = glm::scale(view, p_sprPtr->pInfo.scale);
	view = glm::rotate(view, p_sprPtr->pInfo.angle.z, glm::vec3(0,0,1));
	view = glm::rotate(view, p_sprPtr->pInfo.angle.x, glm::vec3(0,1,0));

	glm::mat4 model(1.0f);
	p_sprPtr->pInfo.shdProgram->Use();

	if(p_sprPtr->pInfo.animated){
		float step = (p_sprPtr->pInfo.aniPosition*p_sprPtr->pInfo.w)/p_sprPtr->pInfo.imgWidth;
		//glUniform1f((*p_sprPtr->pInfo.shdProgram)["StepSize"], step);
		//glUniform1f(StepLoc, step);
		glUniform1f(p_sprPtr->pInfo.shdAniStep_loc, step);
	}
	//glUniform4fv((*pShader)["SpriteColor"], 1, &color.r);
	glUniform4fv(p_sprPtr->pInfo.shdColor_loc, 1, &p_sprPtr->pInfo.color.r);
	//glUniform4fv((*p_sprPtr->pInfo.shdProgram)["SpriteColor"], 1, &p_sprPtr->pInfo.color.r);
	p_sprPtr->pInfo.shdProgram->ProjectMat(glm::value_ptr(gMatrix()[0]));
	p_sprPtr->pInfo.shdProgram->ViewMat(glm::value_ptr(view));
	p_sprPtr->pInfo.shdProgram->ModelMat(glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_sprPtr->pInfo.txrId);

	glBindVertexArray(p_sprPtr->pInfo.vaoVertex);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}
