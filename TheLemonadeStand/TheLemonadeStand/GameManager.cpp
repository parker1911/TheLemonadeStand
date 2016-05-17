#include <string>
#include "GameManager.h"
#include "SDLInit.h"
#include <SDL_image.h>



extern SDL_Window* window;
extern SDL_Surface* screenSurface;
SDL_Surface* gCompanyBackGround = NULL;
SDL_Surface* gCompanyT = NULL;
SDL_Surface* gCompanyA = NULL;
SDL_Surface* gCompanyP = NULL;
SDL_Surface* gCompanyGamming = NULL;
SDL_Surface* gTitleScreen = NULL;
SDL_Surface* gStand = NULL;
SDL_Surface* gPlayer = NULL;

SDL_Rect source;
SDL_Rect destination;

const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

bool quit = false;
int GAME_STATE = 0;

int COMPANY_SCREEN = 0;
int TITLE_SCREEN = 1;
int GAME_SCREEN = 2;

int GAME_SCENE = 0;
int STAND = 0;
int ROAD_W = 1;
int ROAD_N = 2;
int ROAD_E = 3;
int MARKET = 4;
int FARM = 5;
int GENERAL_STORE = 6;

int time = 0;


//Screen dimension constants
extern int SCREEN_WIDTH;		//TODO: currently not using...
extern int SCREEN_HEIGHT;		//TODO: currently not using...

static SDLInit sdlInit;

bool GameManager::Init(){
	bool initSuccess = sdlInit.Setup();
	return initSuccess;
}

bool GameManager::Cleanup(){
	return sdlInit.Cleanup();
}

//The application time based timer
class LTimer
{
public:
	//Initializes variables
	LTimer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time
	Uint32 getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	Uint32 mStartTicks;

	//The ticks stored when the timer was paused
	Uint32 mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};
LTimer::LTimer()
{
	//Initialize the variables
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}

void LTimer::start()
{
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
	//Stop the timer
	mStarted = false;

	//Unpause the timer
	mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
	//If the timer is running and isn't already paused
	if (mStarted && !mPaused)
	{
		//Pause the timer
		mPaused = true;

		//Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause()
{
	//If the timer is running and paused
	if (mStarted && mPaused)
	{
		//Unpause the timer
		mPaused = false;

		//Reset the starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		//Reset the paused ticks
		mPausedTicks = 0;
	}
}

Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if (mStarted)
	{
		//If the timer is paused
		if (mPaused)
		{
			//Return the number of ticks when the timer was paused
			time = mPausedTicks;
		}
		else
		{
			//Return the current time minus the start time
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}

bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
	return mStarted;
}

bool LTimer::isPaused()
{
	//Timer is running and paused
	return mPaused && mStarted;
}



//TODO: Add deltatime later...
void GameManager::Update()
{
	sdlInit.Update();
	if (GameManager::loadMedia() == false)
	{
		printf("loadMedia error!\n");
	}

	//The frames per second timer
	LTimer fpsTimer;
	//The frames per second cap timer
	LTimer capTimer;


	//Start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();


	// Part of the screen we want to draw the sprite to

	destination.x = 150;
	destination.y = 400;
	destination.w = 65;
	destination.h = 44;

	//Main loop flag


	//Event handler
	SDL_Event e;
	while (!quit)
	{


		//Start cap timer
		capTimer.start();

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				//Select surfaces based on key press
				switch (e.key.keysym.sym)
				{
				case SDLK_UP:
					--destination.y;
					
					break;

				case SDLK_DOWN:
					++destination.y;
					
					break;

				case SDLK_LEFT:
					 --destination.x;
					break;

				case SDLK_RIGHT:
					++destination.x;
					break;

				default:
					
					break;
				}
			}




		}
		//Calculate and correct fps
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
		if (avgFPS > 2000000)
		{
			avgFPS = 0;
		}
		//TODO: this needs to be fixed(the fps code) printf("avgFPS: %d\n", avgFPS);


		if (GAME_STATE == COMPANY_SCREEN)
		{
			companyScreen();
		}
		else if (GAME_STATE == TITLE_SCREEN)
		{
			titleScreen();
		}
		else if (GAME_STATE == GAME_SCREEN)
		{
			gameScreen();
		}
		SDL_UpdateWindowSurface(window);

		++countedFrames;
		//If frame finished early
		int frameTicks = capTimer.getTicks();
		if (frameTicks < SCREEN_TICK_PER_FRAME)
		{
			//Wait remaining time
			//TODO: this needs to be fixed(the fps code) SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
		}
		
	}
}

SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, NULL);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

bool GameManager::loadMedia()
{//Loading success flag
	bool success = true;

	//Load PNG surface
	gCompanyBackGround = loadSurface("images/companyBackGround.png");
	if (gCompanyBackGround == NULL)
	{
		printf("companyScreen.png!\n");
		success = false;
	}
	gCompanyT = loadSurface("images/companyT.png");
	if (gCompanyT == NULL)
	{
		printf("companyT.png!\n");
		success = false;
	}
	gCompanyA = loadSurface("images/companyA.png");
	if (gCompanyA == NULL)
	{
		printf("companyA.png!\n");
		success = false;
	}
	gCompanyP = loadSurface("images/companyP.png");
	if (gCompanyP == NULL)
	{
		printf("companyP.png!\n");
		success = false;
	}
	gCompanyGamming = loadSurface("images/companyGamming.png");
	if (gCompanyGamming == NULL)
	{
		printf("companyGamming.png!\n");
		success = false;
	}
	gTitleScreen = loadSurface("images/titleScreen.png");
	if (gTitleScreen == NULL)
	{
		printf("titleScreen.png!\n");
		success = false;
	}
	gStand = loadSurface("images/stand.png");
	if (gStand == NULL)
	{
		printf("stand.png!\n");
		success = false;
	}
	gPlayer = loadSurface("images/player2.png");
	if (gPlayer == NULL)
	{
		printf("player.png!\n");
		success = false;
	}

	return success;
}

void GameManager::companyScreen()
{
	if (time < 1000)
	{
		//Apply the PNG image
		SDL_BlitSurface(gCompanyBackGround, NULL, screenSurface, NULL);
		time++;
	}
	else if (time >= 1000 && time < 2000)
	{
		//Apply the PNG image
		SDL_BlitSurface(gCompanyT, NULL, screenSurface, NULL);
		time++;
	}
	else if (time >= 2000 && time < 3000)
	{
		//Apply the PNG image
		SDL_BlitSurface(gCompanyA, NULL, screenSurface, NULL);
		time++;
	}
	else if (time >= 3000 && time < 4000)
	{
		//Apply the PNG image
		SDL_BlitSurface(gCompanyP, NULL, screenSurface, NULL);
		time++;
	}
	else if (time >= 4000 && time < 6000)
	{
		//Apply the PNG image
		SDL_BlitSurface(gCompanyGamming, NULL, screenSurface, NULL);
		time++;
	}
	else{
		GAME_STATE = TITLE_SCREEN;
		time = 0;
		//Free loaded image
		SDL_FreeSurface(gCompanyBackGround);
		gCompanyBackGround = NULL;
		SDL_FreeSurface(gCompanyT);
		gCompanyT = NULL;
		SDL_FreeSurface(gCompanyA);
		gCompanyA = NULL;
		SDL_FreeSurface(gCompanyP);
		gCompanyP = NULL;
		SDL_FreeSurface(gCompanyGamming);
		gCompanyGamming = NULL;
	}
}

void GameManager::titleScreen()
{
	if (time < 2000)
	{
		//Apply the PNG image
		SDL_BlitSurface(gTitleScreen, NULL, screenSurface, NULL);
		time++;
	}
	else{

		GAME_STATE = GAME_SCREEN;
		time = 0;
		SDL_FreeSurface(gTitleScreen);
		gTitleScreen = NULL;
	}
}

void GameManager::gameScreen()
{
	//// Part of the bitmap that we want to draw

	//source.x = 150;
	//source.y = 370;
	//source.w = 115;
	//source.h = 150;

	//// Part of the screen we want to draw the sprite to

	//destination.x = 150;
	//destination.y = 400;
	//destination.w = 65;
	//destination.h = 44;

	// Part of the bitmap that we want to draw

	source.x = 0;
	source.y = 0;
	source.w = 1150;
	source.h = 1500;





	if (GAME_SCENE == STAND)
	{	SDL_SetColorKey(gPlayer, SDL_TRUE, SDL_MapRGB(gPlayer->format,0xFF, 0,  0xFF));	
		SDL_BlitSurface(gStand, NULL, screenSurface, NULL);
		SDL_SetColorKey(gPlayer, SDL_TRUE, SDL_MapRGB(gPlayer->format,0xFF, 0,  0xFF));
		SDL_BlitSurface(gPlayer, &source, screenSurface, &destination);
	}
}

	

