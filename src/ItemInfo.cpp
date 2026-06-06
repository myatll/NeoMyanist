//
// Created by myat on 3/29/2026.
//

#include "ItemInfo.h"

ItemInfo::ItemInfo(Global &gInfo, Menu& mnu, List& lst) :
    txtr(nullptr), g(gInfo), menu(mnu), list(lst), posterAspect(1), drawOffset(0), offsetAnim(0),
    updateTxtr(true), posRect{}, mouseX(0), mouseY(0), MenuID(-1), poster(nullptr)
{

}

void ItemInfo::setPoster(SDL_Texture *posterTxtr, const int id, const double aspect) {
    // if (currentItem["id"] != id) return;
    if (poster != nullptr) SDL_DestroyTexture(poster);
    posterAspect = aspect;
    poster = posterTxtr;
    updateTxtr = true;
}

void ItemInfo::setItem(const json &Item) {
    if (poster != nullptr) SDL_DestroyTexture(poster);
    poster = nullptr;
    currentItem = Item;
    updateTxtr = true;
    drawOffset = 10;
    offsetAnim = 0;
    if (g.drawCategories) {
        g.drawInfo = true;
        g.needReSize = true;
    }
}

void ItemInfo::addInfo(const json &Info) {
    if (currentItem.empty()) return;
    if (currentItem["id"] != Info["id"]) return;
    for (auto it = Info.begin(); it != Info.end(); ++it) {
        if (!currentItem.contains(it.key())) currentItem[it.key()] = it.value();
    }

    /*
    category "Фильмы"
    description "1960-е годы. После закрытия нью-йоркского ночного клуба на ремонт вышибала Тони по прозвищу Болтун ищет подработку на пару месяцев. Как раз в это время Дон Ширли, утончённый светский лев, богатый и талантливый чернокожий музыкант, исполняющий классическую музыку, собирается в турне по южным штатам, где ещё сильны расистские убеждения и царит сегрегация. Он нанимает Тони в качестве водителя, телохранителя и человека, способного решать текущие проблемы. У этих двоих так мало общего, и эта поездка навсегда изменит жизнь обоих."
    duration "2 часа 10 минут"
    genres ["Драма","Комедия","Исторический"]
    id "2381"
    imdbRating 8.2
    kinopoiskRating 8.55
    miniPoster "https://media.myshows.me/movies/280f/2/a3/2a31b1f1f2c2e001b42a4f3e9e0a739a.jpg"
    name "Зелёная книга"
    poster "https://media.myshows.me/movies/normal/2/a3/2a31b1f1f2c2e001b42a4f3e9e0a739a.jpg"
    rating 9.16
    releaseDate "16 ноября 2018"
    service "MyShows"
    year "2018 год"
    */

    // for (auto it = currentItem.begin(); it != currentItem.end(); ++it) {
    //     std::cout << it.key() << " " << it.value() << std::endl;
    // }
}

void ItemInfo::update() {
    if ((-0.4 <= offsetAnim) xor (offsetAnim <= 0.4)) {offsetAnim += (0 - offsetAnim) * 0.1; updateTxtr = true;}
}

void ItemInfo::render() {
    if (updateTxtr) {
        // SDL_SetRenderDrawColor(g.renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
        // DrawCircle(g.renderer, posRect.x, posRect.y, 3);

        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, 0);
        SDL_SetRenderTarget(g.renderer, txtr);
        SDL_RenderClear(g.renderer);
        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, g.background.a);
        DrawRoundedRect(g.renderer, {0, 0, posRect.w, posRect.h}, 15);

        SDL_Rect posterRect = {10, drawOffset + static_cast<int>(offsetAnim), posRect.w - 20, 30};
        if (poster != nullptr) {
            posterRect.h = static_cast<int>((posterRect.w) / posterAspect);
            SDL_RenderCopy(g.renderer, poster, nullptr, &posterRect);
        }

        if (!currentItem.empty()) {
            SDL_SetRenderDrawColor(g.renderer, g.infoBackground.r, g.infoBackground.g, g.infoBackground.b, g.infoBackground.a);
            SDL_Rect subRect = {10, drawOffset + static_cast<int>(offsetAnim), posRect.w - 20, 30};
            SDL_RenderFillRect(g.renderer, &subRect);
            if (currentItem.contains("rating")) RenderText(g.renderer, std::format("{:.2f}", currentItem["rating"].get<double>()), posterRect.x + posterRect.w - 10, posterRect.y + 15, g.noSelectedText, g.bigFont, -1, 1);
            if (currentItem.contains("year")) RenderText(g.renderer, currentItem["year"], posterRect.x + 10, posterRect.y + 15, g.noSelectedText, g.bigFont, 0, 1);
            if (currentItem.contains("duration")) {
                if (posterRect.h == 30) posterRect.h = 60;
                subRect.y += posterRect.h - 30;
                SDL_RenderFillRect(g.renderer, &subRect);
                RenderText(g.renderer, currentItem["duration"], posterRect.x + (posterRect.w >> 1), posterRect.y + posterRect.h - 15, g.noSelectedText, g.bigFont, 1, 1);
            }

            SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(g.defaultFont, currentItem["name"].get<std::string>().c_str(), g.noSelectedText, posRect.w - 40);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(g.renderer, surface);
            SDL_Rect rect = {((posRect.w - surface->w) >> 1), posterRect.y + posterRect.h + 5, surface->w, surface->h};
            subRect.y = rect.y - 5;
            subRect.h = rect.h + 10;

            int radius = 10;
            for (int x = 0; x <= radius; x++) {
                for (int y = 0; y <= radius; y++) {
                    if (x * x + y * y < radius * radius) {
                        SDL_RenderDrawPoint(g.renderer, subRect.x + radius - x, subRect.y + subRect.h - radius + y);
                        SDL_RenderDrawPoint(g.renderer, subRect.x + subRect.w - radius + x, subRect.y + subRect.h - radius + y);
                    }
                }
            }

            SDL_Rect fillRect = {subRect.x, subRect.y, subRect.w, subRect.h - radius};
            SDL_RenderFillRect(g.renderer, &fillRect);
            fillRect = {subRect.x + radius + 1, subRect.y + subRect.h - radius, subRect.w - radius * 2 - 1, radius};
            SDL_RenderFillRect(g.renderer, &fillRect);

            SDL_RenderCopy(g.renderer, texture, nullptr, &rect);
            subRect.y = rect.y + rect.h + 5;


            SDL_FreeSurface(surface); SDL_DestroyTexture(texture);

            if (currentItem.contains("genres")) {
                std::vector<std::string> v = currentItem["genres"].get<std::vector<std::string>>(); std::string result;
                for (size_t i = 0; i < v.size(); ++i) {result += v[i]; if (i < v.size() - 1) result += ", ";}
                surface = TTF_RenderUTF8_Blended_Wrapped(g.defaultFont, result.c_str(), g.genresText, posRect.w - 20);
                texture = SDL_CreateTextureFromSurface(g.renderer, surface);
                rect = {((posRect.w - surface->w) >> 1), subRect.y + (TTF_FontHeight(g.defaultFont) >> 1), surface->w, surface->h};
                subRect.y = rect.y + rect.h;
                SDL_RenderCopy(g.renderer, texture, nullptr, &rect);

                SDL_FreeSurface(surface); SDL_DestroyTexture(texture);
            }
            if (currentItem.contains("description")) {
                surface = TTF_RenderUTF8_Blended_Wrapped(g.lowFont, currentItem["description"].get<std::string>().c_str(), g.noSelectedText, posRect.w - 20);
                texture = SDL_CreateTextureFromSurface(g.renderer, surface);
                rect = {10, subRect.y + (TTF_FontHeight(g.defaultFont) >> 1), surface->w, surface->h};
                subRect.y = rect.y + rect.h;
                SDL_RenderCopy(g.renderer, texture, nullptr, &rect);

                SDL_FreeSurface(surface); SDL_DestroyTexture(texture);
            }
            if (currentItem.contains("releaseDate")) {
                const std::string date = "Дата выхода: " + currentItem["releaseDate"].get<std::string>();
                surface = TTF_RenderUTF8_Blended_Wrapped(g.defaultFont, date.c_str(), g.noSelectedText, posRect.w - 20);
                texture = SDL_CreateTextureFromSurface(g.renderer, surface);
                rect = {10, subRect.y + (TTF_FontHeight(g.defaultFont) >> 1), surface->w, surface->h};
                subRect.y = rect.y + rect.h;
                SDL_RenderCopy(g.renderer, texture, nullptr, &rect);

                SDL_FreeSurface(surface); SDL_DestroyTexture(texture);
            }
            if (currentItem.contains("addTime")) {
                std::chrono::seconds duration(currentItem["addTime"]);
                std::chrono::system_clock::time_point tp(duration);
                auto tp_seconds = std::chrono::time_point_cast<std::chrono::seconds>(tp);
                std::chrono::zoned_time zt{std::chrono::current_zone(), tp_seconds};
                std::string date = "Время добавления: " + std::format("{:%d.%m.%Y %H:%M:%S}", zt);

                surface = TTF_RenderUTF8_Blended_Wrapped(g.defaultFont, date.c_str(), g.noSelectedText, posRect.w - 20);
                texture = SDL_CreateTextureFromSurface(g.renderer, surface);
                rect = {10, subRect.y + (TTF_FontHeight(g.defaultFont) >> 1), surface->w, surface->h};
                subRect.y = rect.y + rect.h;
                SDL_RenderCopy(g.renderer, texture, nullptr, &rect);

                SDL_FreeSurface(surface); SDL_DestroyTexture(texture);
            }
        }

        SDL_SetRenderDrawColor(g.renderer, g.background.r, g.background.g, g.background.b, g.background.a);
        const int radius = TTF_FontHeight(g.iconsFont) >> 1;
        DrawCircle(g.renderer, posRect.w - radius, radius - 1, radius);
        RenderText(g.renderer, "close", posRect.w - radius, radius - 1, g.statusIconColors[1], g.iconsFont, 1, 1);

        updateTxtr = false;
        SDL_SetRenderTarget(g.renderer, nullptr);
    }

    SDL_RenderCopy(g.renderer, txtr, nullptr, &posRect);
}

void ItemInfo::setPos(const SDL_Rect newPos) {
    g.bg->setFilter(newPos, 30);
    posRect = newPos;
    if (txtr != nullptr) SDL_DestroyTexture(txtr);
    txtr = SDL_CreateTexture(g.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, posRect.w, posRect.h);
    SDL_SetTextureBlendMode(txtr, SDL_BLENDMODE_BLEND);
    updateTxtr = true;
    // drawOffset = 10;
    offsetAnim = 0;
}

void ItemInfo::handle() {
    SDL_GetMouseState(&mouseX, &mouseY);

    mouseX-=posRect.x;
    mouseY-=posRect.y;
    if (0 <= mouseY && mouseY <= posRect.h && 0 <= mouseX && mouseX <= posRect.w and MenuID == -1) {
        if (g.e.type == SDL_MOUSEWHEEL) {
            updateTxtr = true;
            const int step = g.e.wheel.y << 5;
            if (g.e.wheel.y > 0) {
                if (drawOffset + step < 10) {
                    drawOffset += step;
                    offsetAnim -= step;
                } else {
                    offsetAnim += drawOffset - 10;
                    drawOffset = 10;
                }

            }
            else if (g.e.wheel.y < 0) {
                drawOffset += step;
                offsetAnim -= step;
            }
        } else if (g.e.type == SDL_MOUSEBUTTONUP) {
            if (mouseY <= TTF_FontHeight(g.iconsFont) && posRect.w - TTF_FontHeight(g.iconsFont) <= mouseX) {
                g.drawInfo = false;
                g.needReSize = true;
            }
        }
    }
}