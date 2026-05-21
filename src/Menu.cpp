//
// Created by myat on 4/2/2026.
//

#include "Menu.h"

Menu::Menu(SDL_Renderer* rnd, json& sttg, Global& gInfo, SDL_Event &ev):
    renderer(rnd), settings(sttg), g(gInfo), e(ev), iconSize(TTF_FontHeight(g.menuIconsFont)),
    MenuIndex(-1), startX(0), startY(0), mouseX(0), mouseY(0), selectedItem(-1), showAnim(0)
{

}

void Menu::addMenu(menuItem& newItem) {
    int maxW = iconSize + (iconSize >> 1);
    int countOfOffset = 0;
    if (newItem.type == 0) {
        for (int i = 0; i != newItem.draws.size(); ++i) {
            int testW;
            TTF_SizeUTF8(g.menuFont, newItem.draws[i].name.c_str(), &testW, nullptr);
            if (newItem.draws[i].offset) ++countOfOffset;
            testW += iconSize << 1;
            if (testW > maxW) maxW = testW;
        }
    } else if (newItem.type == 2){
        for (int i = 0; i != newItem.draws.size(); ++i) {
            int testW;
            TTF_SizeUTF8(g.menuFont, newItem.draws[i].name.c_str(), &testW, nullptr);
            testW += iconSize;
            if (newItem.draws[i].offset) ++countOfOffset;
            if (testW > maxW) maxW = testW;
        }
    }
    newItem.setSize(maxW, (iconSize >> 2) * countOfOffset + iconSize * static_cast<int>(newItem.draws.size()) + (iconSize >> 1));
    menu.push_back(newItem);
}

bool Menu::setMenu(const int id, const int srtX, const int srtY) {
    if (menu.empty()) return false;
    for (std::vector<menuItem>::size_type item = 0; item < menu.size(); ++item) {
        if (menu[item].id == id) {
            if (srtX != -1) startX = srtX;
            if (srtY != -1) startY = srtY;
            if (menu[item].type == 0) {
                if (startY + menu[item].h > WINDOW_HEIGHT) startY -= menu[item].h;
                if (startX + menu[item].w > WINDOW_WIDTH) startX -= menu[item].w;
            }
            MenuIndex = item;
            return true;
        }
    }
    return false;
}

void Menu::setWinSize(const int WW, const int WH) {
    WINDOW_WIDTH = WW;
    WINDOW_HEIGHT = WH;
}

void Menu::closeMenu() {
    MenuIndex = -1;
}

void Menu::handleEvent() {
	SDL_GetMouseState(&mouseX, &mouseY);
    if (e.type == SDL_MOUSEBUTTONUP) {
        if (MenuIndex != -1) {
            // if (startX >= mouseX or mouseX >= startX + menu[MenuIndex].w or startY >= mouseY or mouseY >= startY + menu[MenuIndex].h) closeMenu();
            if (selectedItem == -1) closeMenu();
            else if (!menu[MenuIndex].draws[selectedItem].unavailable and e.button.button == SDL_BUTTON_LEFT){
                res.notProcessed = true;
                res.value = selectedItem;
                res.lastId = getMenuId();
                closeMenu();
            }
        }

    } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        closeMenu();
    }
}

void Menu::render() {
	if (MenuIndex == -1) return;
    if (menu[MenuIndex].type == 1) {
        SDL_SetRenderDrawColor(renderer, g.menuBackground.r, g.menuBackground.g, g.menuBackground.b, g.menuBackground.a);
        DrawRoundedRect(renderer, {startX - static_cast<int>(iconSize * 0.75), startY - static_cast<int>(iconSize * 0.75), iconSize + (iconSize >> 1), menu[MenuIndex].h}, static_cast<int>(iconSize * 0.75) - 1);
        SDL_SetRenderDrawColor(renderer, g.border.r, g.border.g, g.border.b, g.border.a);
        DRRwB_dev(renderer, {startX - static_cast<int>(iconSize * 0.75), startY - static_cast<int>(iconSize * 0.75), iconSize + (iconSize >> 1), menu[MenuIndex].h}, static_cast<int>(iconSize * 0.75) - 1, 7);

        SDL_SetRenderDrawColor(renderer, g.aimedMenuItem.r, g.aimedMenuItem.g, g.aimedMenuItem.b, g.aimedMenuItem.a);

        selectedItem = -1;
        for (int i = 0; i != menu[MenuIndex].draws.size(); ++i) {
            if (startX - (iconSize >> 1) <= mouseX && mouseX <= startX + (iconSize >> 1) && startY + i * iconSize - (iconSize >> 1) <= mouseY && mouseY <= startY + i * iconSize + (iconSize >> 1)) {
                DrawCircle(renderer, startX, startY + i * iconSize, static_cast<int>(iconSize * 0.625));
                selectedItem = i;
            }
            RenderText(renderer, menu[MenuIndex].draws[i].icon, startX, startY + i * iconSize, g.iconColors[i], g.menuIconsFont, true, true);
        }
    }
    else if (menu[MenuIndex].type == 0) {
        SDL_SetRenderDrawColor(renderer, g.menuBackground.r, g.menuBackground.g, g.menuBackground.b, g.menuBackground.a);
        DrawRoundedRect(renderer, {startX, startY, menu[MenuIndex].w, menu[MenuIndex].h}, static_cast<int>(iconSize * 0.75) - 1);
        SDL_SetRenderDrawColor(renderer, g.border.r, g.border.g, g.border.b, g.border.a);
        DRRwB_dev(renderer, {startX, startY, menu[MenuIndex].w, menu[MenuIndex].h}, static_cast<int>(iconSize * 0.75) - 1, 7);

        SDL_SetRenderDrawColor(renderer, g.aimedMenuItem.r, g.aimedMenuItem.g, g.aimedMenuItem.b, g.aimedMenuItem.a);

        selectedItem = -1;
        int offsetY = 0;
        for (int i = 0; i != menu[MenuIndex].draws.size(); ++i) {
            if (menu[MenuIndex].draws[i].offset) {
                offsetY+=(iconSize >> 2);
                SDL_SetRenderDrawColor(renderer, g.border.r, g.border.g, g.border.b, g.border.a);
                SDL_RenderDrawLineF(renderer, startX + static_cast<int>(iconSize * 0.75), startY + i * iconSize + offsetY + (iconSize >> 3), startX + menu[MenuIndex].w - static_cast<int>(iconSize * 0.75), startY + i * iconSize + offsetY + (iconSize >> 3));
                SDL_SetRenderDrawColor(renderer, g.aimedMenuItem.r, g.aimedMenuItem.g, g.aimedMenuItem.b, g.aimedMenuItem.a);
            }
            if (startX + static_cast<int>(iconSize * 0.75) - (iconSize >> 1) <= mouseX && mouseX <= startX + menu[MenuIndex].w && startY + static_cast<int>(iconSize * 0.75) + i * iconSize - (iconSize >> 1) + offsetY <= mouseY && mouseY <= startY + static_cast<int>(iconSize * 0.75) + i * iconSize + (iconSize >> 1) + offsetY) {
                // DrawCircle(renderer, startX + static_cast<int>(iconSize * 0.75), startY + static_cast<int>(iconSize * 0.75) + i * iconSize, static_cast<int>(iconSize * 0.625));
                DrawBut(renderer, startX + (menu[MenuIndex].w >> 1), startY + static_cast<int>(iconSize * 0.75) + i * iconSize + offsetY, menu[MenuIndex].w - static_cast<int>(iconSize * 1.5), static_cast<int>(iconSize * 0.625));
                selectedItem = i;
            }
            if (menu[MenuIndex].draws[i].unavailable) {
                RenderText(renderer, menu[MenuIndex].draws[i].icon, startX + static_cast<int>(iconSize * 0.75), startY + static_cast<int>(iconSize * 0.75) + i * iconSize + offsetY, g.menuUnavailableText, g.menuIconsFont, true, true);
                RenderText(renderer, menu[MenuIndex].draws[i].name, startX + static_cast<int>(iconSize * 1.5), startY + static_cast<int>(iconSize * 0.75) + i * iconSize + offsetY, g.menuUnavailableText, g.menuFont, false, true);
            } else {
                RenderText(renderer, menu[MenuIndex].draws[i].icon, startX + static_cast<int>(iconSize * 0.75), startY + static_cast<int>(iconSize * 0.75) + i * iconSize + offsetY, g.menuText, g.menuIconsFont, true, true);
                RenderText(renderer, menu[MenuIndex].draws[i].name, startX + static_cast<int>(iconSize * 1.5), startY + static_cast<int>(iconSize * 0.75) + i * iconSize + offsetY, g.menuText, g.menuFont, false, true);
            }
        }
    } else {
        SDL_SetRenderDrawColor(renderer, g.menuBackground.r, g.menuBackground.g, g.menuBackground.b, g.menuBackground.a);
        DrawRoundedRect(renderer, {startX - (menu[MenuIndex].w >> 1), startY - static_cast<int>(iconSize * 0.75), menu[MenuIndex].w, menu[MenuIndex].h}, static_cast<int>(iconSize * 0.75) - 1);
        SDL_SetRenderDrawColor(renderer, g.border.r, g.border.g, g.border.b, g.border.a);
        DRRwB_dev(renderer, {startX - (menu[MenuIndex].w >> 1), startY - static_cast<int>(iconSize * 0.75), menu[MenuIndex].w, menu[MenuIndex].h}, static_cast<int>(iconSize * 0.75) - 1, 7);

        SDL_SetRenderDrawColor(renderer, g.aimedMenuItem.r, g.aimedMenuItem.g, g.aimedMenuItem.b, g.aimedMenuItem.a);

        selectedItem = -1;
        int offsetY = 0;
        for (int i = 0; i != menu[MenuIndex].draws.size(); ++i) {
            if (menu[MenuIndex].draws[i].offset) {
                offsetY+=(iconSize >> 2);
                SDL_SetRenderDrawColor(renderer, g.border.r, g.border.g, g.border.b, g.border.a);
                SDL_RenderDrawLineF(renderer, startX - (menu[MenuIndex].w >> 1) + static_cast<int>(iconSize * 0.75), startY + i * iconSize + offsetY - (iconSize >> 1) - (iconSize >> 3), startX + (menu[MenuIndex].w >> 1) - static_cast<int>(iconSize * 0.75), startY + i * iconSize + offsetY - (iconSize >> 1) - (iconSize >> 3));
                SDL_SetRenderDrawColor(renderer, g.aimedMenuItem.r, g.aimedMenuItem.g, g.aimedMenuItem.b, g.aimedMenuItem.a);
            }
            if (startX - (menu[MenuIndex].w >> 1) <= mouseX && mouseX <= startX + (menu[MenuIndex].w >> 1) && startY - (iconSize >> 1) + i * iconSize + offsetY <= mouseY && mouseY <= startY + (iconSize >> 1) + i * iconSize + offsetY) {
                DrawBut(renderer, startX, startY + i * iconSize + offsetY, menu[MenuIndex].w - static_cast<int>(iconSize * 1.5), static_cast<int>(iconSize * 0.625));
                selectedItem = i;
            }
            if (menu[MenuIndex].draws[i].unavailable) RenderText(renderer, menu[MenuIndex].draws[i].name, startX, startY + i * iconSize + offsetY, g.menuUnavailableText, g.menuFont, true, true);
            else RenderText(renderer, menu[MenuIndex].draws[i].name, startX, startY + i * iconSize + offsetY, g.menuText, g.menuFont, true, true);
        }
    }
}

int Menu::getMenuId() const
{
    if (MenuIndex == -1) return -1;
    return menu[MenuIndex].id;
}

int Menu::menuResultById(const int id) {
    if (!res.notProcessed) return -1;
    if (id == res.lastId) { res.notProcessed = false; return res.value; }
    return -1;
}
std::string Menu::menuResultNameById(const int id) {
    if (!res.notProcessed) return "";
    if (id == res.lastId) {
        for (auto & item : menu) {
            if (item.id == id) {
                res.notProcessed = false; return item.draws[res.value].name;
            }
        }
    }
    return "";
}
