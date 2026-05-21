//
// Created by myat on 3/25/2026.
//

#ifndef NEOMYANIST_STRUCTS_H
#define NEOMYANIST_STRUCTS_H

using json = nlohmann::json;

struct Global {
    SDL_Color allwindowback;
    SDL_Color background;
    SDL_Color menuBackground;
    SDL_Color item;
    SDL_Color aimedItem;
    SDL_Color selectedItem;
    SDL_Color selectedFilter;
    SDL_Color aimedMenuItem;
    SDL_Color border;
    std::vector<SDL_Color> iconColors;
    SDL_Color defaultText;
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
};

#endif //NEOMYANIST_STRUCTS_H