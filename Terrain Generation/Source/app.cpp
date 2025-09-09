#ifndef APP_H
#define APP_H

class Application {

public:
	Application(int sWidth, int sHeight ) {
		SCREEN_HEIGHT = sHeight;
		SCREEN_WIDTH = sWidth;

		//Init();

	}



private:
	int SCREEN_HEIGHT;
	int SCREEN_WIDTH;

	
};

#endif // !APP_H
