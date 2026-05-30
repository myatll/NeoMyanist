//
// Created by myat on 3/29/2026.
//

#ifndef NEOMYANIST_FILTERS_H
#define NEOMYANIST_FILTERS_H

#include "MyatllDraw.h"
#include "nlohmann/json.hpp"
#include "structs.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include "List.h"
#include "Search.h"
#include "Menu.h"

using json = nlohmann::json;

class List;
class Search;
class Filters {
private:
    SDL_Texture* txtr;

    bool updateTxtr;
    int startDrawIndex;
    int currentIndex;
    double scrollAnim;

    bool ctrl;
    int MenuID;

    int mouseX;
    int mouseY;

    SDL_Rect posRect;
    Global& g;
    List& list;
    Search& search;
    Menu& menu;

    std::vector<std::string> selectedFilters;
    std::vector<std::pair<std::string, int>> availableFilters;

public:
    Filters(Global& gInfo, Menu& mnu, List& lst, Search& srch);
    ~Filters() = default;

    defJson filterJson;

    void updateAvailableFilters(const std::map<std::string, int> &filters);
    void render();
    void setPos(SDL_Rect newPos);
    void handle();
    void update();
};


#endif //NEOMYANIST_FILTERS_H
