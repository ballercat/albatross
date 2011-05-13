#include "gameclient.h"
#include <string>
#include <cstdio>

struct MMOption{
	MMOption(float p_X, float p_Y, float p_W, float p_H)
	{
		pos			= glm::vec3(p_X, p_Y, 0);
		box.Left	= p_X;
		box.Right	= p_X+p_W;
		box.Top		= p_Y;
		box.Bottom	= p_Y+p_H;
	}

	MMOption()
	{
		pos		= glm::vec3(0,0,0);
	}

	glm::vec3	pos;
	sf::Rect<float>	box;
};

void MainClient::MainMenu()
{
    bool quit = false;
	bool ostart = false;
	bool oexit = false;

	float centerw = info.window_width/2;
	float centerh = info.window_height/2;

	MMOption optStart(centerw, centerh-100, 40, 20);
	MMOption optExit(centerw, centerh-80, 40, 20);

	glm::vec3 mouse = glm::vec3(0,0,0);

    while(!quit)
    {
        display->beginScene();{

			if(optStart.box.Contains(mouse.x, mouse.y)){
				display->drawText(optStart.pos, "[START]", 12);
				ostart = true;
			}
			else{
				display->drawText(optStart.pos, "START", 12);
				ostart = false;
			}

			if(optExit.box.Contains(mouse.x, mouse.y)){
				display->drawText(optExit.pos, "[EXIT]", 12);
				oexit = true;
			}
			else{
				display->drawText(optExit.pos, "EXIT", 12);
				oexit = false;
			}

			if(mInput->mState.mouse.left){
				if(ostart){
					display->setCursor(info.cursorfile.c_str());
					Run();
					display->Window->ShowMouseCursor(true);
					ostart = false;
				}
				else if(oexit){
					return;
				}
			}
			//display->setCursor(info.cursorfile.c_str());
            display->drawFPS();
        }display->endScene();
        
        //Handle Input
        mInput->Parse();

		mouse = mInput->InputState().mouse.pos;
		//printf("%f %f\n", mouse.x, mouse.y);
    }

}
