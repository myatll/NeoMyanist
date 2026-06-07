//
// Created by myatll on 22.11.2025.
//

#ifndef MYANIST_LIST_H
#define MYANIST_LIST_H
#include "PipeClient.h"
#include "TextureLoader.h"
#include "Menu.h"
#include "nlohmann/json.hpp"
#include "curl/curl.h"
#include "structs.h"
#include <vector>
#include <string>
#include <format>
#include <fstream>

constexpr int fullItemSize = 40;
constexpr int itemSize = 30;

constexpr int fullTipSize = 120;
constexpr int tipSize = 110;

using json = nlohmann::json;
using defJson = nlohmann::ordered_json;

class Search;
class Filters;
class Categories;
class ItemInfo;
class List {
private:
    SDL_Texture* txtr;

    Categories* categ;
    ItemInfo* iinfo;
    Filters* filt;
    Search* search;

    json data;

    int listStartDrawIndex;
    int tipsStartDrawIndex;
    int currentIndex;
    int changeItemIndex;
    double scrollAnim;

    bool ctrl;
    bool needSave;

    int mouseX;
    int mouseY;

    PipeClient& client;
    AsyncTextureLoader& tloader;
    SDL_Rect posRect;
    Global& g;
    Menu& menu;
    int MenuID;

    struct listDrawItem {
        int opinion;
        int status;
        std::string name;
    };

    struct SearchTipsDrawItem {
        bool miniPosterLoaded = false;
        double miniPosterAspect = false;
        SDL_Texture* miniPoster;
        std::string name;
        std::string genres;
        json data;
    };

    std::vector<listDrawItem> list;
    std::vector<SearchTipsDrawItem> tips;
    std::string categoryPath;

public:
    bool updateTxtr;

    List(Global& gInfo, Menu& mnu, PipeClient& clnt, AsyncTextureLoader& tldr);
    ~List() = default;

    void setSearchIter(Search* searchIter);
    void setCategIter(Categories* categIter);
    void setInfoIter(ItemInfo* iinfoIter);
    void setFiltersIter(Filters* filtersIter);
    void returnEditedItem(std::string& oldName, std::string newName, json& currentItem);
    void addItem(std::string name, json& currentItem);

    void render();
    void update();
    void setPos(SDL_Rect newPos);
    void handle();

    void loadCateg(const std::string& path);
    void updateList(const std::string& name, const json& currentItem);
    void drawReset();

private:
    static std::string toLower(const std::string& str);
};


#endif //MYANIST_LIST_H
