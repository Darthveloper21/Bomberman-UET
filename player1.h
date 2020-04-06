#ifndef PLAYER1_H
#define PLAYER1_H
#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

using namespace std;

class player1
{
private:
    SDL_Rect cropRect;
    SDL_Rect bombRect;
    SDL_Texture* texture;
    float frame_counter;
    int frame_width, frame_height;
    int texture_width, texture_height;
    bool active;
    SDL_Scancode keys[6];

    // bomb value
    int bombtime = 0;
    int initbomb = 0;
    int waittime = 1200;
    bool explode = false;
    int bx = 0, by = 0;

    //sound
    Mix_Chunk* setbomb = Mix_LoadWAV("game resources/setbomb.wav");
    Mix_Chunk* explosion = Mix_LoadWAV("game resources/explosion.wav");
    Mix_Chunk* pop = Mix_LoadWAV("game resources/pop.wav");

public:
    player1(SDL_Renderer* renderer, string filepath, int x, int y, int framex, int framey);
    virtual ~player1();

    void update1(float delta, const Uint8 *keyState, char status_map[][100], char bomb_map[][100], char power_map[][100]);
    void draw1(SDL_Renderer* renderer);
    SDL_Rect position_rect1;
    float move_speed1;
    int bomb_length1 = 1;
};

#endif // PLAYER1_H
