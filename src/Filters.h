//
// Created by myat on 3/29/2026.
//

#ifndef NEOMYANIST_FILTERS_H
#define NEOMYANIST_FILTERS_H

#include "MyatllDraw.h"
#include "nlohmann/json.hpp"
#include "structs.h"
#include <vector>
#include <string>
#include <fstream>
using json = nlohmann::json;

class Filters {
private:
    SDL_Renderer* renderer;
    SDL_Texture* txtr;
    TTF_Font* font;
    TTF_Font* icons;

    json& settings;
    SDL_Event &e;

    bool updateTxtr;
    int startDrawIndex;
    int currentIndex;
    double scrollAnim;

    bool ctrl;

    int mouseX;
    int mouseY;

    SDL_Rect posRect;
    Global& g;

    struct listDrawItem {
        int opinion;
        int status;
        std::string name;
    };
    std::vector<listDrawItem> list;

public:
    Filters(SDL_Renderer* rnd, TTF_Font* fnt, TTF_Font* icns, json& sttg, Global& gInfo, SDL_Event &ev);
    ~Filters() = default;

    void render();
    void setPos(SDL_Rect newPos);
    void handle();
};


#endif //NEOMYANIST_FILTERS_H