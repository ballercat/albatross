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


#ifndef ASSETS_HEADER_GUARD
#define ASSETS_HEADER_GUARD
#include"vertbuffer.h"
#include <GL/gl.h>
#include <glm/glm.hpp>

extern bool LoadTex(GLuint texid, const char *fpath);
extern GLuint CreateShader(const char *fpath, GLuint type);
extern GLuint* GlobalShader();
extern glm::mat4* gMatrix();
extern glm::vec3* gTransform();

struct VBOVertex
{
	glm::vec3	v;
	glm::vec2	t;
	glm::vec4	c;
	glm::vec3	n;
};

struct Shader
{
public:
	Shader(const char *vspath, const char *fspath)
	{
		Init(vspath, fspath);
	}
	Shader() :
		_fs(0),
		_vs(0),
		_program(0)
		{}
	Shader(const Shader &p_Copy) :
		_fs(p_Copy._fs),
		_vs(p_Copy._vs),
		_program(_program)
		{

		}

	~Shader();

	void Init(const char *vspath, const char *fspath);
	void Use();
	void MVP(GLfloat *mvp);


	GLuint& operator()(void);
	GLuint operator[](const char *uniform);
	GLuint GetAttrib(const char *p_Attrib);

	void ProjectMat(GLfloat *p_Matrix);
	void ViewMat(GLfloat *p_Matrix);
	void ModelMat(GLfloat *p_Matrix);

	Shader& operator=(const Shader& p_Copy)
	{
		_fs = p_Copy._fs;
		_vs = p_Copy._vs;
		_program = p_Copy._program;

		return *this;
	}

public:
	GLuint _fs, _vs;
	GLuint _program;
};



#endif //ASSETS_HEADER_GUARD
