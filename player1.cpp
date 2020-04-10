#include "player1.h"
#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

using namespace std;

const int SCREEN_WIDTH = 1344;
const int SCREEN_HEIGHT = 896;

player1::player1(SDL_Renderer* renderer, string filepath, int x, int y, int framex, int framey)
{
    SDL_Surface *surface = IMG_Load(filepath.c_str());
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(texture == NULL) cout << "error";
    SDL_FreeSurface(surface);

    SDL_QueryTexture(texture, NULL, NULL, &cropRect.w, &cropRect.h);
    position_rect1.x = x;
    position_rect1.y = y;
    cropRect.y = 0;

    texture_width = cropRect.w;
    texture_height = cropRect.h;
    cropRect.w /= framex;
    cropRect.h /= framey;

    frame_width = position_rect1.w = cropRect.w;
    frame_height = position_rect1.h = cropRect.h;

    active = false;

    keys[0] = SDL_SCANCODE_W;
    keys[1] = SDL_SCANCODE_S;
    keys[2] = SDL_SCANCODE_A;
    keys[3] = SDL_SCANCODE_D;
    keys[4] = SDL_SCANCODE_SPACE;

    move_speed1 = 55.0f;
}

player1::~player1()
{
    SDL_DestroyTexture(texture);
}

void player1::update1(float delta, const Uint8 *keyState, char status_map[][100], char bomb_map[][100], char power_map[][100], SDL_Event keyboard)
{
    int cnt = 0;
    active = true;
    bombRect.w = 64;
    bombRect.h = 64;

    for(int i = 0; i < 14; ++i)
        for(int j = 0; j < 17; ++j)
            if(bomb_map[i][j] == '*')
            {
                if(bomb_map[position_rect1.y / 64][position_rect1.x / 64] != bomb_map[i][j] && bomb_map[(position_rect1.y + frame_width) / 64][(position_rect1.x + frame_height) / 64] != bomb_map[i][j]
                && bomb_map[position_rect1.y / 64][(position_rect1.x + frame_width) / 64] != bomb_map[i][j] && bomb_map[(position_rect1.y + frame_height) / 64][position_rect1.x / 64] != bomb_map[i][j])
                    status_map[i][j] = '3';
            }

    if(status_map[position_rect1.y / 64][position_rect1.x / 64] == '0' && status_map[(position_rect1.y + frame_height) / 64][(position_rect1.x + frame_width) / 64] == '0'
    && status_map[(position_rect1.y + frame_height) / 64][position_rect1.x / 64] == '0' && status_map[position_rect1.y / 64][(position_rect1.x + frame_width) / 64] == '0')
    {
        if(keyState[keys[0]]) // up
        {
            position_rect1.y -= move_speed1 * delta;
            if(status_map[position_rect1.y / 64][position_rect1.x / 64] != '0' or
               status_map[position_rect1.y / 64][(position_rect1.x + frame_width) / 64] != '0')
                position_rect1.y = (position_rect1.y/64 + 1) * 64 + 1;
            cropRect.y = frame_height;
        }
        else if(keyState[keys[1]]) // down
        {
            position_rect1.y += move_speed1 * delta + 1;
            if(status_map[(position_rect1.y + frame_height) / 64][position_rect1.x / 64] != '0' or
               status_map[(position_rect1.y + frame_height) / 64][(position_rect1.x + frame_width) / 64] != '0')
                position_rect1.y = ((position_rect1.y + frame_height) / 64 - 1) * 64 + 63 - frame_height;
            cropRect.y = 0;
        }
        else if(keyState[keys[2]]) // left
        {
            position_rect1.x -= move_speed1 * delta;
            if(status_map[position_rect1.y / 64][position_rect1.x / 64] != '0' or
               status_map[(position_rect1.y + frame_height) / 64][position_rect1.x / 64] != '0')
                position_rect1.x = (position_rect1.x / 64 + 1) * 64 + 1;
            cropRect.y = frame_height * 2;
        }
        else if(keyState[keys[3]]) // right
        {
            position_rect1.x += move_speed1 * delta + 1;
            if(status_map[position_rect1.y / 64][(position_rect1.x + frame_width) / 64] != '0' or
               status_map[(position_rect1.y + frame_height) / 64][(position_rect1.x + frame_width) / 64] != '0')
                position_rect1.x = ((position_rect1.x + frame_width) / 64 - 1) * 64 + 63 - frame_width;
            cropRect.y = frame_height * 3;
        }
        else
        {
            active = false;
        }

        if((position_rect1.x + frame_width) % 64 > 32) bombRect.x = position_rect1.x + frame_width;
        else if((position_rect1.x + frame_width) % 64 <= 32) bombRect.x = position_rect1.x;
        if((position_rect1.y + frame_height) % 64 > 32) bombRect.y = position_rect1.y + frame_height;
        else if((position_rect1.y + frame_height) % 64 <= 32) bombRect.y = position_rect1.y;

        if(keyState[keys[4]] && explode[press_cnt] == false && press_cnt < pocket && keyboard.type == SDL_KEYDOWN && bomb_map[bombRect.y/64][bombRect.x/64] == '0') // bomb
        {
            Mix_PlayChannel(1, setbomb, 0);
            bx[press_cnt] = bombRect.y / 64; by[press_cnt] = bombRect.x / 64;
            bomb_map[bx[press_cnt]][by[press_cnt]] = '*';
            initbomb[press_cnt] = bombtime[press_cnt];
            explode[press_cnt] = true;
            press_cnt++;
            cout << press_cnt << endl;
        }
    }
    else active = false;

    while(cnt < pocket)
    {
        bombtime[cnt] = SDL_GetTicks();
        if(bombtime[cnt] - initbomb[cnt] >= waittime && explode[cnt] == true)
        {
            Mix_PlayChannel(2, explosion, 0);
            if(status_map[bx[cnt]][by[cnt]] != '2') bomb_map[bx[cnt]][by[cnt]] = 'f';
            for(int i = 0; i <= bomb_length1; ++i) {if(status_map[bx[cnt] + i][by[cnt]] != '2' && bx[cnt] + i < 14) bomb_map[bx[cnt] + i][by[cnt]] = 'f'; else break;}
            for(int i = 0; i <= bomb_length1; ++i) {if(status_map[bx[cnt] - i][by[cnt]] != '2' && bx[cnt] - i > 0)  bomb_map[bx[cnt] - i][by[cnt]] = 'f'; else break;}
            for(int i = 0; i <= bomb_length1; ++i) {if(status_map[bx[cnt]][by[cnt] + i] != '2' && by[cnt] + i < 17) bomb_map[bx[cnt]][by[cnt] + i] = 'f'; else break;}
            for(int i = 0; i <= bomb_length1; ++i) {if(status_map[bx[cnt]][by[cnt] - i] != '2' && by[cnt] + i > 0)  bomb_map[bx[cnt]][by[cnt] - i] = 'f'; else break;}

            if(bombtime[cnt] - initbomb[cnt] >= waittime + 400)
            {
                if(status_map[bx[cnt]][by[cnt]] != '2') {bomb_map[bx[cnt]][by[cnt]] = '0'; status_map[bx[cnt]][by[cnt]] = '0';}
                for(int i = 0; i <= bomb_length1; ++i)
                {
                    if(status_map[bx[cnt] + i][by[cnt]] != '2' && bx[cnt] + i < 14) {bomb_map[bx[cnt] + i][by[cnt]] = '0'; status_map[bx[cnt] + i][by[cnt]] = '0';}
                    else break;
                }
                for(int i = 0; i <= bomb_length1; ++i)
                {
                    if(status_map[bx[cnt] - i][by[cnt]] != '2' && bx[cnt] - i > 0) {bomb_map[bx[cnt] - i][by[cnt]] = '0'; status_map[bx[cnt] - i][by[cnt]] = '0';}
                    else break;
                }
                for(int i = 0; i <= bomb_length1; ++i)
                {
                    if(status_map[bx[cnt]][by[cnt] + i] != '2' && by[cnt] + i < 17) {bomb_map[bx[cnt]][by[cnt] + i] = '0'; status_map[bx[cnt]][by[cnt] + i] = '0';}
                    else break;
                }
                for(int i = 0; i <= bomb_length1; ++i)
                {
                    if(status_map[bx[cnt]][by[cnt] - i] != '2' && by[cnt] - i > 0) {bomb_map[bx[cnt]][by[cnt] - i] = '0'; status_map[bx[cnt]][by[cnt] - i] = '0';}
                    else break;
                }
                explode[cnt] = false;
                press_cnt--;
            }
        }
        cnt++;
    }

    if(power_map[(position_rect1.y + 32) / 64][(position_rect1.x + 32) / 64] == '1')
    {
        Mix_PlayChannel(3, pop, 0);
        bomb_length1++;
        waittime+=200;
        power_map[(position_rect1.y + 32) / 64][(position_rect1.x + 32) / 64] = '0';
    }

    if(power_map[(position_rect1.y + 32) / 64][(position_rect1.x + 32) / 64] == '2')
    {
        Mix_PlayChannel(3, pop, 0);
        move_speed1+=2.5f;
        power_map[(position_rect1.y + 32) / 64][(position_rect1.x + 32) / 64] = '0';
    }

    if(power_map[(position_rect1.y + 32) / 64][(position_rect1.x + 32) / 64] == '3')
    {
        Mix_PlayChannel(3, pop, 0);
        pocket++;
        power_map[(position_rect1.y + 32) / 64][(position_rect1.x + 32) / 64] = '0';
    }

    if(active)
    {
        frame_counter += delta;

        if(frame_counter >= 0.25f)
        {
            frame_counter = 0;
            cropRect.x += frame_width;
            if(cropRect.x >= texture_width) cropRect.x = 0;
        }
    }
    else
    {
        frame_counter = 0;
        cropRect.x = 0;
    }
}

void player1::draw1(SDL_Renderer* renderer)
{
    SDL_RenderCopy(renderer, texture, &cropRect, &position_rect1);
}
