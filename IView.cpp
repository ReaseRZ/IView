#include "stdafx.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Screen dimension
int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

SDL_Rect rect;
//The window renderer
SDL_Renderer* gRenderer = NULL;

//The Texture render
SDL_Texture* gTexture = NULL;

std::string formatImage;

bool init()
{
	//Initialization flag
	SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
	SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Photo Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = NULL;
				if (formatImage == "jpg")
					imgFlags = IMG_INIT_JPG;
				if (formatImage == "png")
					imgFlags = IMG_INIT_PNG;
				if (formatImage == "avif")
					imgFlags = IMG_INIT_AVIF;
				if (formatImage == "webp")
					imgFlags = IMG_INIT_WEBP;

				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia(std::string filePath)
{
	//Loading success flag
	bool success = true;
	gTexture = loadTexture(filePath);
	if (gTexture == NULL)
		return !success;
	 while(rect.w > SCREEN_WIDTH || rect.h > SCREEN_HEIGHT) {
		 rect.w = rect.w * 90 / 100;
		 rect.h = rect.h * 90 / 100;
	}
	 rect.x = (SCREEN_WIDTH / 2) - (rect.w/2);
	 rect.y = (SCREEN_HEIGHT / 2) - (rect.h / 2);
	 
	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	SDL_GetClipRect(loadedSurface, &rect);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixel
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

int main(int argc, char* args[])
{
	std::string imagePath = args[1];
	for (size_t i = 0; i < imagePath.length(); i++) {
		if (imagePath[i] == '.')
			formatImage = imagePath.substr(i + 1, imagePath.length());
	}
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia(imagePath))
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_KEYDOWN)
					{
						if(e.key.keysym.sym == SDLK_ESCAPE)
							quit = true;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
				SDL_RenderClear(gRenderer);
				
				//Renderer Image
				SDL_RenderCopy(gRenderer, gTexture, NULL, &rect);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
  }
	//Free resources and close SDL
	close();
	return 0;
}
