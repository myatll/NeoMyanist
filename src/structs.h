//
// Created by myat on 3/25/2026.
//

#ifndef NEOMYANIST_STRUCTS_H
#define NEOMYANIST_STRUCTS_H

#include "Background.h"

using json = nlohmann::json;

struct Global {
    SDL_Color allWindowBack;
    SDL_Color background;
    SDL_Color menuBackground;
    SDL_Color infoBackground;
    SDL_Color item;
    SDL_Color aimedItem;
    SDL_Color selectedItem;
    SDL_Color itemState;
    SDL_Color selectedFilter;
    SDL_Color aimedMenuItem;
    SDL_Color border;
    std::vector<SDL_Color> opinionIconColors;
    std::vector<SDL_Color> statusIconColors;
    SDL_Color defaultText;
    SDL_Color genresText;
    SDL_Color noSelectedText;
    SDL_Color menuText;
    SDL_Color menuUnavailableText;

    TTF_Font* defaultFont;
    TTF_Font* lowFont;
    TTF_Font* bigFont;
    TTF_Font* iconsFont;
    TTF_Font* menuFont;
    TTF_Font* menuIconsFont;

    json& settings;
    json& state;

    SDL_Event& e;

    SDL_Texture* loadindImg;
    SDL_Renderer* renderer;
    Background* bg;

    bool drawCategories = true;
    bool drawFilters = false;
    bool drawInfo = false;

    bool needReSize = false;

    void setRenderer(SDL_Renderer* rnd) {
        renderer = rnd;
    }
    void setBackground(Background* _bg) {
        bg = _bg;
    }
};

#endif //NEOMYANIST_STRUCTS_H