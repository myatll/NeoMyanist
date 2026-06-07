//
// Created by myat on 3/29/2026.
//

#include "Filters.h"

Filters::Filters(Global& gInfo, Menu& mnu, List& lst, Search& srch) :
    txtr(nullptr), g(gInfo), list(lst), MenuID(-1), menu(mnu), search(srch),
    updateTxtr(true), posRect{}, startDrawIndex(0), scrollAnim(0), currentIndex(-1)
{
    filterJson = {
        {"genres", {}},
        {"drawFilter", 0}
    };
}

void Filters::updateAvailableFilters(const std::map<std::string, int> &filters) {
    if (filters.empty()) {
        filterJson["genres"].clear();
        selectedFilters.clear();
        availableFilters.clear();
        g.drawFilters = false;
        g.needReSize = true;
    } else {
        std::vector<std::pair<std::string, int>> vec(filters.begin(), filters.end());
        for (const auto& filter: selectedFilters) {
            for (auto& item : vec) {
                if (filter == item.first) item.second = -1;
            }
        }
        std::ranges::sort(vec, [](const auto& a, const auto& b) {
            if (a.second == -1 && b.second != -1) return true;
            if (b.second == -1 && a.second != -1) return false;
            return a.second > b.second;
        });
        availableFilters = vec;
        if (g.drawCategories and search.mode) {
            g.drawFilters = true;
            g.needReSize = true;
        }
    }
    startDrawIndex = 0;
    updateTxtr = true;
}

void Filters::render() {

    if (updateTxtr) {
        // SDL_SetRenderDrawColor(g.renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
        // DrawCircle(g.renderer, posRect.x, posRect.y, 3);

        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, 0);
        SDL_SetRenderTarget(g.renderer, txtr);
        SDL_RenderClear(g.renderer);
        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, g.background.a);
        DrawRoundedRect(g.renderer, {0, 0, posRect.w, posRect.h}, 15);

        currentIndex = -1;
        int drawElemNum = static_cast<int>(scrollAnim / fullItemSize) - 1;
        while (drawElemNum*fullItemSize - static_cast<int>(scrollAnim) < posRect.h) {
            int drawElemIndex = drawElemNum + startDrawIndex;
            if (drawElemIndex >= 0) {
                if (drawElemIndex >= availableFilters.size()) break;
                int drawOffset = drawElemNum*fullItemSize - static_cast<int>(scrollAnim) + (itemSize >> 1);

                SDL_SetRenderDrawColor(g.renderer, g.aimedItem.r, g.aimedItem.g, g.aimedItem.b, g.aimedItem.a);
                if (availableFilters[drawElemIndex].second == -1) SDL_SetRenderDrawColor(g.renderer, g.selectedFilter.r, g.selectedFilter.g, g.selectedFilter.b, g.selectedFilter.a);
                if (MenuID == -1 and drawOffset - (fullItemSize >> 1) <= mouseY && mouseY <= drawOffset + (fullItemSize >> 1) && 5 <= mouseX && mouseX <= posRect.w - 5) {
                    currentIndex = drawElemIndex;
                    SDL_SetRenderDrawColor(g.renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
                }

                DrawRoundedRect(g.renderer, {10, drawElemNum * fullItemSize - static_cast<int>(scrollAnim), posRect.w - 20, itemSize}, 10);
                RenderText_Wrapped(g.renderer, availableFilters[drawElemIndex].first, 20, drawOffset, g.noSelectedText, g.lowFont, false, true, posRect.w - 60);

                if (availableFilters[drawElemIndex].second != -1) {
                    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(g.lowFont, std::to_string(availableFilters[drawElemIndex].second).c_str(), g.noSelectedText); SDL_Texture *textTexture = SDL_CreateTextureFromSurface(g.renderer, textSurface);
                    const SDL_Rect dstRect = {posRect.w - 20 - textSurface->w, drawOffset - (itemSize >> 1) + (textSurface->h >> 1), textSurface->w, textSurface->h};
                    SDL_RenderCopy(g.renderer, textTexture, nullptr, &dstRect);
                    SDL_DestroyTexture(textTexture);
                    SDL_FreeSurface(textSurface);
                } else {
                    RenderText(g.renderer, "backspace", posRect.w - 25, drawOffset, g.defaultText, g.iconsFont, true, true);
                }

            }
            ++drawElemNum;
        }

        updateTxtr = false;
        SDL_SetRenderTarget(g.renderer, nullptr);
    }

    SDL_RenderCopy(g.renderer, txtr, nullptr, &posRect);
}

void Filters::setPos(SDL_Rect newPos) {
    posRect = newPos;
    g.bg->setFilter(newPos, 30);
    if (txtr != nullptr) SDL_DestroyTexture(txtr);
    txtr = SDL_CreateTexture(g.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, posRect.w, posRect.h);
    SDL_SetTextureBlendMode(txtr, SDL_BLENDMODE_BLEND);
    updateTxtr = true;
}

void Filters::handle() {
    SDL_GetMouseState(&mouseX, &mouseY);

    mouseX-=posRect.x;
    mouseY-=posRect.y;
    if (0 <= mouseY && mouseY <= posRect.h && 0 <= mouseX && mouseX <= posRect.w and MenuID == -1) {
        updateTxtr = true;
        if (g.e.type == SDL_MOUSEWHEEL) {
            if (g.e.wheel.y > 0) {
                if (startDrawIndex - g.e.wheel.y > -1) {
                    scrollAnim += g.e.wheel.y * fullItemSize;
                    startDrawIndex -= g.e.wheel.y;
                }
            }
            else if (g.e.wheel.y < 0) {
                if (startDrawIndex - g.e.wheel.y + posRect.h / fullItemSize < availableFilters.size() +  1) {
                    scrollAnim += g.e.wheel.y * fullItemSize;
                    startDrawIndex -= g.e.wheel.y;
                }
            }

        } if (g.e.type == SDL_MOUSEBUTTONUP) {
            if (currentIndex != -1) {
                if (availableFilters[currentIndex].second != -1) {
                    selectedFilters.push_back(availableFilters[currentIndex].first);
                    filterJson["genres"] = selectedFilters;
                    search.update4Filters();
                } else {
                    for (std::vector<std::string>::size_type i = 0; i != selectedFilters.size(); ++i) {
                        if (availableFilters[currentIndex].first == selectedFilters[i]) {
                            selectedFilters.erase(selectedFilters.begin() + i);
                            filterJson["genres"] = selectedFilters;
                            search.update4Filters();
                            break;
                        }
                    }
                }
            }
        }

    } else if (currentIndex != -1) updateTxtr = true;
}

void Filters::update() {
    if ((-0.4 <= scrollAnim) xor (scrollAnim <= 0.4)) { scrollAnim += (0 - scrollAnim) * 0.1 ; updateTxtr = true; }

    if (MenuID != menu.getMenuId()) {
        MenuID = menu.getMenuId();
        updateTxtr = true;
    }
}

void Filters::updateDrawState() const {
    g.drawFilters = (g.drawCategories and !availableFilters.empty() and search.mode);
    g.needReSize = true;
}
