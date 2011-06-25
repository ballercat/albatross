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


#include <GL/glew.h>
#include "assets.h"
#include "sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SimpleIni.h"
#include <iostream>
#include <ostream>
#include <cassert>

////////////////////////////////////////////////////////////
/// Default ctor
////////////////////////////////////////////////////////////
Sprite::Sprite()
{
    width = 50; height = 50;
    xstart = 0;
    pivot = glm::vec3(0.5f,0.5f,0.0f);
    off = glm::vec3(0,0,0);
    angle = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);
    color = glm::vec4(1,1,1,1);
	textureid = -1;
    animated = false;

	mSpeed = 0.10f;
    mPosition = 0;
    mLength = 0;
	mTime	= &timing::GlobalTime::Instance();
    mLastUpdate = mTime->GetElapsedTime();
	pShader = NULL;
}

////////////////////////////////////////////////////////////
/// ctor with no sprh file
////////////////////////////////////////////////////////////
Sprite::Sprite(GLuint &p_TextureID)
{
	animated = false;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, p_TextureID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glDisable(GL_TEXTURE_2D);

	imgd = glm::vec2(width, height);

	off = glm::vec3(0.0f, 0.0f, 0.0f);
	pivot = glm::vec3(0.5f, 0.5f, 0.0f);
	angle = glm::vec3(0,0,0);
	scale = glm::vec3(1,1,1);
	color = glm::vec4(1,1,1,1);
	textureid = p_TextureID;

	mSpeed = 1000.0f;
	mPosition = 0;
	mLength = 0;
	mTime = &timing::GlobalTime::Instance();
	mLastUpdate = mTime->GetElapsedTime();
	pShader = NULL;

	Build();
}

////////////////////////////////////////////////////////////
/// ctor with only a .sprh header
////////////////////////////////////////////////////////////
Sprite::Sprite(const char *p_SPRH)
{
	textureid = -1; //This sprite is invalid at the moment
	pShader = NULL;
	//Parese sprh file	
	_parseInfo(p_SPRH);

	//Build it even though you can use it
	Build();
}

////////////////////////////////////////////////////////////
///Generic sprite loader
////////////////////////////////////////////////////////////
Sprite::Sprite(const char *fpath, GLuint texid)
{
	/* File path is NOT to the image but to
	 * the .spr text file holding the sprite data
	 * this really cuts down on having to compile
	 * in all the sprite information every time */
	_parseInfo(fpath);
	pShader = NULL;

	textureid = texid;

	mTime		= &timing::GlobalTime::Instance();
	mLastUpdate = mTime->GetElapsedTime();
	Build();
}

////////////////////////////////////////////////////////////
/// Parse a .sprh file(Sprite header)
////////////////////////////////////////////////////////////
void Sprite::_parseInfo(const char *fpath)
{
	CSimpleIniA sprh;
	if(sprh.LoadFile(fpath) < 0){
		std::cerr << "Error reading " << fpath << " sprite header\n";
	}

	animated = sprh.GetBoolValue("Sprite", "animated");

	width = sprh.GetDoubleValue("Sprite", "width");
	height = sprh.GetDoubleValue("Sprite", "height");

	off.x = sprh.GetDoubleValue("Offset", "x");
	off.y = sprh.GetDoubleValue("Offset", "y");

	pivot.x = sprh.GetDoubleValue("Pivot", "x");
	pivot.y = sprh.GetDoubleValue("Pivot", "y");

	color.r = sprh.GetDoubleValue("Color", "red");
	color.g = sprh.GetDoubleValue("Color", "green");
	color.b = sprh.GetDoubleValue("Color", "blue");
	color.a = sprh.GetDoubleValue("Color", "alpha");

	scale.x = sprh.GetDoubleValue("Scale", "x");
	scale.y = sprh.GetDoubleValue("Scale", "y");
	scale.z = sprh.GetDoubleValue("Scale", "z");

	imgd = glm::vec2(width, height);
	mLength = 0;
	mSpeed = 0;
	xstart = 0;

	if(animated){
		imgd.x = sprh.GetDoubleValue("Image", "width");
		imgd.y = sprh.GetDoubleValue("Image", "height");
		mLength = sprh.GetLongValue("Animation","length");
		mSpeed = sprh.GetDoubleValue("Animation","speed");
		xstart = sprh.GetDoubleValue("Animation","xstart");
	}
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Sprite::Build()
{
	VBOVertex vbodata[6];
	GLushort vboindx[] = { 0, 1, 2, 3, 4, 5 };

	vbodata[0].v = glm::vec3(-pivot.x * width, -pivot.y * height, 0);
	vbodata[1].v = glm::vec3(-pivot.x * width,(1 - pivot.y) * height,0);
	vbodata[2].v = glm::vec3((1 - pivot.x) * width, (1 - pivot.y) * height,0);
	//Triangle 1
	vbodata[3].v = glm::vec3(-pivot.x * width, -pivot.y * height, 0);
	vbodata[4].v = glm::vec3((1 - pivot.x) * width, (1 - pivot.y) * height,0);
	vbodata[5].v = glm::vec3((1 - pivot.x) * width, -pivot.y * height,0);

	//Texture coordinates
	float zrx = off.x/imgd.x;
	float zry = off.y/imgd.y;
	float h = height/imgd.y;
	float w = width/imgd.x;
	//Triangle 0
	vbodata[0].t = glm::vec2(zrx, zry + h);
	vbodata[1].t = glm::vec2(zrx, zry);
	vbodata[2].t = glm::vec2(zrx + w, zry);
	//Triangle 1
	vbodata[3].t = glm::vec2(zrx, zry + h);
	vbodata[4].t = glm::vec2(zrx + w, zry);
	vbodata[5].t = glm::vec2(zrx + w, zry + h);

	if(animated){
		pShader = new Shader("assets/shader/default.vert", "assets/shader/animated.frag");
	}
	else{
		pShader = new Shader("assets/shader/default.vert", "assets/shader/sprite.frag");
	}
	glGenVertexArrays(1, &pVertexVAO);
	glBindVertexArray(pVertexVAO);

	glGenBuffers(1, &pVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vbodata), vbodata, GL_STATIC_DRAW);

	glGenBuffers(1, &pIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vboindx), vboindx, GL_STATIC_DRAW);

	glVertexAttribPointer(pShader->GetAttrib("vPosition"), 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(pShader->GetAttrib("vPosition"));

	glVertexAttribPointer(pShader->GetAttrib("vUV"), 2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), BUFFER_OFFSET(sizeof(glm::vec3)));
	glEnableVertexAttribArray(pShader->GetAttrib("vUV"));

	//glVertexAttribPointer(pShader->GetAttrib("vColor"), 4, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), BUFFER_OFFSET(sizeof(glm::vec3)+sizeof(glm::vec2)));
	//glEnableVertexAttribArray(pShader->GetAttrib("vColor"));

	pShader->Use();
	StepLoc  = (*pShader)["StepSize"];
	ColorLoc = (*pShader)["SpriteColor"];
}

////////////////////////////////////////////////////////////
/// Step trough the frames
////////////////////////////////////////////////////////////
void Sprite::Step(void)
{
	//Don't waste time with static sprites
	if(!mLength)
		return;

	float t		= mTime->GetElapsedTime();

	//Global Time can be reset, need to account for that so..
	if( t < mLastUpdate )
		mLastUpdate = t - mSpeed;

	float dt 	= t - mLastUpdate;

	if( dt >= mSpeed )
    {
        mPosition+=int(dt/mSpeed);
        if(mPosition > mLength){
			mPosition = 0;
		}
        mLastUpdate = t;
    }

}

void Sprite::Draw()
{
	assert(textureid != -1);

	glm::mat4 view = glm::translate(gMatrix()[2], pos);
	view = glm::scale(view, scale);
	view = glm::rotate(view, angle.z, glm::vec3(0,0,1));
	view = glm::rotate(view, angle.x, glm::vec3(0,1,0));

	glm::mat4 model(1.0f);
	pShader->Use();

	if(animated){
		float step = (xstart + (mPosition*width))/imgd.x;
		glUniform1f((*pShader)["StepSize"], step);
		//glUniform1f(StepLoc, step);
	}
	//glUniform4fv((*pShader)["SpriteColor"], 1, &color.r);
	glUniform4fv(ColorLoc, 1, &color.r);
	pShader->ProjectMat(glm::value_ptr(gMatrix()[0]));
	pShader->ViewMat(glm::value_ptr(view));
	pShader->ModelMat(glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureid);

	glBindVertexArray(pVertexVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

