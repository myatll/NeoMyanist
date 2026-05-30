//
// Created by myat on 4/2/2026.
//

#ifndef NEOMYANIST_MENU_H
#define NEOMYANIST_MENU_H

#include <SDL_ttf.h>
#include <SDL.h>
#include <vector>
#include <iostream>
#include <codecvt>
#include "MyatllDraw.h"
#include "nlohmann/json.hpp"
#include "structs.h"

using json = nlohmann::json;

// 10, 20 - выбор мнения для поиска и списка
// 15, 25 - выбор статуса для поиска и списка
// 27 - меню изменения эоемента в списке

struct menuResult {
    bool notProcessed = false;
    int lastId = -1;
    int value = -1;
};
/*
 * type
 * 0 default:
 * icon - name
 * icon - name
 * icon - name
 *
 * 1 only icons:
 * icon
 * icon
 * icon
 */

struct menuItem {

    int type = 0;
    int id = 0;
    int w = -1;
    int h = -1;

    struct draw {
        std::string name;
        std::string icon;
        bool offset = false;
        bool unavailable = false;
    };

    std::vector<draw> draws;

    void add(const std::string& icon, const std::string& name) {
        draw newItem;
        newItem.name = name;
        newItem.icon = icon;
        draws.push_back(newItem);
    }

    void setSize(const int W, const int H) {
        w = W; h = H;
    }

    void change(const std::vector<draw>::size_type it, const std::string& name, const std::string& icon = "") {
        if (it > draws.size() or draws.empty()) return;
        draws[it].name = name;
        if (!icon.empty()) draws[it].icon = icon;
    }

    void setOffset(const std::vector<draw>::size_type it, const bool offset) {
        if (it > draws.size() or draws.empty()) return;
        draws[it].offset = offset;
    }

    void setUnavailable(const std::vector<draw>::size_type it, const bool unavailable) {
        if (it > draws.size() or draws.empty()) return;
        draws[it].unavailable = unavailable;
    }

};

class Menu
{
public:
    Menu(SDL_Renderer* rnd, json& sttg, Global& gInfo);
    ~Menu() = default;

    int MenuIndex;

	void addMenu(menuItem& newItem);
    bool setMenu(int id, int srtX = -1, int srtY = -1);
    void setWinSize(int WW, int WH);
    void closeMenu();
    void handleEvent();
    void render();

    [[nodiscard]] int getMenuId() const;
    int menuResultById(int id);
    std::string menuResultNameById(int id);

private:
    SDL_Renderer* renderer;
    json& settings;

    Global& g;

    int startX;
    int startY;
    int mouseX;
    int mouseY;

    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    int selectedItem;
    double showAnim;

    const int iconSize;

    menuResult res;
	std::vector<menuItem> menu;
};

#endif //NEOMYANIST_MENU_H