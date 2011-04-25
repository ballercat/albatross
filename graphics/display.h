#include <GL/glew.h>
#include "sprite.h"
#include <SFML/Graphics.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

#define SHADER_PIPELINE 1
#ifdef FIXED_PIPELINE
	#undef FIXED_PIPELINE
#endif

#include "assets.h"

/* TODO: create separate renderrers */
////////////////////////////////////////////////////////////
/// Graphics/Windowing class
////////////////////////////////////////////////////////////
class Display
{
public:
	///Default Constructor
	Display(const char* name="bowhead");
	///Deconstructor
	virtual ~Display(void);

public:
	///Windowing
    void resize(int w,int h);
    void setCursor(const char *fpath);

public:
	///Scene managment
    void enable2D(void);
    void disable2D(void);
	void beginScene(GLuint shader=0);
	void endScene();

public:
	///Built in rendering functions
	void drawSprite(Sprite* sprite);
	void drawPolygon(GLvoid* verts,size_t size);
	void drawArray(GLvoid* v,GLvoid *t, GLvoid *c,GLuint size, GLuint type, GLuint texid);
	void drawCircle();
	void drawText(glm::vec3 pos,const char* text,size_t size=12);
	void drawLine(glm::vec3 v1, glm::vec3 v2,glm::vec3 colour=glm::vec3(0,0,0));

public: ///Inline helper functions
    ///Renver fps
	inline void drawFPS(void){
        Window->Draw(_fps);
    }

    ///Convert screen vector to world coordinates
    inline glm::vec3 stw(glm::vec3 v) {
		glm::vec3 ret = glm::unProject( glm::vec3(v.x,600-v.y, 0.0f) ,
										Model,
										Projection,
										glm::vec4(0,0,1024,600) );
		ret.z = 0;
		return ret;
    }

	inline float* Matrix(void){
		return glm::value_ptr(MVP);
	}

    //Data
	sf::RenderWindow	*Window;
	GLuint Texture[256];

	Sprite cursor;
	glm::vec3 camera;
	glm::vec3 cam_translate;
	glm::vec3 zoom;
	glm::mat4 MVP;
	float FrameTime;
	int SkipFrames;

private:
	sf::String 	_text;
	sf::String  _fps;
	GLfloat		fps;

	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	sf::Clock _timer;
	float _draw_time;
	unsigned _skip_frames_counter;
};


