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

Sprite::Sprite()
{
    width = 50; height = 50;
    xstart = 0;
    pivot = glm::vec3(0.5f,0.5f,0.0f);
    off = glm::vec3(0,0,0);
    angle = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);
    color = glm::vec4(1,1,1,1);

    mSpeed = 0.10f;
    mPosition = 0;
    mLength = 0;
    mLastUpdate = mTimer.GetElapsedTime();
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

	textureid = texid;
	mLastUpdate = mTimer.GetElapsedTime();
	Build();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
Sprite::~Sprite(void){}

void Sprite::_parseInfo(const char *fpath)
{
	/*
    FILE *fp;
    
    fp = fopen(fpath, "r");
    if(fp == NULL){
        fprintf(stderr, "Can't open sprite header file: %s\n", fpath);
        return;
    }
    
    //No need to loop trough this or anything 
    //too fancy, the variables are always the same
    fscanf(fp, "width = %i\n", &width);
    fscanf(fp, "height = %i\n", &height);
    fscanf(fp, "imgd = %f, %f\n", &imgd.x, &imgd.y);
    fscanf(fp, "off = %f, %f, %f\n", &off.x, &off.y, &off.z);
	fscanf(fp, "pivot = %f, %f\n", &pivot.x, &pivot.y);
    fscanf(fp, "color = %f, %f, %f, %f\n", &color.r, &color.g, &color.b, &color.a);
	fscanf(fp, "scale = %f, %f, %f\n", &scale.x, &scale.y, &scale.z);
    fscanf(fp, "xstart = %f\n", &xstart);
    fscanf(fp, "length = %i\n", &mLength);
    fscanf(fp, "speed = %f\n", &mSpeed);

	fclose(fp); */

	CSimpleIniA sprh;
	if(sprh.LoadFile(fpath) < 0){
		std::cerr << "Error reading " << fpath << " sprite header\n";
	}

	width = sprh.GetDoubleValue("Sprite", "width");
	height = sprh.GetDoubleValue("Sprite", "height");

	imgd.x = sprh.GetDoubleValue("Image", "width");
	imgd.y = sprh.GetDoubleValue("Image", "height");

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

	mLength = sprh.GetLongValue("Animation","length");
	if(mLength){
		mSpeed = sprh.GetDoubleValue("Animation","speed");
		xstart = sprh.GetDoubleValue("Animation","xstart");
	}
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Sprite::Build()
{
	//Vertex data
	//Triangle 0
	vertdata[0] = glm::vec3(-pivot.x * width, -pivot.y * height, 0);
	vertdata[1] = glm::vec3(-pivot.x * width,(1 - pivot.y) * height,0);
	vertdata[2] = glm::vec3((1 - pivot.x) * width, (1 - pivot.y) * height,0);
	//Triangle 1
	vertdata[3] = glm::vec3(-pivot.x * width, -pivot.y * height, 0);
	vertdata[4] = glm::vec3((1 - pivot.x) * width, (1 - pivot.y) * height,0);
	vertdata[5] = glm::vec3((1 - pivot.x) * width, -pivot.y * height,0);

	//Texture coordinates
	float zrx = off.x/imgd.x;
	float zry = off.y/imgd.y;
	float h = height/imgd.y;
	float w = width/imgd.x;
	//Triangle 0
	texdata[0] = glm::vec2(zrx, zry + h);
	texdata[1] = glm::vec2(zrx, zry);
	texdata[2] = glm::vec2(zrx + w, zry);
	//Triangle 1
	texdata[3] = glm::vec2(zrx, zry + h);
	texdata[4] = glm::vec2(zrx + w, zry);
	texdata[5] = glm::vec2(zrx + w, zry + h);

#ifdef SHADER_PIPELINE
	glm::vec4 vColor[] = { color, color, color, color, color, color };

	_quad.Init(textureid, 6, GlobalShader()[0]);
	_quad.VertexPointer(vertdata, 6*sizeof(glm::vec3));
	_quad.ColorPointer(vColor, 6*sizeof(glm::vec4));
	_quad.TexturePointer(texdata, 6*sizeof(glm::vec2));
	_quad.Bind();

	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);
	proj = glm::ortho(-512.0f,512.0f,-300.0f,300.0f,-1.0f,1.0f);
	model = glm::mat4(1.0f);
	mvp = model * view * proj;

	_mvp = glGetUniformLocation(GlobalShader()[0], "MVP");
#endif
}

void Sprite::Step(void)
{
    float t = mTimer.GetElapsedTime();
	float dt = t - mLastUpdate;
	if( dt >= mSpeed )
    {
        mPosition+=int(dt/mSpeed);
        if(mPosition > mLength){
			mPosition = 0;
		}
        mLastUpdate = t;

        off.x = xstart + (mPosition*width);

		float zrx = off.x/imgd.x;
		float zry = off.y/imgd.y;
		float h = height/imgd.y;
		float w = width/imgd.x;
		texdata[0] = glm::vec2(zrx, zry + h);
		texdata[1] = glm::vec2(zrx, zry);
		texdata[2] = glm::vec2(zrx + w, zry);
		texdata[3] = glm::vec2(zrx, zry + h);
		texdata[4] = glm::vec2(zrx + w, zry);
		texdata[5] = glm::vec2(zrx + w, zry + h);
#ifdef SHADER_PIPELINE
		_quad.TexturePointer(texdatacoord, 6*sizeof(glm::vec2));
#endif
    }

}

void Sprite::Draw()
{/*
	vcoord[0] = glm::vec3(pos.x - pivot.x * width, pos.y - pivot.y * height, 0);
	vcoord[1] = glm::vec3(pos.x - pivot.x * width, pos.y + (1 - pivot.y) * height,0);
	vcoord[2] = glm::vec3(pos.x + (1 - pivot.x) * width, pos.y  + (1 - pivot.y) * height,0);
	vcoord[3] = glm::vec3(pos.x - pivot.x * width, pos.y - pivot.y * height, 0);
	vcoord[4] = glm::vec3(pos.x + (1 - pivot.x) * width, pos.y + (1 - pivot.y) * height,0);
	vcoord[5] = glm::vec3(pos.x + (1 - pivot.x) * width, pos.y - pivot.y * height,0);
	_quad.VertexPointer(vcoord, 6*sizeof(glm::vec3)); */
#ifdef SHADER_PIPELINE
	view = glm::translate(gMatrix()[3], pos);
	view = glm::scale(view, scale);
	view = glm::rotate(view, angle.z, glm::vec3(0,0,1));
	view = glm::rotate(view, angle.x, glm::vec3(0,1,0));
	mvp = view;
	glUseProgram(GlobalShader()[0]);
	glUniformMatrix4fv(_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	_quad.Draw();
#endif

	glEnable(GL_TEXTURE_2D);{
		glBindTexture(GL_TEXTURE_2D, textureid);
		//Translate
		glPushMatrix();{
			//glLoadIdentity();
			glTranslated(pos.x, pos.y, 0.0f);
			glScalef(scale.x, scale.y, scale.z);
			glRotatef(angle.x, 0.0f, -1.0f, 0.0f);
			glRotatef(angle.z, 0.0f, 0.0f, 1.0f);

			//Color
			glColor4f( color.x, color.y, color.z, color.a );

			//vertdataexdataes
			glBegin(GL_TRIANGLES);{
				glTexCoord2f( texdata[0].x, texdata[0].y );
				glVertex3f( vertdata[0].x, vertdata[0].y, vertdata[0].z );

				glTexCoord2f( texdata[1].x, texdata[1].y );
				glVertex3f( vertdata[1].x, vertdata[1].y, vertdata[1].z );

				glTexCoord2f( texdata[2].x, texdata[2].y );
				glVertex3f( vertdata[2].x, vertdata[2].y, vertdata[2].z );

				glTexCoord2f( texdata[3].x, texdata[3].y );
				glVertex3f( vertdata[3].x, vertdata[3].y, vertdata[3].z );

				glTexCoord2f( texdata[4].x, texdata[4].y );
				glVertex3f( vertdata[4].x, vertdata[4].y, vertdata[4].z );

				glTexCoord2f( texdata[5].x, texdata[5].y );
				glVertex3f( vertdata[5].x, vertdata[5].y, vertdata[5].z );
			}glEnd();

		}glPopMatrix();
	}glDisable(GL_TEXTURE_2D);

}

