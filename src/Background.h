//
// Created by myatll on 8/19/25.
//

#ifndef SPIDER_BACKGROUND_H
#define SPIDER_BACKGROUND_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>

constexpr static auto block_sz = 100;

class Background {
private:
    int8_t x_offset[block_sz][block_sz];
    int8_t y_offset[block_sz][block_sz];

    SDL_Surface* load_img;
    SDL_Surface* crop_img;
    SDL_Texture* background;
    SDL_Renderer* renderer;

    SDL_Rect posRect;
    bool needFilter = true;

    static void put_pixel(const SDL_Surface* surface, int x, int y, Uint32 pixel);

    static Uint32 get_pixel(const SDL_Surface* surface, int x, int y);
    void modify_pixels_safe(SDL_Rect rct);

    void cropToAspectRatio();

public:
    Background(SDL_Renderer* rnd);
    void loadOptImage(const std::string &filepath);
    void setPos (SDL_Rect prct);
    void generatePattern(int index_ = 0, float amplitude = 10.0f, float frequency1 = 0.1f, float frequency2 = 0.23f, float noiseIntensity = 2.5f);

    void render();
    void setFilter(SDL_Rect rct, int radius = 0, bool relative = true, bool unSafe = false);
    virtual ~Background();
};

#endif //SPIDER_BACKGROUND_H
