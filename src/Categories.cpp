//
// Created by myat on 3/29/2026.
//

#include "Categories.h"

Categories::Categories(Global &gInfo, Menu& mnu, List& lst, Search& srch) :
    txtr(nullptr), g(gInfo), menu(mnu), list(lst), search(srch),
    updateTxtr(true), posRect{}, modeAnim(0), mouseX(0), mouseY(0), MenuID(-1)
{
    std::string path = "../items/";
    path += g.state["categories"][g.state["currentСategory"]]["dir"];
    path += "/list.json";
    // path +=  + "\\list.json";
    list.loadCateg(path);
    // std::cout <<  path << std::endl;

    menuItem selectCategory;
    selectCategory.add("", "Добавить");
    for (auto it = g.state["categories"].begin(); it != g.state["categories"].end(); ++it) {
    // for (auto name : g.state["categories"]) {
        selectCategory.add("", it.key());
    }
    selectCategory.setUnavailable(0, true);
    selectCategory.setOffset(1, true);

    selectCategory.type = 2;
    selectCategory.id = 30;
    menu.addMenu(selectCategory);

    menuItem changeCategory;
    changeCategory.add("edit", "Изменить");
    changeCategory.add("delete", "Удалить");

    changeCategory.type = 0;
    changeCategory.id = 35;
    menu.addMenu(changeCategory);
}

std::string Categories::getCategory() const {
    return g.state["currentСategory"];
}

void Categories::update() {
    if (MenuID != menu.getMenuId()) {
        updateTxtr = true;
        MenuID = menu.getMenuId();

        if (std::string result = menu.menuResultNameById(30); !result.empty()) {
            if (result == "Добавить") {

            } else {
                g.state["currentСategory"] = result;
                std::string path = "../items/";
                path += g.state["categories"][g.state["currentСategory"]]["dir"];
                path += "/list.json";
                // path +=  + "\\list.json";
                list.loadCateg(path);
                search.reloadCateg();
            }
        }
    }
}

void Categories::render() {
    if (updateTxtr) {
        // SDL_SetRenderDrawColor(g.renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
        // DrawCircle(g.renderer, posRect.x, posRect.y, 3);

        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, 0);
        SDL_SetRenderTarget(g.renderer, txtr);
        SDL_RenderClear(g.renderer);
        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, g.background.a);
        DrawRoundedRect(g.renderer, {0, 0, posRect.w, posRect.h}, 15);

        if (MenuID != 30) RenderText_Wrapped(g.renderer, getCategory(), posRect.w >> 1, posRect.h >> 1, g.noSelectedText, g.defaultFont, true, true, posRect.w - 10);


        updateTxtr = false;
        SDL_SetRenderTarget(g.renderer, nullptr);
    }

    SDL_RenderCopy(g.renderer, txtr, nullptr, &posRect);
}

void Categories::setPos(SDL_Rect newPos) {
    g.bg->setFilter(newPos, 30);
    posRect = newPos;
    if (txtr != nullptr) SDL_DestroyTexture(txtr);
    txtr = SDL_CreateTexture(g.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, posRect.w, posRect.h);
    SDL_SetTextureBlendMode(txtr, SDL_BLENDMODE_BLEND);
    updateTxtr = true;
}

void Categories::handle() {
    SDL_GetMouseState(&mouseX, &mouseY);
    mouseX-=posRect.x;
    mouseY-=posRect.y;
    if (0 <= mouseY && mouseY <= posRect.h && 0 <= mouseX && mouseX <= posRect.w and MenuID == -1) {
        updateTxtr = true;
        if (g.e.type == SDL_MOUSEBUTTONUP) {
            if (g.e.button.button == SDL_BUTTON_LEFT) menu.setMenu(30, posRect.x + posRect.w / 2, posRect.y + posRect.h / 2);
            else if (g.e.button.button == SDL_BUTTON_RIGHT) menu.setMenu(35, mouseX + posRect.x, mouseY + posRect.y);
        }
    }
}
