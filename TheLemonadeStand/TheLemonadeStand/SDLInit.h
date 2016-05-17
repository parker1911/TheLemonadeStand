//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

class SDLInit{

public:
	bool Setup();
	bool Cleanup();
	void Update();
};