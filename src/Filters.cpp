//
// Created by myat on 3/29/2026.
//

#include "Filters.h"

Filters::Filters(SDL_Renderer* rnd, TTF_Font* fnt, TTF_Font* icns, json& sttg, Global& gInfo, SDL_Event &ev) :
    renderer(rnd), font(fnt), icons(icns), settings(sttg), e(ev), txtr(nullptr), g(gInfo),
    updateTxtr(true), posRect{}, startDrawIndex(0), scrollAnim(0), currentIndex(-1){


}

void Filters::render() {
    if ((-0.4 <= scrollAnim) xor (scrollAnim <= 0.4)) { scrollAnim += (0 - scrollAnim) * 0.1; updateTxtr = true; }

    if (updateTxtr) {
        SDL_SetRenderDrawColor(renderer, g.allwindowback.r, g.allwindowback.g, g.allwindowback.b, 0);
        SDL_SetRenderTarget(renderer, txtr);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, g.background.r, g.background.g, g.background.b, g.background.a);

        DrawRoundedRect(renderer, {0, 0, posRect.w, posRect.h}, 15);

        updateTxtr = false;
        SDL_SetRenderTarget(renderer, nullptr);
    }

    SDL_RenderCopy(renderer, txtr, nullptr, &posRect);
}

void Filters::setPos(SDL_Rect newPos) {
    posRect = newPos;
    if (txtr != nullptr) SDL_DestroyTexture(txtr);
    txtr = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, posRect.w, posRect.h);
    SDL_SetTextureBlendMode(txtr, SDL_BLENDMODE_BLEND);
    updateTxtr = true;
}
