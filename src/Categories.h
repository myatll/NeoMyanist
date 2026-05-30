//
// Created by myat on 3/29/2026.
//

#ifndef NEOMYANIST_CATEGORIES_H
#define NEOMYANIST_CATEGORIES_H

#include "MyatllDraw.h"
#include <filesystem>
#include "Menu.h"
#include "List.h"
#include "Search.h"
#include "nlohmann/json.hpp"
#include "structs.h"

using json = nlohmann::json;

class Categories {
private:
    SDL_Texture* txtr;

    bool updateTxtr;
    double modeAnim;

    int mouseX;
    int mouseY;

    int MenuID;

    SDL_Rect posRect;
    Global& g;
    Menu& menu;
    Search& search;
    List& list;

public:
    Categories(Global& gInfo, Menu& mnu, List& lst, Search& srch);
    ~Categories() = default;

    std::string getCategory() const;
    void update();
    void render();
    void setPos(SDL_Rect newPos);
    void handle();
};


#endif //NEOMYANIST_CATEGORIES_H