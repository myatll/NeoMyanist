//
// Created by myat on 3/28/2026.
//

#ifndef NEOMYANIST_SEARCH_H
#define NEOMYANIST_SEARCH_H

#include "MyatllDraw.h"
#include "nlohmann/json.hpp"
#include <MyatllNotepad.h>
#include "Menu.h"
#include "PipeClient.h"
#include "structs.h"
#include "Categories.h"
#include "List.h"

using json = nlohmann::json;

class List;
class Categories;
class Filters;
class Search {
private:
    SDL_Texture* txtr;

    Categories* categ;
    Filters* filt;

    bool updateTxtr;
    double modeAnim;
    bool isChange;
    std::string oldNameEdit;

    int mouseX;
    int mouseY;

    int status;
    int opinion;

    int MenuID;
    Uint32 sendTimer;
    bool lastSend;

    json currentItem;

    SDL_Rect posRect;
    OneStroke search;
    PipeClient& client;
    Menu& menu;
    Global& g;
    List& list;

public:
    bool mode;

    Search(Global& gInfo, Menu& mnu, List& lst, PipeClient& clnt);
    ~Search() = default;

    void setCategIter(Categories* categIter);
    void setFilterIter(Filters* filterIter);
    void reloadCateg();

    void update4Filters() const;

    void resetCurrentItem();
    void setItem4Edit(std::string& name, json& data);
    void setTip(std::string& name, json& data);
    void update();
    void render();
    void setPos(SDL_Rect newPos);
    void handle();
};


#endif //NEOMYANIST_SEARCH_H