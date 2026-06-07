//
// Created by myat on 3/28/2026.
//

#include "Search.h"

#include "Filters.h"

Search::Search(Global& gInfo, Menu& mnu, List& lst, PipeClient& clnt) :
    txtr(nullptr), g(gInfo), menu(mnu), list(lst), client(clnt),
    updateTxtr(true), posRect{}, modeAnim(0), mouseX(0), mouseY(0), mode(true), search(gInfo.renderer, gInfo.defaultFont, gInfo.settings), categ(nullptr),
    status(0), opinion(0), MenuID(-1), isChange(false), sendTimer(0)
{
    search.init(g.e);
    search.setText("");
    // search.setText("");
    search.setActive(true);
    search.setColor(g.defaultText);

    menuItem opinionMenu;
    for (std::string name : g.settings["icon_names"]["opinion"]) {
        opinionMenu.add(name, "");
    }
    opinionMenu.type = 1;
    opinionMenu.id = 10;
    menu.addMenu(opinionMenu);

    menuItem statusMenu;
    for (std::string name : g.settings["icon_names"]["status"]) {
        statusMenu.add(name, "");
    }

    statusMenu.type = 1;
    statusMenu.id = 15;
    menu.addMenu(statusMenu);

}

void Search::setCategIter(Categories *categIter) {
    categ = categIter;
}

void Search::setFilterIter(Filters *filterIter) {
    filt = filterIter;
}

void Search::reloadCateg() {
    isChange = false;
    mode = true;
    opinion = 0;
    search.setText("");
    status = 0;
    currentItem = {
        {"opinion", 0},
        {"status", 0}
    };
    list.drawReset();
}

void Search::update4Filters() const {
    list.updateList(search.getText(), currentItem);
}

void Search::resetCurrentItem() {
    json resetItem = {
        {"opinion", currentItem["opinion"]},
        {"status", currentItem["status"]}
    };
    currentItem = resetItem;
}

void Search::setItem4Edit(std::string& name, json &data) {
    updateTxtr = true;
    mode = false;
    isChange = true;
    oldNameEdit = name;
    setTip(name, data);
    opinion = data["opinion"];
    status = data["status"];
    filt->updateAvailableFilters({});
}

void Search::setTip(std::string &name, json &data) {
    updateTxtr = true;
    for (auto it = data.begin(); it != data.end(); ++it) {
        currentItem[it.key()] = it.value();
    }
    search.setText(name);
    search.setActive(true);
}

void Search::update() {
    if (mode) {
        if (modeAnim > 0.4) { modeAnim += (0 - modeAnim) * 0.15; updateTxtr = true; }
    }
    else if (modeAnim < 199.6) { modeAnim += (200 - modeAnim) * 0.15; updateTxtr = true; }
    if (search.getActive() and search.changed()) {
        updateTxtr = true;
        if (!mode) {
            lastSend = true;
            sendTimer = SDL_GetTicks();
        } else {
            filt->updateAvailableFilters({});
            list.updateList(search.getText(), currentItem);
        }
    }

    if (lastSend and SDL_GetTicks() - sendTimer > 500 and !mode and !search.isEmpty()) {
        json msg;
        sendTimer = SDL_GetTicks();
        msg["findByName"] = {
            {"category", categ->getCategory()},
            {"name", search.getText()}
        };
        client.send(msg.dump() + "\n");
        lastSend = false;
    }

    if (MenuID != menu.getMenuId()) {
        MenuID = menu.getMenuId();

        if (int result_; (result_ = menu.menuResultById(10)) != -1) {
            opinion = result_;
            currentItem["opinion"] = opinion;
            if (mode) {
                filt->updateAvailableFilters({});
                list.updateList(search.getText(), currentItem);
            }
        } else if ((result_ = menu.menuResultById(15)) != -1) {
            status = result_;
            currentItem["status"] = status;
            if (mode) {
                filt->updateAvailableFilters({});
                list.updateList(search.getText(), currentItem);
            }
        }
        updateTxtr = true;
    }
}

void Search::render() {

    if (updateTxtr) {
        // SDL_SetRenderDrawColor(g.renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
        // DrawCircle(g.renderer, posRect.x, posRect.y, 3);

        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, 0);
        SDL_SetRenderTarget(g.renderer, txtr);
        SDL_RenderClear(g.renderer);
        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, g.background.a);
        DrawRoundedRect(g.renderer, {0, 0, posRect.w, posRect.h}, 15);
        if (!search.getActive()) SDL_SetRenderDrawColor(g.renderer, g.aimedItem.r, g.aimedItem.g, g.aimedItem.b, g.aimedItem.a);
        else SDL_SetRenderDrawColor(g.renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
        DrawRoundedRect(g.renderer, {80, 10, posRect.w - 160, posRect.h - 20}, (posRect.h - 21) >> 1);

        SDL_SetRenderDrawColor(g.renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
        if (isChange) SDL_RenderDrawLineF(g.renderer, 30, posRect.h - 1, posRect.w - 30, posRect.h - 1);
        if (currentItem.contains("id")) DrawCircle(g.renderer, posRect.w - 10, 10, 4);

        SDL_SetRenderDrawColor(g.renderer, g.aimedItem.r, g.aimedItem.g, g.aimedItem.b, g.aimedItem.a);
        const int radius = static_cast<int>(TTF_FontHeight(g.iconsFont) * 0.625);
        if ((posRect.h >> 1) - 15 <= mouseY && mouseY <= (posRect.h >> 1) + 15) {
            if (25 - 15 <= mouseX && mouseX <= 25 + 15) DrawCircle(g.renderer, 25, posRect.h >> 1, radius);
            if (55 - 15 <= mouseX && mouseX <= 55 + 15) DrawCircle(g.renderer, 55, posRect.h >> 1, radius);
            if (posRect.w - 55 - 15 <= mouseX && mouseX <= posRect.w - 55 + 15) DrawCircle(g.renderer, posRect.w - 55, posRect.h >> 1, radius);
            if (posRect.w - 25 - 15 <= mouseX && mouseX <= posRect.w - 25 + 15) DrawCircle(g.renderer, posRect.w - 25, posRect.h >> 1, radius);
        }

        RenderText(g.renderer, g.settings["icon_names"]["opinion"][opinion], posRect.w - 55, posRect.h >> 1, g.opinionIconColors[opinion], g.iconsFont, true, true);
        RenderText(g.renderer, g.settings["icon_names"]["status"][status], posRect.w - 25, posRect.h >> 1, g.statusIconColors[status], g.iconsFont, true, true);

        RenderText(g.renderer, "database_search", 25, posRect.h >> 1, {g.noSelectedText.r, g.noSelectedText.r, g.noSelectedText.r, static_cast<Uint8>(200 - modeAnim + 50)}, g.iconsFont, true, true);
        RenderText(g.renderer, "format_list_bulleted_add", 55, posRect.h >> 1, {g.noSelectedText.r, g.noSelectedText.r, g.noSelectedText.r, static_cast<Uint8>(modeAnim + 50)}, g.iconsFont, true, true);

        updateTxtr = false;
        SDL_SetRenderTarget(g.renderer, nullptr);
    }

    SDL_RenderCopy(g.renderer, txtr, nullptr, &posRect);
    search.render();
}

void Search::setPos(SDL_Rect newPos) {
    g.bg->setFilter(newPos, 30);
    posRect = newPos;
    if (txtr != nullptr) SDL_DestroyTexture(txtr);
    txtr = SDL_CreateTexture(g.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, posRect.w, posRect.h);
    SDL_SetTextureBlendMode(txtr, SDL_BLENDMODE_BLEND);
    updateTxtr = true;
    search.setSize({posRect.x + 90, posRect.y + (posRect.h >> 1) - (TTF_FontHeight(g.defaultFont) >> 1), posRect.w - 180, static_cast<int>(TTF_FontHeight(g.defaultFont) * 1.5)});
}

void Search::handle() {
    SDL_GetMouseState(&mouseX, &mouseY);
    search.handleEvent(mouseX, mouseY);
    mouseX-=posRect.x;
    mouseY-=posRect.y;
    if (0 <= mouseY && mouseY <= posRect.h && 0 <= mouseX && mouseX <= posRect.w and MenuID == -1) {
        updateTxtr = true;
        if (g.e.type == SDL_MOUSEBUTTONUP) {
            // if (e.button.button == SDL_BUTTON_RIGHT) {
                 if (10 <= mouseY && mouseY <= posRect.h - 10) {
                     if (25 - 25 <= mouseX && mouseX <= 25 + 15) {
                         mode = true; opinion = 0; status = 0;
                         currentItem["status"] = 0; currentItem["opinion"] = 0;
                         search.setText("");
                         // filt->updateAvailableFilters({});
                         list.drawReset();
                         filt->updateDrawState();
                         if (isChange) isChange = false;
                         resetCurrentItem();
                     }
                     if (55 - 15 <= mouseX && mouseX <= 55 + 25) {
                         mode = false;
                         opinion = 0;
                         status = 1;
                         currentItem["status"] = 1; currentItem["opinion"] = 0;
                         // filt->updateAvailableFilters({});
                         g.drawFilters = false;
                         g.needReSize = true;
                         list.updateTxtr = true;
                         list.drawReset();
                         resetCurrentItem();
                     }
                     if (posRect.w - 55 - 15 <= mouseX && mouseX <= posRect.w - 55 + 15) menu.setMenu(10, posRect.x + posRect.w - 55, posRect.y + (posRect.h >> 1));
                     if (posRect.w - 25 - 15 <= mouseX && mouseX <= posRect.w - 25 + 15) menu.setMenu(15, posRect.x + posRect.w - 25, posRect.y + (posRect.h >> 1));
                 }
            // }
        }
    } else {
        if (g.e.type == SDL_MOUSEBUTTONDOWN) {
            updateTxtr = true;
        }
    }

    if (g.e.type == SDL_KEYDOWN and search.getActive()) {
        if (!mode and !search.getText().empty()) {
            if (g.e.key.keysym.sym == SDLK_RETURN) {
                if (!isChange) {
                    search.setActive(false);
                    list.addItem(search.getText(), currentItem);
                    search.setText("");
                } else {
                    search.setActive(false);
                    list.returnEditedItem(oldNameEdit, search.getText(), currentItem);
                    search.setText("");
                    isChange = false;
                }
                mode = true;
                opinion = 0;
                status = 0;
                currentItem = {
                    {"opinion", 0},
                    {"status", 0}
                };
            } else if (g.e.key.keysym.sym == SDLK_ESCAPE) {
                updateTxtr = true;
                search.setText("");
                isChange = false;
                search.setActive(false);
                resetCurrentItem();
            }
        }
    }
}
