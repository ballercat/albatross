#include "display.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>

Display::Display(const char* name)
{
	Window = new sf::RenderWindow(sf::VideoMode(1024,600),name,sf::Style::Close);
	//Window->PreserveOpenGLStates(true);

    camera = glm::vec3(0,0,0);
	cam_translate = glm::vec3(0,0,0);
    zoom = glm::vec3(1,1,1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);

    GLenum err = glewInit();
    if(GLEW_OK != err){
        exit(0x20202);
    }

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnable(GL_TEXTURE_RECTANGLE_NV);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glViewport(0,0,1024,600);

	/* //Deprecated
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-512,512,-300,300,-12.0,12.0);
	*/

	//matrices
	MVP = glm::mat4();
	View = glm::mat4(1.0f);
	Model = glm::mat4(1.0f);
	Projection = glm::mat4(1.0f);
	Projection = glm::ortho(-512.0f,512.0f,-300.0f,300.0f,-1.0f,1.0f);

	MVP = Model * View * Projection;
	gMatrix()[0] = Projection;
	gMatrix()[1] = Model;
	gMatrix()[2] = View;
	gMatrix()[3] = MVP;

    _text.SetText("NULL");
    _text.SetPosition(0,0);
    _text.SetColor(sf::Color(0,0,0));

    _fps.SetText("0");
    _fps.SetPosition(0,0);
    _fps.SetColor(sf::Color(0,0,0));
    _fps.SetSize(10);

	FrameTime = 0.0f;
	_skip_frames_counter = 0;
	_draw_time = _timer.GetElapsedTime();
	SkipFrames = 2;
}

Display::~Display()
{
	if(Window){
		delete Window;
	}
}

void Display::resize(int w, int h)
{

}

void Display::setCursor(const char *fpath)
{
    sf::Image rawimage;
    if(!rawimage.LoadFromFile(fpath)){
        fprintf(stderr,"Could not load cursor!\n");
        exit(0x205);
    }


	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture[0xFF]);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rawimage.GetWidth(), rawimage.GetHeight(),
                   0, GL_RGBA, GL_UNSIGNED_BYTE, rawimage.GetPixelsPtr());

    cursor.width = rawimage.GetWidth();
    cursor.height = rawimage.GetHeight();
    cursor.off = glm::vec3(0,0,0);
    cursor.scale = glm::vec3(1,1,0);
    cursor.textureid = Texture[0xFF];
	cursor.imgd = glm::vec2(10,10);
	cursor.Build();

    Window->ShowMouseCursor(false);
}
void Display::enable2D(void)
{
    GLint iViewport[4];

    glGetIntegerv(GL_VIEWPORT, iViewport);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(iViewport[0], iViewport[0] + iViewport[2],
            iViewport[1] + iViewport[3], iViewport[1], -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

}
void Display::disable2D(void)
{
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void Display::beginScene(GLuint shader)
{
	_draw_time = _timer.GetElapsedTime();
    glClear(GL_COLOR_BUFFER_BIT);

    /* //Deprecated:
    glLoadIdentity();
    glTranslated(-camera.x,-camera.y,0.0f);
    glScalef(zoom.x,zoom.y,zoom.z);
	*/

	//Camera and zoom transformations
	View = glm::translate(glm::mat4(1.0f), -camera);
	View = glm::scale(View, zoom);

	MVP = Model * View * Projection;
	gMatrix()[0] = Projection;
	gMatrix()[1] = Model;
	gMatrix()[2] = View;
	gMatrix()[3] = MVP;

	gTransform()[0] = camera;

	if(shader != 0){
		glUseProgram(shader);
		glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	}
}
void Display::endScene(void)
{
    glFlush();
/*
	glUseProgram(0);

	if(SkipFrames){
		if(_skip_frames_counter > SkipFrames){
			Window->Display();
			_skip_frames_counter = 0;
		}
		else {
			_skip_frames_counter++;
		}
	}else {
		Window->Display();
	}

*/
	Window->Display();
	float t = _timer.GetElapsedTime();
	FrameTime = t - _draw_time;
	_draw_time = t;

	if(FrameTime > (1.0f/60.0f)){
		SkipFrames--;
		if(SkipFrames < 0)
			SkipFrames = 0;
	}
	else{
		if(!_skip_frames_counter)
			SkipFrames++;
	}

	fps = FrameTime;
    char a[12];
    sprintf(a,"%d",int(1.0f/fps));
    _fps.SetText(a);
}

void Display::drawSprite(Sprite* sprite)
{

}
void Display::drawPolygon(GLvoid* verts,size_t size)
{
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, size);
}
void Display::drawCircle()
{
}
void Display::drawText(glm::vec3 pos,const char* text,size_t size)
{
    _text.SetText(text);
    _text.SetPosition(pos.x,pos.y);
    _text.SetSize(size);
    Window->Draw(_text);
}
void Display::drawLine(glm::vec3 v1,glm::vec3 v2,glm::vec3 colour)
{
    glLineWidth(2.0f);
    glColor3f(colour.x,colour.y,colour.z);
    glBegin(GL_LINES);
        glVertex3f(v1.x,v1.y,v1.z);
        glVertex3f(v2.x,v2.y,v2.z);
    glEnd();
    glLineWidth(1.0f);
}

void Display::drawArray(GLvoid *v, GLvoid *t, GLvoid *c, GLenum size,GLenum type, GLuint texid)
{
    if(t){
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        //glEnable(GL_TEXTURE_2D);
        glTexCoordPointer(2, GL_FLOAT, 0, t);
        glBindTexture(GL_TEXTURE_2D, texid);
    }
    if(c){
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, c);
    }

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, v);
    glDrawArrays(type, 0, size*3);

    glDisableClientState(GL_VERTEX_ARRAY);

    if(t){
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        //glDisable(GL_TEXTURE_2D);
    }
    if(c)
        glDisableClientState(GL_COLOR_ARRAY);

}
