#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "player1.h"
#include "player2.h"

using namespace std;

const int SCREEN_WIDTH = 1344;
const int SCREEN_HEIGHT = 896;

bool main_logic = true;
bool running = true;

char status_map[100][100];
char bomb_map[100][100];
char power_map[100][100];

const string WINDOW_TITLE = "Ok Bomber!";

Mix_Music* bgm;

void logSDLError(std::ostream& os, const std::string &msg, bool fatal = false);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void quit_check(SDL_Window* window, SDL_Renderer* renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void start();
void loadmyimage_but_des(SDL_Texture* texture, SDL_Renderer* renderer, int xd, int yd, int width, int height);
bool newgame(SDL_Event mousemotion, SDL_Renderer* renderer, SDL_Texture* newgame1, SDL_Texture* newgame2, int desx, int desy, int widthx, int heighty, SDL_Window* window);
bool play_again(SDL_Event mousemotion, SDL_Renderer* renderer, SDL_Texture* play_again1, SDL_Texture* play_again2, int desx, int desy, int widthx, int heighty, SDL_Window* window);

int main(int argc, char* argv[])
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer);
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) logSDLError(cout, "audio error", false);
    bgm = Mix_LoadMUS("game resources/wii_bgm.mp3");
    Mix_PlayMusic(bgm, -1);

    SDL_Event control;
    const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);

    SDL_Texture* background = IMG_LoadTexture(renderer, "game resources/grass_ground.png");
    SDL_Texture* brick = IMG_LoadTexture(renderer, "game resources/white_brick.png");
    SDL_Texture* box = IMG_LoadTexture(renderer, "game resources/box.png");
    SDL_Texture* bomb = IMG_LoadTexture(renderer, "game resources/bomb.png");
    SDL_Texture* fire = IMG_LoadTexture(renderer, "game resources/fire.png");
    SDL_Texture* player1_win = IMG_LoadTexture(renderer, "game resources/1win.png");
    SDL_Texture* player2_win = IMG_LoadTexture(renderer, "game resources/2win.png");

    SDL_Texture* power_1 = IMG_LoadTexture(renderer, "game resources/power1.png");
    SDL_Texture* power_2 = IMG_LoadTexture(renderer, "game resources/power2.png");
    SDL_Texture* power_3 = IMG_LoadTexture(renderer, "game resources/power3.png");

    // GUI
    SDL_Texture* newgame1 = IMG_LoadTexture(renderer, "game resources/newgame_frame.png");
    SDL_Texture* newgame2 = IMG_LoadTexture(renderer, "game resources/newgame.png");
    SDL_Texture* play_again1 = IMG_LoadTexture(renderer, "game resources/play_again_frame.png");
    SDL_Texture* play_again2 = IMG_LoadTexture(renderer, "game resources/play_again.png");

    while(main_logic)
    {
        SDL_RenderClear(renderer);
		if(newgame(control, renderer, newgame1, newgame2, 472, 400, 400, 125, window) == false) {main_logic = false; running = false;}
		start();
		player1 player1(renderer, "game resources/red-dude.png", 64, 64, 3, 4);
        player2 player2(renderer, "game resources/yellow-dude.png", 972, 788, 3, 4);

		int x1, y1, x2, y2;
        int prev_time = 0;
        int current_time = 0;
        float delta_time = 0;
        int win_number = 0;

		loadmyimage_but_des(background, renderer, 0, 0, 1344, 896);
		SDL_RenderPresent(renderer);

		while(running)
		{
		    prev_time = current_time;
			current_time = SDL_GetTicks();
			delta_time = (current_time - prev_time) / 250.0f;

            if(SDL_PollEvent(&control) != 0 && keyState[SDL_SCANCODE_ESCAPE] || control.type == SDL_QUIT)
            {
                quitSDL(window, renderer);
                main_logic = false;
                running = false;
                break;
            }
            if(keyState[SDL_SCANCODE_1])
                if(!Mix_PlayingMusic()) Mix_PlayMusic(bgm, -1);
            if(keyState[SDL_SCANCODE_2])
            {
                Mix_HaltChannel(-1);
                Mix_HaltMusic();
            }

			x1 = player1.position_rect1.x;
			y1 = player1.position_rect1.y;
			x2 = player2.position_rect2.x;
			y2 = player2.position_rect2.y;

			SDL_RenderClear(renderer);
			loadmyimage_but_des(background, renderer, 0, 0, 1344, 896);

			for(int i = 0; i < 14; ++i)
			    for(int j = 0; j < 17; ++j)
            	{
                	if(power_map[i][j] == '1') loadmyimage_but_des(power_1, renderer, j*64, i*64, 64, 64);
                	if(power_map[i][j] == '2') loadmyimage_but_des(power_2, renderer, j*64, i*64, 64, 64);
                	if(power_map[i][j] == '3') loadmyimage_but_des(power_3, renderer, j*64, i*64, 64, 64);
            	}

			for(int i = 0; i < 14; ++i)
			    for(int j = 0; j < 17; ++j)
				{
				    if(status_map[i][j] == '2') loadmyimage_but_des(brick, renderer, j*64, i*64, 64, 64);
                	else if(status_map[i][j] == '1') loadmyimage_but_des(box, renderer, j*64, i*64, 64, 64);
                	if(bomb_map[i][j] == '*' or bomb_map[i][j] == 'b') loadmyimage_but_des(bomb, renderer, j*64, i*64, 64, 64);
                	else if(bomb_map[i][j] == 'f')
					{
                    	loadmyimage_but_des(fire, renderer, j*64, i*64, 64, 64);
                    	if(bomb_map[y1 / 64][x1 / 64] == 'f' or bomb_map[(y1 + 40) / 64][x1 / 64] == 'f'
                    	or bomb_map[y1 / 64][(x1 + 40) / 64] == 'f' or bomb_map[(y1 + 40) / 64][(x1 + 40) / 64] == 'f')
	                        win_number = 2;
    	                if(bomb_map[y2 / 64][x2 / 64] == 'f' or bomb_map[(y2 + 40) / 64][x2 / 64] == 'f'
        	            or bomb_map[y2 / 64][(x2 + 40) / 64] == 'f' or bomb_map[(y2 + 40) / 64][(x2 + 40) / 64] == 'f')
            	            win_number = 1;
                	}
            	}

			if(win_number == 1) {loadmyimage_but_des(player1_win, renderer, 1088, 0, 256, 256); running = false;}
			else if(win_number == 2) {loadmyimage_but_des(player2_win, renderer, 1088, 0, 256, 256); running = false;}

			player1.update1(delta_time, keyState, status_map, bomb_map, power_map, control);
			player2.update2(delta_time, keyState, status_map, bomb_map, power_map, control);
			player1.draw1(renderer);
			player2.draw2(renderer);
			SDL_RenderPresent(renderer);
    	}

    	if(win_number == 0) break;

    	if(play_again(control, renderer, play_again1, play_again2, 1098, 796, 236, 50, window) == true)
        {
            running = true;
            main_logic = true;
            continue;
        }
    	else {main_logic = false; running = false;}
    }

    //if(win_number != 0) quit_check(window, renderer);

    for(int i = 0; i < 14; ++i)
    {
        for(int j = 0; j < 17; ++j)
            cout << status_map[i][j];
        cout << endl;
    }

    quitSDL(window, renderer);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(brick);
    SDL_DestroyTexture(box);
    SDL_DestroyTexture(bomb);
    SDL_DestroyTexture(fire);
    SDL_DestroyTexture(power_1);
    SDL_DestroyTexture(power_2);
    SDL_DestroyTexture(player1_win);
    SDL_DestroyTexture(player2_win);
    SDL_DestroyTexture(newgame1);
    SDL_DestroyTexture(newgame2);
    Mix_FreeMusic(bgm);

    return 0;
}

void logSDLError(std::ostream& os, const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

void quit_check(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_Event control;
    const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);

    while(true)
    {
        if(SDL_WaitEvent(&control) != 0 && (keyState[SDL_SCANCODE_ESCAPE] || control.type == SDL_QUIT))
        {
        	quitSDL(window, renderer);
            break;
        }
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);
    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
        logSDLError(std::cout, "CreateWindow", true);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
        logSDLError(std::cout, "CreateRenderer", true);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_Surface* iconimg = IMG_Load("game resources/icon.png");
    SDL_SetWindowIcon(window, iconimg);
    SDL_FreeSurface(iconimg);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

void start()
{
    srand(time(NULL));
    int posibility, power_number;
    for(int i = 0; i < 14; ++i)
        for(int j = 0; j < 17; ++j)
        {
            if(i == 0 or j == 0 or i == 13 or j == 16 or (i % 2 == 0 && j % 2 == 0))
                status_map[i][j] = '2';
            else if(i > 0 && j < 16 && ((i % 2 == 0 && j % 2 == 1) or (i % 2 == 1 && j % 2 == 0))) status_map[i][j] = '1';
            else status_map[i][j] = '0';
        }
    status_map[2][1] = '0'; status_map[1][2] = '0';
    status_map[12][15] = '0'; status_map[12][14] = '0';
    status_map[12][13] = '0'; status_map[5][8] = '2';
    status_map[6][8] = '1'; status_map[4][12] = '0';
    status_map[7][8] = '2'; status_map[8][4] = '0';
    status_map[5][5] = '2'; status_map[7][11] = '2';

    for(int i = 0; i < 14; ++i)
        for(int j = 0; j < 17; ++j)
        {
            bomb_map[i][j] = '0';
            power_map[i][j] = '0';
        }

    for(int i = 0; i < 14; ++i)
        for(int j = 0; j < 17; ++j)
        {
            posibility = rand() % 100 + 1;
            power_number = rand() % 3 + 1;
            if(posibility <= 21 && status_map[i][j] == '1') power_map[i][j] = power_number + '0';
        }
}

void loadmyimage_but_des(SDL_Texture* texture, SDL_Renderer* renderer, int xd, int yd, int width, int height)
{
    SDL_Rect rect;
    rect.x = xd;
    rect.y = yd;
    rect.w = width;
    rect.h = height;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

bool newgame(SDL_Event mousemotion, SDL_Renderer* renderer, SDL_Texture* newgame1, SDL_Texture* newgame2, int desx, int desy, int widthx, int heighty, SDL_Window* window)
{
	SDL_RenderClear(renderer);
	SDL_Texture* background = IMG_LoadTexture(renderer, "game resources/background1.png");
	SDL_RenderPresent(renderer);
    const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);

    loadmyimage_but_des(newgame1, renderer, desx, desy, widthx, heighty);
    SDL_RenderPresent(renderer);
    bool check = true;
    while(check == true)
	{
		if(SDL_WaitEvent(&mousemotion) != 0 && (keyState[SDL_SCANCODE_ESCAPE] || mousemotion.type == SDL_QUIT))
        {
            main_logic = false;
            running = false;
        	quitSDL(window, renderer);
            return false;
        }
        if(keyState[SDL_SCANCODE_1])
            if(!Mix_PlayingMusic()) Mix_PlayMusic(bgm, -1);
        if(keyState[SDL_SCANCODE_2])
        {
            Mix_HaltChannel(-1);
            Mix_HaltMusic();
        }
		else if(mousemotion.type == SDL_MOUSEMOTION)
		{
		    if(mousemotion.button.x > desx && mousemotion.button.x < (widthx+desx) && mousemotion.button.y > desy && mousemotion.button.y < (heighty+desy))
            {
                loadmyimage_but_des(background, renderer, 0, 0, 1344, 896);
            	loadmyimage_but_des(newgame1, renderer, desx, desy, widthx, heighty);
                SDL_RenderPresent(renderer);
            }
        	else
        	{
        	    loadmyimage_but_des(background, renderer, 0, 0, 1344, 896);
        		loadmyimage_but_des(newgame2, renderer, desx, desy, widthx, heighty);
                SDL_RenderPresent(renderer);
        	}
    	}
    	else if(mousemotion.type == SDL_MOUSEBUTTONUP
        && (mousemotion.button.x > desx && mousemotion.button.x < (widthx+desx) && mousemotion.button.y > desy && mousemotion.button.y < (heighty+desy)))
        {
			return true;
            break;
        }
    }

    SDL_DestroyTexture(background);
    SDL_RenderClear(renderer);
}

bool play_again(SDL_Event mousemotion, SDL_Renderer* renderer, SDL_Texture* play_again1, SDL_Texture* play_again2, int desx, int desy, int widthx, int heighty, SDL_Window* window)
{
    const Uint8 *keyState;
    keyState = SDL_GetKeyboardState(NULL);

    loadmyimage_but_des(play_again1, renderer, desx, desy, widthx, heighty);
    SDL_RenderPresent(renderer);
    bool check = true;
    while(check == true)
	{
		if(SDL_WaitEvent(&mousemotion) != 0 && (keyState[SDL_SCANCODE_ESCAPE] || mousemotion.type == SDL_QUIT))
        {
            main_logic = false;
            running = false;
        	quitSDL(window, renderer);
            return false;
        }
        if(keyState[SDL_SCANCODE_1])
            if(!Mix_PlayingMusic()) Mix_PlayMusic(bgm, -1);
        if(keyState[SDL_SCANCODE_2])
        {
            Mix_HaltChannel(-1);
            Mix_HaltMusic();
        }
		else if(mousemotion.type == SDL_MOUSEMOTION)
		{
		    if(mousemotion.button.x > desx && mousemotion.button.x < (widthx+desx) && mousemotion.button.y > desy && mousemotion.button.y < (heighty+desy))
            {
            	loadmyimage_but_des(play_again1, renderer, desx, desy, widthx, heighty);
                SDL_RenderPresent(renderer);
            }
        	else
        	{
        		loadmyimage_but_des(play_again2, renderer, desx, desy, widthx, heighty);
                SDL_RenderPresent(renderer);
        	}
    	}
    	else if(mousemotion.type == SDL_MOUSEBUTTONUP
        && (mousemotion.button.x > desx && mousemotion.button.x < (widthx+desx) && mousemotion.button.y > desy && mousemotion.button.y < (heighty+desy)))
        {
			return true;
            break;
        }
    }

    SDL_DestroyTexture(play_again1);
    SDL_DestroyTexture(play_again2);
    SDL_RenderClear(renderer);
}
