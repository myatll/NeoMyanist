//
// Created by myat on 3/29/2026.
//

#ifndef NEOMYANIST_ITEMINFO_H
#define NEOMYANIST_ITEMINFO_H

#include "MyatllDraw.h"
#include "Menu.h"
#include "List.h"
#include "nlohmann/json.hpp"
#include "structs.h"

using json = nlohmann::json;

class ItemInfo {
private:
    SDL_Texture* txtr;
    SDL_Texture* poster;

    bool updateTxtr;

    int mouseX;
    int mouseY;
    int MenuID;
    double posterAspect;

    json currentItem;

    SDL_Rect posRect;
    Global& g;
    Menu& menu;
    List& list;

    int drawOffset;
    double offsetAnim;

public:
    ItemInfo(Global& gInfo, Menu& mnu, List& lst);
    ~ItemInfo() = default;

    void setPoster(SDL_Texture* posterTxtr, int id, double aspect);
    void setItem(const json& Item);
    void addInfo(const json& Info);
    void update();
    void render();
    void setPos(SDL_Rect newPos);
    void handle();
};


#endif NEOMYANIST_ITEMINFO_H