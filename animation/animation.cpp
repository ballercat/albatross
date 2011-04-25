#include "animation.h"
#include "assets.h"
#include <cstdio>

using namespace animation;
using namespace std;

////////////////////////////////////////////////////////////
Skeletal_t::Skeletal_t(float plen)
{
	len = plen;

	glm::vec3 vertex = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec4 color = glm::vec4(BONECOLOR_PLAIN);


	count = 0;
}

////////////////////////////////////////////////////////////
Bone_t* Skeletal_t::addBone(float plen, Bone_t* parent)
{
	Bone_t nbone;

	if(parent){
		//Parent exists create the new bone
		//the parents front joint
		nbone.len = plen;
		nbone.A.a = 0.0f;
		nbone.A.p = parent->front->p;
		nbone.B.a = 0.0f;
		nbone.B.p = parent->front->p;
		nbone.B.p.x += plen;

		nbone.front = &nbone.B;
		nbone.back = &nbone.A;

		nbone.id = count;
	}
	else {
		//Main bone(backbone?)
		nbone.len = plen;

		nbone.A.a = 0.0f;
		nbone.A.p = glm::vec3(0.0f,0.0f,0.0f);
		nbone.B.a = 0.0f;
		nbone.B.p = glm::vec3(0.0f,nbone.len,0.0f);

		nbone.front = &nbone.B;
		nbone.back = &nbone.A;

		nbone.id = count;
	}

	count++;
	_bones.push_back(nbone);

	vdata.push_back(nbone.A.p);
	vdata.push_back(nbone.B.p);
	cdata.push_back(glm::vec4(BONECOLOR_PLAIN));
	cdata.push_back(glm::vec4(BONECOLOR_PLAIN));

	return &_bones[count-1];
}

////////////////////////////////////////////////////////////
void Skeletal_t::removeBone(Bone_t& pbone)
{

}

////////////////////////////////////////////////////////////
void Skeletal_t::buildDefaultBody()
{
	//Backbone
	Bone_t back(glm::vec3(0.0f,10.0f,0.0f), glm::vec3(0.0f,25.0f,0.0f), 25.0f, count);
	count++;

	//head
	Bone_t head(&back,glm::vec3(0.0f,35.0f,0.0f), glm::vec3(0.0f,10.0f,0.0f), count);
	count++;

	//arms
	Bone_t left(glm::vec3(0.0f,30.0f,0.0f), glm::vec3(12.0f,0.0f,0.0f), 12.0f, count);
	count++;
	Bone_t leftfore(glm::vec3(12.0f,30.0f,0.0f), glm::vec3(7.0f,0.0f,0.0f), 7.0f, count);
	count++;

	Bone_t right(glm::vec3(0.0f,30.0f,0.0f), glm::vec3(-12.0f,0.0f,0.0f), 12.0f, count);
	count++;
	Bone_t rightfore(glm::vec3(-12.0f,30.0f,0.0f), glm::vec3(-7.0f,0.0f,0.0f), 7.0f, count);
	count++;

	printf("Count: %i\n",count);
	_bones.push_back(back);
	_bones.push_back(head);
	_bones[0].child.push_back(&_bones[1]);
	_bones.push_back(left); _bones.push_back(leftfore);
	_bones.push_back(right); _bones.push_back(rightfore);
}

////////////////////////////////////////////////////////////
void Skeletal_t::draw(void)
{
	Bone_t::_joint_t *A, *B;

	glPushMatrix();
	glScalef(3.0f,3.0f,0.0f);


	for(size_t k =0;k<_bones.size();k++){

		A = &_bones[k].A;
		B = &_bones[k].B;
/*
		glBegin(GL_LINES);{
			glVertex3f( A->p.x, A->p.y, 0.0f );
			glVertex3f( B->p.x, B->p.y, 0.0f );
		}glEnd(); */

		glPushMatrix();{
			glTranslatef( A->p.x, A->p.y, 0.0f );
			glRotatef(A->a, 0.0f, 0.0f, -1.0f);
			glColor4f(BONECOLOR_PLAIN);
			glBegin(GL_LINES);{
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( B->p.x, B->p.y, 0.0f );
			}glEnd();

			glPointSize(3.0f);
			glColor4f(JOINTCOLOR_PLAIN);
			glBegin(GL_POINTS);{
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( B->p.x, B->p.y, 0.0f );
			}glEnd();

		}glPopMatrix();
	}
/*
	for(size_t k =0;k<_bones.size();k++){

		A = &_bones[k].A;
		B = &_bones[k].B;

		glColor4f(JOINTCOLOR_PLAIN);
		glPointSize(4.0f);
		glBegin(GL_POINTS);{
			glVertex3f( A->p.x, A->p.y, 0.0f );
			glVertex3f( B->p.x, B->p.y, 0.0f );
		}glEnd();

		glColor3f(0.0f,0.0f,5.0f);
		glPointSize(2.0f);
		glBegin(GL_POINTS);{
			glVertex3f( _bones[k].center.x, _bones[k].center.y, 0.0f );
		}glEnd();
	}*/

	glColor3f(0.0f,1.0f,0.0f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(-25.0f,0.0f);
		glVertex2f(-25.0f,50.0f);
		glVertex2f(25.0f,50.0f);
		glVertex2f(25.0f,0.0f);
	glEnd();
	glPopMatrix();
}

////////////////////////////////////////////////////////////
void Skeletal_t::step(float pdt)
{

}
