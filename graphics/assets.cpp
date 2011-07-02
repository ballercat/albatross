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
#include <SFML/Graphics.hpp>
#include <cstdio>
#include "assets.h"
#include "SimpleIni.h"

////////////////////////////////////////////////////////////
bool LoadTex(GLuint texid, const char *fpath)
{
    sf::Image rawimage;
    if(!rawimage.LoadFromFile(fpath)){
		fprintf(stderr, "Failed loading %s\n", fpath);
        return false;
    }

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texid);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawimage.GetWidth(), rawimage.GetHeight(),
                   0, GL_RGBA, GL_UNSIGNED_BYTE, rawimage.GetPixelsPtr());

	glDisable(GL_TEXTURE_2D);

    return true;
}

////////////////////////////////////////////////////////////
void LoadTexturesFromData(std::vector<std::string> *tname, GLuint* Texture, GLuint type)
{
	if(tname->empty())
		return;

	glGenTextures(tname->size(), Texture);

	for(size_t k = 0; k < tname->size();k++){
		if(!LoadTex(Texture[k], (*tname)[k].c_str())){
			fprintf(stderr, "Failed loading  %s!\n", (*tname)[k].c_str());
		}
		else {
			fprintf(stdout, "Loaded: %s ...... \n", (*tname)[k].c_str());
		}
	}
}

////////////////////////////////////////////////////////////
GLuint CreateShader(const char *fpath, GLuint type)
{
	FILE *file;
	char *source = NULL;
	GLuint shader; //shader id
	int fsize;

	if(fpath != NULL){
		file = fopen(fpath, "rt");

		if(file == NULL){
			fprintf(stderr, "Could not read shader file: %s\n",fpath);
			return NULL;
		}

		fseek(file, 0, SEEK_END);
		fsize = ftell(file);
		rewind(file);

		source = new char[fsize];
		fsize = fread(source, sizeof(char), fsize, file);
		source[fsize] = '\0';

		fclose(file);

		//constand pointer that is required
		const char* cdata = source;

		//Create shader
		shader = glCreateShader(type);

		//Set the shader source
		glShaderSource(shader, 1, &cdata, NULL);

		//free up the memory
		delete source;

		//Compile the shader
		glCompileShader(shader);

		return shader;
	}

	return NULL;
}

////////////////////////////////////////////////////////////
GLuint* GlobalShader()
{
	static GLuint _glbl_shader_programs[3];

	return _glbl_shader_programs;
}

////////////////////////////////////////////////////////////
glm::mat4* gMatrix()
{
	static glm::mat4 _glbl_matrix[6];

	return &_glbl_matrix[0];
}

////////////////////////////////////////////////////////////
glm::vec3* gTransform()
{
	static glm::vec3 _glbl_transform[6];

	return _glbl_transform;
}

////////////////////////////////////////////////////////////
Shader::~Shader()
{
	glDeleteShader(_vs);
	glDeleteShader(_fs);
	glDeleteProgram(_program);
}

////////////////////////////////////////////////////////////
void Shader::Init(const char *vspath, const char *fspath)
{
	_vs = CreateShader(vspath, GL_VERTEX_SHADER);
	_fs = CreateShader(fspath, GL_FRAGMENT_SHADER);

	_program = glCreateProgram();
	glAttachShader(_program, _vs);
	glAttachShader(_program, _fs);
	glLinkProgram(_program);
}

////////////////////////////////////////////////////////////
void Shader::Use()
{
	glUseProgram(_program);
}

////////////////////////////////////////////////////////////
void Shader::MVP(GLfloat *mvp)
{
	glUniformMatrix4fv(glGetUniformLocation(_program, "MVP"), 1, GL_FALSE, mvp);
}

void Shader::ProjectMat(GLfloat *p_Matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(_program, "ProjectionMatrix"), 1, GL_FALSE, p_Matrix);
}
void Shader::ViewMat(GLfloat *p_Matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(_program, "ViewMatrix"), 1, GL_FALSE, p_Matrix);
}
void Shader::ModelMat(GLfloat *p_Matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(_program, "ModelMatrix"), 1, GL_FALSE, p_Matrix);
}

////////////////////////////////////////////////////////////
GLuint& Shader::operator()(void)
{
	return _program;
}

////////////////////////////////////////////////////////////
GLuint Shader::operator[](const char *uniform)
{
	return glGetUniformLocation(_program, uniform);
}

GLuint Shader::GetAttrib(const char *p_Attrib)
{
	return glGetAttribLocation(_program, p_Attrib);
}
