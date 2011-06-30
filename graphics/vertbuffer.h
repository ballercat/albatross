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

#ifndef VERTBUFFER_HEADER_GUARD
#define VERTBUFFER_HEADER_GUARD

#include<GL/glew.h>
#include<glm/glm.hpp>

#define BUFFER_OFFSET(i)	((char*)NULL + (i))

struct Vertex{
	Vertex():
	v(glm::vec3(0)),
	t(glm::vec2(0)),
	c(glm::vec4(0))
	{

	}

	glm::vec3	v;
	glm::vec2	t;
	glm::vec4	c;
};

class GenericBuffer
{
public:
    GenericBuffer() :
        pProgram(0),
        pArray(0),
        pBuffer(0),
        pIndex(0),
        pVertexPos(0),
        pTexturePos(0),
        pColorPos(0)
    {
    }

    GenericBuffer(const GenericBuffer& p_Copy):
    pProgram(p_Copy.pProgram),
    pArray(p_Copy.pArray),
    pBuffer(p_Copy.pBuffer),
    pIndex(p_Copy.pIndex),
    pVertexPos(p_Copy.pVertexPos),
    pTexturePos(p_Copy.pTexturePos),
    pColorPos(p_Copy.pColorPos)
    {
    }

    GenericBuffer& operator=(const GenericBuffer& p_Copy)
    {
		if(pBuffer){
			glDeleteBuffers(1, &pBuffer);
			glDeleteBuffers(1, &pIndex);
			glDeleteVertexArrays(1, &pArray);
		}
        pProgram = p_Copy.pProgram;
        pArray = p_Copy.pArray;
        pBuffer = p_Copy.pBuffer;
        pIndex = p_Copy.pIndex;
        pVertexPos = p_Copy.pVertexPos;
        pTexturePos = p_Copy.pTexturePos;
        pColorPos = p_Copy.pColorPos;
    }

    GenericBuffer(Vertex *p_Data, GLushort *p_Index, GLuint indx_size, GLuint vert_size):
    pProgram(0),
    pArray(0),
    pBuffer(0),
    pIndex(0),
    pVertexPos(0),
    pTexturePos(0),
    pColorPos(0)
    {
        glGenVertexArrays(1, &pArray);
        glBindVertexArray(pArray);

        glGenBuffers(1, &pBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, pBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vert_size, p_Data, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &pIndex);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vertex)*vert_size, p_Index, GL_DYNAMIC_DRAW);

        glBindVertexArray(0);
    }

    void SetShader(GLuint p_Program)
    {
        //NOTE: remember to disable vertex arrays if program is set
        pProgram = p_Program;

        pVertexPos = glGetAttribLocation(pProgram, "vPosition");
        pTexturePos = glGetAttribLocation(pProgram, "vUV");
        pColorPos = glGetAttribLocation(pProgram, "vColor");

        glBindVertexArray(pArray);
        glBindBuffer(GL_ARRAY_BUFFER, pBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndex);

        glVertexAttribPointer(pVertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                BUFFER_OFFSET(0));
        glVertexAttribPointer(pTexturePos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                BUFFER_OFFSET(sizeof(glm::vec3)));
        glVertexAttribPointer(pColorPos, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                BUFFER_OFFSET(sizeof(glm::vec3)+sizeof(glm::vec2)));

        glEnableVertexAttribArray(pVertexPos);
        glEnableVertexAttribArray(pTexturePos);
        glEnableVertexAttribArray(pColorPos);

        glBindVertexArray(0);
    }

	inline void Generate()
	{
		glGenVertexArrays(1, &pArray);
		glBindVertexArray(pArray);

		glGenBuffers(1, &pBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, pBuffer);

		glGenBuffers(1, &pIndex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndex);

		glBindVertexArray(0);
	}

    inline void Bind()
    {
        glBindVertexArray(pArray);
    }

	inline void UpdateData(Vertex *p_Data, GLushort *p_Indx, GLuint size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, pBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*size, p_Data, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vertex)*size, p_Indx, GL_DYNAMIC_DRAW);
	}

public:
    GLuint  pProgram;
    GLuint  pArray;
    GLuint  pBuffer;
    GLuint  pIndex;

    GLuint  pVertexPos;
    GLuint  pTexturePos;
    GLuint  pColorPos;
};

#endif
