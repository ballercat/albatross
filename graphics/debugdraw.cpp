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

#ifdef NDEBUG

#include "gameclient.h"

//Nearly all bellow code is from chipmunk example code

#define LINE_COLOR 0.0f, 1.0f, 0.0f
#define COLLISION_COLOR 1.0f, 0.0f, 0.0f
#define BODY_COLOR 0.0f, 0.0f, 1.0f

static void
glColor_from_pointer(void *ptr)
{
	unsigned long val = (long)ptr;

	// hash the pointer up nicely
	val = (val+0x7ed55d16) + (val<<12);
	val = (val^0xc761c23c) ^ (val>>19);
	val = (val+0x165667b1) + (val<<5);
	val = (val+0xd3a2646c) ^ (val<<9);
	val = (val+0xfd7046c5) + (val<<3);
	val = (val^0xb55a4f09) ^ (val>>16);

//	GLfloat v = (GLfloat)val/(GLfloat)ULONG_MAX;
//	v = 0.95f - v*0.15f;
//
//	glColor3f(v, v, v);

	GLubyte r = (val>>0) & 0xFF;
	GLubyte g = (val>>8) & 0xFF;
	GLubyte b = (val>>16) & 0xFF;

	GLubyte max = r>g ? (r>b ? r : b) : (g>b ? g : b);

	const int mult = 127;
	const int add = 63;
	r = (r*mult)/max + add;
	g = (g*mult)/max + add;
	b = (b*mult)/max + add;

	glColor3ub(r, g, b);
}

static void
glColor_for_shape(cpShape *shape, cpSpace *space)
{
    cpBody *body = shape->body;
    if(body){
        if(body->node.next){
            GLfloat v = 0.25f;
            glColor3f(v,v,v);
            return;
        } else if(body->node.idleTime > space->sleepTimeThreshold) {
            GLfloat v = 0.9f;
            glColor3f(v,v,v);
            return;
        }
    }

    glColor_from_pointer(shape);
}
static const GLfloat circleVAR[] = {
	 0.0000f,  1.0000f,
	 0.2588f,  0.9659f,
	 0.5000f,  0.8660f,
	 0.7071f,  0.7071f,
	 0.8660f,  0.5000f,
	 0.9659f,  0.2588f,
	 1.0000f,  0.0000f,
	 0.9659f, -0.2588f,
	 0.8660f, -0.5000f,
	 0.7071f, -0.7071f,
	 0.5000f, -0.8660f,
	 0.2588f, -0.9659f,
	 0.0000f, -1.0000f,
	-0.2588f, -0.9659f,
	-0.5000f, -0.8660f,
	-0.7071f, -0.7071f,
	-0.8660f, -0.5000f,
	-0.9659f, -0.2588f,
	-1.0000f, -0.0000f,
	-0.9659f,  0.2588f,
	-0.8660f,  0.5000f,
	-0.7071f,  0.7071f,
	-0.5000f,  0.8660f,
	-0.2588f,  0.9659f,
	 0.0000f,  1.0000f,
	 0.0f, 0.0f, // For an extra line to see the rotation.
};
static const int circleVAR_count = sizeof(circleVAR)/sizeof(GLfloat)/2;
static void
drawCircleShape(cpBody *body, cpCircleShape *circle, cpSpace *space)
{
	glVertexPointer(2, GL_FLOAT, 0, circleVAR);

	glPushMatrix(); {
		cpVect center = circle->tc;
		glTranslatef(center.x, center.y, 0.0f);
		glRotatef(body->a*180.0f/M_PI, 0.0f, 0.0f, 1.0f);
		glScalef(circle->r, circle->r, 1.0f);

		if(!circle->shape.sensor){
			glColor_for_shape((cpShape *)circle, space);
			glDrawArrays(GL_TRIANGLE_FAN, 0, circleVAR_count - 1);
		}

		glColor3f(LINE_COLOR);
		glDrawArrays(GL_LINE_STRIP, 0, circleVAR_count);
	} glPopMatrix();
}
static const GLfloat pillVAR[] = {
	 0.0000f,  1.0000f, 1.0f,
	 0.2588f,  0.9659f, 1.0f,
	 0.5000f,  0.8660f, 1.0f,
	 0.7071f,  0.7071f, 1.0f,
	 0.8660f,  0.5000f, 1.0f,
	 0.9659f,  0.2588f, 1.0f,
	 1.0000f,  0.0000f, 1.0f,
	 0.9659f, -0.2588f, 1.0f,
	 0.8660f, -0.5000f, 1.0f,
	 0.7071f, -0.7071f, 1.0f,
	 0.5000f, -0.8660f, 1.0f,
	 0.2588f, -0.9659f, 1.0f,
	 0.0000f, -1.0000f, 1.0f,

	 0.0000f, -1.0000f, 0.0f,
	-0.2588f, -0.9659f, 0.0f,
	-0.5000f, -0.8660f, 0.0f,
	-0.7071f, -0.7071f, 0.0f,
	-0.8660f, -0.5000f, 0.0f,
	-0.9659f, -0.2588f, 0.0f,
	-1.0000f, -0.0000f, 0.0f,
	-0.9659f,  0.2588f, 0.0f,
	-0.8660f,  0.5000f, 0.0f,
	-0.7071f,  0.7071f, 0.0f,
	-0.5000f,  0.8660f, 0.0f,
	-0.2588f,  0.9659f, 0.0f,
	 0.0000f,  1.0000f, 0.0f,
};
static const int pillVAR_count = sizeof(pillVAR)/sizeof(GLfloat)/3;
static void
drawSegmentShape(cpBody *body, cpSegmentShape *seg, cpSpace *space)
{
	cpVect a = seg->ta;
	cpVect b = seg->tb;

	if(seg->r){
		glVertexPointer(3, GL_FLOAT, 0, pillVAR);
		glPushMatrix(); {
			cpVect d = cpvsub(b, a);
			cpVect r = cpvmult(d, seg->r/cpvlength(d));

			const GLfloat matrix[] = {
				 r.x, r.y, 0.0f, 0.0f,
				-r.y, r.x, 0.0f, 0.0f,
				 d.x, d.y, 0.0f, 0.0f,
				 a.x, a.y, 0.0f, 1.0f,
			};
			glMultMatrixf(matrix);

			if(!seg->shape.sensor){
				glColor_for_shape((cpShape *)seg, space);
				glDrawArrays(GL_TRIANGLE_FAN, 0, pillVAR_count);
			}

			glColor3f(LINE_COLOR);
			glDrawArrays(GL_LINE_LOOP, 0, pillVAR_count);
		} glPopMatrix();
	} else {
		glColor3f(LINE_COLOR);
		glBegin(GL_LINES); {
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		} glEnd();
	}
}
static void
drawPolyShape(cpBody *body, cpPolyShape *poly, cpSpace *space)
{
    int count = poly->numVerts;

#if CP_USE_DOUBLES
    glVertexPointer(2, GL_DOUBLE, 0, poly->tVerts);
#else
    glVertexPointer(2, GL_FLOAT, 0, poly->tVerts);
#endif

    if(!poly->shape.sensor){
        glColor_for_shape((cpShape*)poly, space);
        glDrawArrays(GL_TRIANGLE_FAN, 0, count);
    }

    glColor3f(LINE_COLOR);
    glDrawArrays(GL_LINE_LOOP, 0, count);
}

static const GLfloat springVAR[] = {
	0.00f, 0.0f,
	0.20f, 0.0f,
	0.25f, 3.0f,
	0.30f,-6.0f,
	0.35f, 6.0f,
	0.40f,-6.0f,
	0.45f, 6.0f,
	0.50f,-6.0f,
	0.55f, 6.0f,
	0.60f,-6.0f,
	0.65f, 6.0f,
	0.70f,-3.0f,
	0.75f, 6.0f,
	0.80f, 0.0f,
	1.00f, 0.0f,
};
static const int springVAR_count = sizeof(springVAR)/sizeof(GLfloat)/2;
static void
drawSpring(cpDampedSpring *spring, cpBody *body_a, cpBody *body_b)
{
	cpVect a = cpvadd(body_a->p, cpvrotate(spring->anchr1, body_a->rot));
	cpVect b = cpvadd(body_b->p, cpvrotate(spring->anchr2, body_b->rot));

	glPointSize(5.0f);
	glBegin(GL_POINTS); {
		glVertex2f(a.x, a.y);
		glVertex2f(b.x, b.y);
	} glEnd();

	cpVect delta = cpvsub(b, a);

	glVertexPointer(2, GL_FLOAT, 0, springVAR);
	glPushMatrix(); {
		GLfloat x = a.x;
		GLfloat y = a.y;
		GLfloat cos = delta.x;
		GLfloat sin = delta.y;
		GLfloat s = 1.0f/cpvlength(delta);

		const GLfloat matrix[] = {
				 cos,    sin, 0.0f, 0.0f,
			-sin*s,  cos*s, 0.0f, 0.0f,
				0.0f,   0.0f, 1.0f, 0.0f,
					 x,      y, 0.0f, 1.0f,
		};

		glMultMatrixf(matrix);
		glDrawArrays(GL_LINE_STRIP, 0, springVAR_count);
	} glPopMatrix();
}
static void
drawConstraint(cpConstraint *constraint)
{
	cpBody *body_a = constraint->a;
	cpBody *body_b = constraint->b;

	const cpConstraintClass *klass = constraint->klass;
	if(klass == cpPinJointGetClass()){
		cpPinJoint *joint = (cpPinJoint *)constraint;

		cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
		cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));

		glPointSize(5.0f);
		glBegin(GL_POINTS); {
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		} glEnd();

		glBegin(GL_LINES); {
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		} glEnd();
	} else if(klass == cpSlideJointGetClass()){
		cpSlideJoint *joint = (cpSlideJoint *)constraint;

		cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
		cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));

		glPointSize(5.0f);
		glBegin(GL_POINTS); {
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		} glEnd();

		glBegin(GL_LINES); {
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		} glEnd();
	} else if(klass == cpPivotJointGetClass()){
		cpPivotJoint *joint = (cpPivotJoint *)constraint;

		cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
		cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));

		glPointSize(10.0f);
		glBegin(GL_POINTS); {
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		} glEnd();
	} else if(klass == cpGrooveJointGetClass()){
		cpGrooveJoint *joint = (cpGrooveJoint *)constraint;

		cpVect a = cpvadd(body_a->p, cpvrotate(joint->grv_a, body_a->rot));
		cpVect b = cpvadd(body_a->p, cpvrotate(joint->grv_b, body_a->rot));
		cpVect c = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));

		glPointSize(5.0f);
		glBegin(GL_POINTS); {
			glVertex2f(c.x, c.y);
		} glEnd();

		glBegin(GL_LINES); {
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		} glEnd();
	} else if(klass == cpDampedSpringGetClass()){
		drawSpring((cpDampedSpring *)constraint, body_a, body_b);
	} else {
//		printf("Cannot draw constraint\n");
	}
}
static
void ChipmunkToGame(cpShape *shape, cpSpace *space)
{
    cpBody *body = shape->body;
    switch(shape->klass->type){
        case CP_CIRCLE_SHAPE:
            drawCircleShape(body, (cpCircleShape*)shape, space);
            break;
        case CP_SEGMENT_SHAPE:
            drawSegmentShape(body, (cpSegmentShape*)shape, space);
            break;
        case CP_POLY_SHAPE:
            drawPolyShape(body, (cpPolyShape*)shape, space);
            break;
        case CP_NUM_SHAPES:
        default:
            exit(0xFF00FF);
    }
}
void MainClient::DebugDrawPhysics(void)
{
	glEnable(GL_VERTEX_ARRAY);
	
    cpSpace *space = mPhysics->GetWorldHandle();

    cpSpaceHashEach(space->staticShapes,(cpSpaceHashIterator)ChipmunkToGame, space);

    cpSpaceHashEach(space->activeShapes,(cpSpaceHashIterator)ChipmunkToGame, space);

    glColor3f(0.5f, 1.0f, 0.5f);

    cpArray *constraints = space->constraints;
    for(int i = 0,count = constraints->num; i < count; i++)
        drawConstraint((cpConstraint *)constraints->arr[i]);

    glPointSize(3.0f);
    glBegin(GL_POINTS); {
        cpArray *arbiters = space->arbiters;
        for(int i=0; i<arbiters->num; i++){
            cpArbiter *arb = (cpArbiter*)arbiters->arr[i];

            glColor3f(COLLISION_COLOR);
            for(int i=0; i<arb->numContacts; i++){
                cpVect v = arb->contacts[i].p;
                glVertex2f(v.x, v.y);
            }
        }
    } glEnd();
	
	glDisable(GL_VERTEX_ARRAY);
}

#endif
