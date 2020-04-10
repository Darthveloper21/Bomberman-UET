#include "player2.h"
#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

using namespace std;

const int SCREEN_WIDTH = 1344;
const int SCREEN_HEIGHT = 896;

player2::player2(SDL_Renderer* renderer, string filepath, int x, int y, int framex, int framey)
{
    SDL_Surface *surface = IMG_Load(filepath.c_str());
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(texture == NULL) cout << "error";
    SDL_FreeSurface(surface);

    SDL_QueryTexture(texture, NULL, NULL, &cropRect.w, &cropRect.h);
    position_rect2.x = x;
    position_rect2.y = y;
    cropRect.y = 0;

    texture_width = cropRect.w;
    texture_height = cropRect.h;
    cropRect.w /= framex;
    cropRect.h /= framey;

    frame_width = position_rect2.w = cropRect.w;
    frame_height = position_rect2.h = cropRect.h;

    active = false;

    keys[0] = SDL_SCANCODE_UP;
    keys[1] = SDL_SCANCODE_DOWN;
    keys[2] = SDL_SCANCODE_LEFT;
    keys[3] = SDL_SCANCODE_RIGHT;
    keys[4] = SDL_SCANCODE_P;

    move_speed2 = 55.0f;
}

player2::~player2()
{
    SDL_DestroyTexture(texture);
}

void player2::update2(float delta, const Uint8 *keyState, char status_map[][100], char bomb_map[][100], char power_map[][100], SDL_Event keyboard)
{
    int cnt = 0;
    active = true;
    bombRect.w = 64;
    bombRect.h = 64;

    for(int i = 0; i < 14; ++i)
        for(int j = 0; j < 17; ++j)
            if(bomb_map[i][j] == 'b')
            {
                if(bomb_map[position_rect2.y / 64][position_rect2.x / 64] != bomb_map[i][j] && bomb_map[(position_rect2.y + frame_height) / 64][(position_rect2.x + frame_width) / 64] != bomb_map[i][j]
                && bomb_map[position_rect2.y / 64][(position_rect2.x + frame_width) / 64] != bomb_map[i][j] && bomb_map[(position_rect2.y + frame_height) / 64][position_rect2.x / 64] != bomb_map[i][j])
                    status_map[i][j] = '3';
            }

    if(status_map[position_rect2.y / 64][position_rect2.x / 64] == '0' && status_map[(position_rect2.y + frame_height) / 64][(position_rect2.x + frame_width) / 64] == '0'
    && status_map[(position_rect2.y + frame_height) / 64][position_rect2.x / 64] == '0' && status_map[position_rect2.y / 64][(position_rect2.x + frame_width) / 64] == '0')
    {
        if(keyState[keys[0]]) // up
        {
            position_rect2.y -= move_speed2 * delta;
            if(status_map[position_rect2.y / 64][position_rect2.x / 64] != '0' or
               status_map[position_rect2.y / 64][(position_rect2.x + frame_width) / 64] != '0')
                position_rect2.y = (position_rect2.y/64 + 1) * 64 + 1;
            cropRect.y = frame_height;
        }
        else if(keyState[keys[1]]) // down
        {
            position_rect2.y += move_speed2 * delta + 1;
            if(status_map[(position_rect2.y + frame_height) / 64][position_rect2.x / 64] != '0' or
               status_map[(position_rect2.y + frame_height) / 64][(position_rect2.x + frame_width) / 64] != '0')
                position_rect2.y = ((position_rect2.y + frame_height) / 64 - 1) * 64 + 63 - frame_height;
            cropRect.y = 0;
        }
        if(keyState[keys[2]]) // left
        {
            position_rect2.x -= move_speed2 * delta;
            if(status_map[position_rect2.y / 64][position_rect2.x / 64] != '0' or
               status_map[(position_rect2.y + frame_height) / 64][position_rect2.x / 64] != '0')
                position_rect2.x = (position_rect2.x / 64 + 1) * 64 + 1;
            cropRect.y = frame_height * 2;
        }
        else if(keyState[keys[3]]) // right
        {
            position_rect2.x += move_speed2 * delta + 1;
            if(status_map[position_rect2.y / 64][(position_rect2.x + frame_width) / 64] != '0' or
               status_map[(position_rect2.y + frame_height) / 64][(position_rect2.x + frame_width) / 64] != '0')
                position_rect2.x = ((position_rect2.x + frame_width) / 64 - 1) * 64 + 63 - frame_width;
            cropRect.y = frame_height * 3;
        }
        else if(!keyState[keys[0]] && !keyState[keys[1]] && !keyState[keys[2]] && !keyState[keys[3]])
        {
            active = false;
        }

        if((position_rect2.x + frame_width) % 64 > 32) bombRect.x = position_rect2.x + frame_width;
        else if((position_rect2.x + frame_width) % 64 <= 32) bombRect.x = position_rect2.x;
        if((position_rect2.y + frame_height) % 64 > 32) bombRect.y = position_rect2.y + frame_height;
        else if((position_rect2.y + frame_height) % 64 <= 32) bombRect.y = position_rect2.y;

        if(keyState[keys[4]] && explode[press_cnt] == false && press_cnt < pocket && keyboard.type == SDL_KEYDOWN && bomb_map[bombRect.y/64][bombRect.x/64] == '0') // bomb
        {
            Mix_PlayChannel(4, setbomb, 0);
            bx[press_cnt] = bombRect.y / 64; by[press_cnt] = bombRect.x / 64;
            bomb_map[bx[press_cnt]][by[press_cnt]] = 'b';
            initbomb[press_cnt] = bombtime[press_cnt];
            explode[press_cnt] = true;
            press_cnt++;
        }
    }
    else active = false;

    while(cnt < pocket)
    {
        bombtime[cnt] = SDL_GetTicks();
        if(bombtime[cnt] - initbomb[cnt] >= waittime && explode[cnt] == true)
        {
            Mix_PlayChannel(5, explosion, 0);
            if(status_map[bx[cnt]][by[cnt]] != '2') bomb_map[bx[cnt]][by[cnt]] = 'f';
            for(int i = 0; i <= bomb_length2; ++i)
            {
                if(status_map[bx[cnt] + i][by[cnt]] == '0' && bx[cnt] + i < 14) bomb_map[bx[cnt] + i][by[cnt]] = 'f';
                if(status_map[bx[cnt] + i][by[cnt]] == '1') {bomb_map[bx[cnt] + i][by[cnt]] = 'f'; break;}
                if(status_map[bx[cnt] + i][by[cnt]] == '2') break;
            }
            for(int i = 0; i <= bomb_length2; ++i)
            {
                if(status_map[bx[cnt] - i][by[cnt]] == '0' && bx[cnt] - i > 0)  bomb_map[bx[cnt] - i][by[cnt]] = 'f';
                if(status_map[bx[cnt] - i][by[cnt]] == '1') {bomb_map[bx[cnt] - i][by[cnt]] = 'f'; break;}
                if(status_map[bx[cnt] - i][by[cnt]] == '2') break;
            }
            for(int i = 0; i <= bomb_length2; ++i)
            {
                if(status_map[bx[cnt]][by[cnt] + i] == '0' && by[cnt] + i < 17) bomb_map[bx[cnt]][by[cnt] + i] = 'f';
                if(status_map[bx[cnt]][by[cnt] + i] == '1') {bomb_map[bx[cnt]][by[cnt] + i] = 'f'; break;}
                if(status_map[bx[cnt]][by[cnt] + i] == '2') break;
            }
            for(int i = 0; i <= bomb_length2; ++i)
            {
                if(status_map[bx[cnt]][by[cnt] - i] == '0' && by[cnt] + i > 0)  bomb_map[bx[cnt]][by[cnt] - i] = 'f';
                if(status_map[bx[cnt]][by[cnt] - i] == '1') {bomb_map[bx[cnt]][by[cnt] - i] = 'f'; break;}
                if(status_map[bx[cnt]][by[cnt] - i] == '2') break;
            }

            if(bombtime[cnt] - initbomb[cnt] >= waittime + 400)
            {
                if(status_map[bx[cnt]][by[cnt]] != '2') {bomb_map[bx[cnt]][by[cnt]] = '0'; status_map[bx[cnt]][by[cnt]] = '0';}
                for(int i = 0; i <= bomb_length2; ++i)
                {
                    if(status_map[bx[cnt] + i][by[cnt]] == '0' && bx[cnt] + i < 14) {bomb_map[bx[cnt] + i][by[cnt]] = '0'; status_map[bx[cnt] + i][by[cnt]] = '0';}
                    if(status_map[bx[cnt] + i][by[cnt]] == '1') {bomb_map[bx[cnt] + i][by[cnt]] = '0'; status_map[bx[cnt] + i][by[cnt]] = '0'; break;}
                    if(status_map[bx[cnt] + i][by[cnt]] == '2') break;
                }
                for(int i = 0; i <= bomb_length2; ++i)
                {
                    if(status_map[bx[cnt] - i][by[cnt]] == '0' && bx[cnt] - i > 0)  {bomb_map[bx[cnt] - i][by[cnt]] = '0'; status_map[bx[cnt] - i][by[cnt]] = '0';}
                    if(status_map[bx[cnt] - i][by[cnt]] == '1') {bomb_map[bx[cnt] - i][by[cnt]] = '0'; status_map[bx[cnt] - i][by[cnt]] = '0'; break;}
                    if(status_map[bx[cnt] - i][by[cnt]] == '2') break;
                }
                for(int i = 0; i <= bomb_length2; ++i)
                {
                    if(status_map[bx[cnt]][by[cnt] + i] == '0' && by[cnt] + i < 17) {bomb_map[bx[cnt]][by[cnt] + i] = '0'; status_map[bx[cnt]][by[cnt] + i] = '0';}
                    if(status_map[bx[cnt]][by[cnt] + i] == '1') {bomb_map[bx[cnt]][by[cnt] + i] = '0'; status_map[bx[cnt]][by[cnt] + i] = '0'; break;}
                    if(status_map[bx[cnt]][by[cnt] + i] == '2') break;
                }
                for(int i = 0; i <= bomb_length2; ++i)
                {
                    if(status_map[bx[cnt]][by[cnt] - i] == '0' && by[cnt] + i > 0)  {bomb_map[bx[cnt]][by[cnt] - i] = '0'; status_map[bx[cnt]][by[cnt] - i] = '0';}
                    if(status_map[bx[cnt]][by[cnt] - i] == '1') {bomb_map[bx[cnt]][by[cnt] - i] = '0'; status_map[bx[cnt]][by[cnt] - i] = '0'; break;}
                    if(status_map[bx[cnt]][by[cnt] - i] == '2') break;
                }
                explode[cnt] = false;
                press_cnt--;
            }
        }
        cnt++;
    }

    if(power_map[(position_rect2.y + 32) / 64][(position_rect2.x + 32) / 64] == '1')
    {
        Mix_PlayChannel(6, pop, 0);
        waittime+=100;
        bomb_length2++;
        power_map[(position_rect2.y + 32) / 64][(position_rect2.x + 32) / 64] = '0';
    }

    if(power_map[(position_rect2.y + 32) / 64][(position_rect2.x + 32) / 64] == '2')
    {
        Mix_PlayChannel(6, pop, 0);
        waittime-=100;
        move_speed2+=2.5f;
        power_map[(position_rect2.y + 32) / 64][(position_rect2.x + 32) / 64] = '0';
    }

    if(power_map[(position_rect2.y + 32) / 64][(position_rect2.x + 32) / 64] == '3')
    {
        Mix_PlayChannel(6, pop, 0);
        waittime+=150;
        pocket++;
        power_map[(position_rect2.y + 32) / 64][(position_rect2.x + 32) / 64] = '0';
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

void player2::draw2(SDL_Renderer* renderer)
{
    SDL_RenderCopy(renderer, texture, &cropRect, &position_rect2);
}
