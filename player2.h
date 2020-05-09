#ifndef PLAYER2_H
#define PLAYER2_H
#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

using namespace std;

class player2
{
private:
    SDL_Rect cropRect;
    SDL_Rect bombRect;
    float frame_counter;
    int frame_width, frame_height;
    int texture_width, texture_height;
    bool active;
    SDL_Scancode keys[6];

    // bomb value
    int bombtime[100] = {0};
    int initbomb[100] = {0};
    int waittime = 1500;
    bool explode[100] = {false};
    int bx[100] = {0}, by[100] = {0}, pocket = 1, press_cnt = 0;

public:
    player2(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int framex, int framey);
    virtual ~player2();

    void update2(float delta, const Uint8 *keyState, char status_map[][100], char bomb_map[][100], char power_map[][100],
                 SDL_Event keyboard, Mix_Chunk* setbomb, Mix_Chunk* explosion, Mix_Chunk* pop, bool sound);
    void draw2(SDL_Renderer* renderer, SDL_Texture* texture);
    SDL_Rect position_rect2;
    float move_speed2;
    int bomb_length2 = 1;

};

#endif // PLAYER2_H
