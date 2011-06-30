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
#include"renderhooks.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

using namespace gfx;

static GLuint	sprVert_vao, sprVert_vbo, sprIndx_vbo;

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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(_width/2),(_width/2),-(_height/2),(_height/2),-5.0f, 5.0f);
}

void ShaderPipeline::beginScene()
{
	_draw_time = _timer->GetElapsedTime();

	glClear(GL_COLOR_BUFFER_BIT);

	View = glm::scale(glm::mat4(1.0f), zoom);
	View = glm::translate(View, -camera);

	MVP = Model * View * Projection;

	gMatrix()[2] = View;
}

void ShaderPipeline::setupBuffers()
{
	VBOVertex vbodata[6];

	GLushort vboindx[] = { 0, 1, 2, 3, 4, 5 };
	vbodata[0].v = glm::vec3( 0.0f, 0.0f, 0.0f);
	vbodata[1].v = glm::vec3( 0.0f, 1.0f, 0.0f);
	vbodata[2].v = glm::vec3( 1.0f, 1.0f, 0.0f);
	//Triangle 2
	vbodata[3].v = glm::vec3( 0.0f, 0.0f, 0.0f);
	vbodata[4].v = glm::vec3( 1.0f, 1.0f, 0.0f);
	vbodata[5].v = glm::vec3( 1.0f, 0.0f, 0.0f);

	//Texture coordinates
	vbodata[0].t = glm::vec2(0, 1);
	vbodata[1].t = glm::vec2(0, 0);
	vbodata[2].t = glm::vec2(1, 0);
	//Triangle 2
	vbodata[3].t = glm::vec2(0, 1);
	vbodata[4].t = glm::vec2(1, 0);
	vbodata[5].t = glm::vec2(1, 1);

	glGenVertexArrays(1, &sprVert_vao);
	glBindVertexArray(sprVert_vao);

	glGenBuffers(1, &sprVert_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, sprVert_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vbodata), vbodata, GL_STATIC_DRAW);

	glGenBuffers(1, &sprIndx_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprIndx_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vboindx), vboindx, GL_STATIC_DRAW);

	glVertexAttribPointer(Link::Instance().glsl.sprAnimated->GetAttrib("vPosition"),
						3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
						BUFFER_OFFSET(0));
	glVertexAttribPointer(Link::Instance().glsl.sprDefault->GetAttrib("vPosition"),
						3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
						BUFFER_OFFSET(0));

	glVertexAttribPointer(Link::Instance().glsl.sprAnimated->GetAttrib("vUV"),
						2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
						BUFFER_OFFSET(sizeof(glm::vec3)));
	glVertexAttribPointer(Link::Instance().glsl.sprDefault->GetAttrib("vUV"),
						2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex),
						BUFFER_OFFSET(sizeof(glm::vec3)));

	glEnableVertexAttribArray(Link::Instance().glsl.sprDefault->GetAttrib("vUV"));
	glEnableVertexAttribArray(Link::Instance().glsl.sprDefault->GetAttrib("vPosition"));
	glEnableVertexAttribArray(Link::Instance().glsl.sprAnimated->GetAttrib("vUV"));
	glEnableVertexAttribArray(Link::Instance().glsl.sprAnimated->GetAttrib("vPosition"));
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
    if(t){
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, t);
        glBindTexture(GL_TEXTURE_2D, texid);
    }
    if(c){
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, c);
    }

    glEnableClientState(GL_VERTEX_ARRAY);

	glActiveTexture(GL_TEXTURE0);
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

void ShaderPipeline::drawText(glm::vec3 pos, const char* text, size_t size)
{
    _text.SetText(text);
    _text.SetPosition(pos.x,pos.y);
    _text.SetSize(size);
    Window->Draw(_text);
}

void ShaderPipeline::spriteBuild(Sprite *p_sprPtr)
{
	if(p_sprPtr->pInfo.animated)
		p_sprPtr->pInfo.shdProgram = Link::Instance().glsl.sprAnimated;
	else
		p_sprPtr->pInfo.shdProgram = Link::Instance().glsl.sprDefault;

	p_sprPtr->pInfo.vaoVertex = sprVert_vao;
	p_sprPtr->pInfo.shdAniPos_loc  = (*p_sprPtr->pInfo.shdProgram)["AnimationPosition"];
	p_sprPtr->pInfo.shdAniLength_loc = (*p_sprPtr->pInfo.shdProgram)["AnimationLength"];
	p_sprPtr->pInfo.shdColor_loc = (*p_sprPtr->pInfo.shdProgram)["SpriteColor"];
}

void ShaderPipeline::spriteDraw(Sprite *p_sprPtr)
{
	glm::mat4 view(1.0f);
	//view = gMatrix()[2];
	view = gMatrix()[2];
	view = glm::translate(view, p_sprPtr->pInfo.pos);
	view = glm::rotate(view, p_sprPtr->pInfo.angle.z, glm::vec3(0,0,1));
	view = glm::rotate(view, p_sprPtr->pInfo.angle.x, glm::vec3(0,1,0));
	view = glm::scale(view, glm::vec3(p_sprPtr->pInfo.w, p_sprPtr->pInfo.h, 1));
	view = glm::scale(view, p_sprPtr->pInfo.scale);
	view = glm::translate(view, -p_sprPtr->pInfo.pivot);
	p_sprPtr->pInfo.shdProgram->Use();

	if(p_sprPtr->pInfo.animated){
		glUniform1f(p_sprPtr->pInfo.shdAniPos_loc, p_sprPtr->pInfo.aniPosition);
		glUniform1f(p_sprPtr->pInfo.shdAniLength_loc, p_sprPtr->pInfo.aniLength+1);
	}


	glUniform4fv(p_sprPtr->pInfo.shdColor_loc, 1, &p_sprPtr->pInfo.color.r);

	p_sprPtr->pInfo.shdProgram->ViewMat(glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_sprPtr->pInfo.txrId);

	glBindVertexArray(p_sprPtr->pInfo.vaoVertex);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}
