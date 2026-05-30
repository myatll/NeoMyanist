//
// Created by myatll on 8/19/25.
//

#include "MyatllDraw.h"
#include <SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <SDL.h>

void RenderText(SDL_Renderer *renderer, const std::string &text, int x, int y,
                SDL_Color color, TTF_Font *useFont, bool x_centered, bool y_centered,
                int maxWidth) {
    std::string renderedText = text;

    // Если задано ограничение по ширине
    if (maxWidth > 0) {
        int textWidth = 0;
        std::string tempText = text;
        std::string ellipsis = "...";

        // Проверяем, помещается ли текст целиком
        TTF_SizeUTF8(useFont, tempText.c_str(), &textWidth, nullptr);

        // Если текст не помещается
        if (textWidth > maxWidth) {
            // Находим максимальное количество слов, которое помещается
            size_t lastSpace = 0;

            while (true) {
                lastSpace = tempText.find_last_of(" \t\n", lastSpace - 1);

                if (lastSpace == std::string::npos) {
                    // Если пробелов нет, просто обрезаем по символам
                    while (!tempText.empty()) {
                        TTF_SizeUTF8(useFont, (tempText + ellipsis).c_str(), &textWidth, nullptr);
                        if (textWidth <= maxWidth) break;
                        tempText.pop_back();
                    }
                    break;
                }

                std::string truncated = tempText.substr(0, lastSpace) + ellipsis;
                TTF_SizeUTF8(useFont, truncated.c_str(), &textWidth, nullptr);

                if (textWidth <= maxWidth) {
                    renderedText = truncated;
                    break;
                }

                if (lastSpace == 0) break;
            }
        }
    }

    SDL_Surface *surface = TTF_RenderUTF8_Blended(useFont, renderedText.c_str(), color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    if (x_centered) rect.x = rect.x - rect.w / 2;
    if (y_centered) rect.y = rect.y - rect.h / 2;


    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void RenderText_Wrapped(SDL_Renderer *renderer, const std::string &text, int x, int y, SDL_Color color,
                        TTF_Font *useFont, bool x_centered, bool y_centered, int x_transfer) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(useFont, text.c_str(), color, x_transfer);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!surface) return;
    if (!texture) return;
    SDL_Rect rect = {x, y, surface->w, surface->h};
    if (x_centered) rect.x = rect.x - rect.w / 2;
    if (y_centered) rect.y = rect.y - rect.h / 2;

    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void DrawCircle(SDL_Renderer *renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void DrawArc(SDL_Renderer *renderer, int centerX, int centerY, int radius, int startAngle, int endAngle) {
    int segments = radius << 2;
    double angleStep = (endAngle - startAngle) * M_PI / 180.0 / segments;

    for (int i = 0; i < segments; ++i) {
        double angle1 = startAngle * M_PI / 180.0 + i * angleStep;
        double angle2 = startAngle * M_PI / 180.0 + (i + 1) * angleStep;

        int x1 = int(centerX + radius * cos(angle1));
        int y1 = int(centerY + radius * sin(angle1));
        int x2 = int(centerX + radius * cos(angle2));
        int y2 = int(centerY + radius * sin(angle2));

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void DrawQuadrantCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, int quadrant) {
    int drawX = 0;
    int drawY = 0;
    for (int x = 0; x <= radius; x++) {
        for (int y = 0; y <= radius; y++) {
            if (x * x + y * y <= radius * radius) {
                if (quadrant == 1) {
                    drawX = centerX + x;
                    drawY = centerY - y;
                } else if (quadrant == 2) {
                    drawX = centerX - x;
                    drawY = centerY - y;
                } else if (quadrant == 3) {
                    drawX = centerX - x;
                    drawY = centerY + y;
                } else if (quadrant == 4) {
                    drawX = centerX + x;
                    drawY = centerY + y;
                }
                SDL_RenderDrawPoint(renderer, drawX, drawY);
            }
        }
    }
}

void DrawInQuadrantCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, int quadrant) {
    int drawX = 0;
    int drawY = 0;
    for (int x = 0; x <= radius; x++) {
        for (int y = 0; y <= radius; y++) {
            if (x * x + y * y > radius * radius) {
                if (quadrant == 1) {
                    drawX = centerX + x;
                    drawY = centerY - y;
                } else if (quadrant == 2) {
                    drawX = centerX - x;
                    drawY = centerY - y;
                } else if (quadrant == 3) {
                    drawX = centerX - x;
                    drawY = centerY + y;
                } else if (quadrant == 4) {
                    drawX = centerX + x;
                    drawY = centerY + y;
                }
                SDL_RenderDrawPoint(renderer, drawX, drawY);
            }
        }
    }
}

void DrawRoundedRect(SDL_Renderer *renderer, SDL_Rect rect, int radius) {
    for (int x = 0; x <= radius; x++) {
        for (int y = 0; y <= radius; y++) {
            if (x * x + y * y < radius * radius) {
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + x, rect.y + radius - y);
                SDL_RenderDrawPoint(renderer, rect.x + radius - x, rect.y + radius - y);
                SDL_RenderDrawPoint(renderer, rect.x + radius - x, rect.y + rect.h - radius + y);
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + x, rect.y + rect.h - radius + y);
            }
        }
    }

    SDL_Rect fillRect = {rect.x + radius + 1, rect.y, rect.w - radius * 2 - 1, rect.h};
    SDL_RenderFillRect(renderer, &fillRect);
    fillRect = {rect.x + rect.w - radius, rect.y + radius + 1, radius, rect.h - radius * 2 - 1};
    SDL_RenderFillRect(renderer, &fillRect);
    fillRect = {rect.x + 1, rect.y + radius + 1, radius, rect.h - radius * 2 - 1};
    SDL_RenderFillRect(renderer, &fillRect);
}

void DrawBut(SDL_Renderer *renderer, int x_, int y_, int w_, int radius, SDL_Color clr) {

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                if (dx < 0) SDL_RenderDrawPoint(renderer, x_ - (w_ >> 1) + dx, y_ + dy);
                else if (dx > 0) SDL_RenderDrawPoint(renderer, x_ + (w_ >> 1) + dx, y_ + dy);
            }
        }
    }

    SDL_Rect fillRect = {x_ - (w_ >> 1), y_ - radius + 1, w_, (radius << 1) - 1};
    if (!(fillRect.w % 2)) fillRect.w += 1;
    SDL_RenderFillRect(renderer, &fillRect);

    if (clr.a == 0) return;
    SDL_SetRenderDrawColor(renderer, clr.r, clr.g, clr.b, clr.a);

    DrawArc(renderer, x_ - w_ / 2, y_, radius, 90, 270);
    DrawArc(renderer, x_ + w_ / 2, y_, radius, 270, 450);

    SDL_RenderDrawLineF(renderer, x_ - (w_ >> 1), y_ - radius, x_ + (w_ >> 1), y_ - radius);
    SDL_RenderDrawLineF(renderer, x_ - (w_ >> 1), y_ + radius, x_ + (w_ >> 1), y_ + radius);
}


void DRRwB(SDL_Renderer *renderer, SDL_Rect rect, int radius) {

    DrawArc(renderer, rect.x + radius, rect.y + radius, radius, 180, 270);
    DrawArc(renderer, rect.x + rect.w - radius, rect.y + radius, radius, 270, 360);
    DrawArc(renderer, rect.x + radius, rect.y + rect.h - radius, radius, 90, 180);
    DrawArc(renderer, rect.x + rect.w - radius, rect.y + rect.h - radius, radius, 0, 90);


    SDL_RenderDrawLineF(renderer, rect.x + radius, rect.y, rect.x + rect.w - radius - 1, rect.y);
    SDL_RenderDrawLineF(renderer, rect.x + radius, rect.y + rect.h - 1, rect.x + rect.w - radius - 1,
                        rect.y + rect.h - 1);
    SDL_RenderDrawLineF(renderer, rect.x, rect.y + radius, rect.x, rect.y + rect.h - radius - 1);
    SDL_RenderDrawLineF(renderer, rect.x + rect.w - 1, rect.y + radius, rect.x + rect.w - 1,
                        rect.y + rect.h - radius - 1);

}

void DRRwB_dev(SDL_Renderer *renderer, SDL_Rect rect, int radius, int segments) {
    auto DrawArc_dev{[&](int centerX, int centerY, int radius, int startAngle, int endAngle){
        double angleStep = (endAngle - startAngle) * M_PI / 180.0 / segments;

        for (int i = 0; i < segments; ++i) {
            double angle1 = startAngle * M_PI / 180.0 + i * angleStep;
            double angle2 = startAngle * M_PI / 180.0 + (i + 1) * angleStep;

            int x1 = int(centerX + radius * cos(angle1));
            int y1 = int(centerY + radius * sin(angle1));
            int x2 = int(centerX + radius * cos(angle2));
            int y2 = int(centerY + radius * sin(angle2));

            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }};

    DrawArc_dev(rect.x + radius, rect.y + radius, radius, 180, 270);
    DrawArc_dev(rect.x + rect.w - radius, rect.y + radius, radius, 270, 360);
    DrawArc_dev(rect.x + radius, rect.y + rect.h - radius, radius, 90, 180);
    DrawArc_dev(rect.x + rect.w - radius, rect.y + rect.h - radius, radius, 0, 90);


    SDL_RenderDrawLineF(renderer, rect.x + radius, rect.y, rect.x + rect.w - radius - 1, rect.y);
    SDL_RenderDrawLineF(renderer, rect.x + radius, rect.y + rect.h, rect.x + rect.w - radius - 1,
                        rect.y + rect.h);
    SDL_RenderDrawLineF(renderer, rect.x, rect.y + radius, rect.x, rect.y + rect.h - radius - 1);
    SDL_RenderDrawLineF(renderer, rect.x + rect.w, rect.y + radius, rect.x + rect.w,
                        rect.y + rect.h - radius - 1);

}