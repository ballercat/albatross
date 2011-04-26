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
static bool LoadTex(GLuint texid, const char *fpath)
{
    sf::Image rawimage;
    if(!rawimage.LoadFromFile(fpath)){
        return false;
    }

#ifdef SHADER_PIPELINE
	glActiveTexture(GL_TEXTURE0);
#else
	glEnable(GL_TEXTURE_2D);
#endif
    glBindTexture(GL_TEXTURE_2D, texid);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawimage.GetWidth(), rawimage.GetHeight(),
                   0, GL_RGBA, GL_UNSIGNED_BYTE, rawimage.GetPixelsPtr());

#ifdef FIXED_PIPELINE
	glDisable(GL_TEXTURE_2D);
#endif

    return true;
}

////////////////////////////////////////////////////////////
void LoadTextures(const char *fpath, GLuint* Texture)
{
/*	FILE *fp;

    fp = fopen(fpath, "r");
    if(fp == NULL){
        fprintf(stderr, "Can't open texture list:%s!\n",fpath);
        exit(0x300);
    }

    char texpath[256], texname[256];
    GLuint texid = 0;
	GLuint type; */
	texini.LoadFile(fpath);

    glGenTextures(256, Texture);
	CSimpleIniA texini;
	
	
	std::string texpath;
	LoadTex(Texture[PLAYER_TORSO], texini.GetValue("Merc", "still"));
	LoadTex(Texture[PLAYER_LEGS], texini.GetValue("Merc", "run"));
	LoadTex(Texture[PLAYER_JETS], texini.GetValue("Merc", "jets"));
	LoadTex(Texture[JET_FLAME], texini.GetValue("Merc", "jetflame"));

	texpath = texini.GetValue("Map", "plain");
	LoadTex(Texture[WORLD_PLAIN], texpath.c_str());

	texpath = texini.GetValue("AK", "main");
	LoadTex(Texture[AK47], texpath.c_str());
	texpath = texini.GetValue("AK", "bullet");
	LoadTex(Texture[AK47_BULLET], texpath.c_str());
	texpath = texini.GetValue("AK", "fire");
	LoadTex(Texture[AK47_FIRE], texpath.c_str());

	LoadTex(Texture[TREE_PLAIN], texini.GetValue("Sprite","tree"));	
/*
    while(!feof(fp)) {
        if(fscanf(fp, "%s", texpath) != 1){
            fprintf(stderr, "Error parsing %s!\n", fpath);
            fprintf(stderr, "Dump: %s %u\n",texpath);
            //exit(0x301);
        }

        if(!LoadTex(Texture[texid], texpath)){
            fprintf(stderr, "Failed loading %s!\n", texpath);
        }
        else {
            fprintf(stdout, "Loaded: %s ...... \n", texpath);
        }
		texid++;
    }

    fclose(fp); */
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
	GLuint shader,program; //shader id
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

#ifdef SHADER_PIPELINE
////////////////////////////////////////////////////////////
void VertexArray::Init(GLuint texid, GLuint size, GLuint program)
{
    _vbo = (0);
    _cbo =(0);
    _tbo = (0);
    _id = (0);
    _texid = texid;
    _program = program;
    _size = size;
}

////////////////////////////////////////////////////////////
VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_cbo);
	glDeleteBuffers(1, &_tbo);
	glDeleteVertexArrays(1, &_id);
}

////////////////////////////////////////////////////////////
void VertexArray::VertexPointer(GLvoid *ptr, GLuint size)
{
    if(!_vbo){
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, GL_STREAM_DRAW);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, GL_STREAM_DRAW);
    }
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

////////////////////////////////////////////////////////////
void VertexArray::ColorPointer(GLvoid *ptr, GLuint size)
{
    if(!_cbo){
        glGenBuffers(1, &_cbo);
        glBindBuffer(GL_ARRAY_BUFFER, _cbo);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, GL_STREAM_DRAW);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, _cbo);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, GL_STREAM_DRAW);
    }
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

////////////////////////////////////////////////////////////
void VertexArray::TexturePointer(GLvoid *ptr, GLuint size)
{
    if(!_tbo){
        glGenBuffers(1, &_tbo);
        glBindBuffer(GL_ARRAY_BUFFER, _tbo);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, GL_STREAM_DRAW);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, _tbo);
        glBufferData(GL_ARRAY_BUFFER, size, ptr, GL_STREAM_DRAW);
    }
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

////////////////////////////////////////////////////////////
void VertexArray::Bind(void)
{
	_vPosition = glGetAttribLocation(_program, "vPosition");
	_vColor = glGetAttribLocation(_program, "vColor");
	_vUV = glGetAttribLocation(_program, "vUV");

	glGenVertexArrays(1, &_id);
	glBindVertexArray(_id);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glVertexAttribPointer(_vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, _cbo);
		glVertexAttribPointer(_vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

		if(_tbo){
			glBindBuffer(GL_ARRAY_BUFFER, _tbo);
			glVertexAttribPointer(_vUV, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(_vUV);
		}

		glEnableVertexAttribArray(_vPosition);
		glEnableVertexAttribArray(_vColor);
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////
void VertexArray::Draw(GLuint ptype)
{
    glBindVertexArray(_id);
		if(_tbo){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _texid);
		}
		if(ptype)
			glDrawArrays(ptype, 0, _size);
		else
			glDrawArrays(GL_TRIANGLES, 0, _size);
	glBindVertexArray(0);
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

#endif //#ifdef SHADER_PIPELINE
