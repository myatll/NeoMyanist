//
// Created by myatll on 8/19/25.
//

#ifndef SPIDER_MYATLLDRAW_H
#define SPIDER_MYATLLDRAW_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>

void RenderText(SDL_Renderer *renderer, const std::string &text, int x, int y,
                SDL_Color color, TTF_Font *useFont, int xDeviation = 0, int yDeviation = 0,
                int maxWidth = -1);

void RenderText_Wrapped(SDL_Renderer *renderer, const std::string &text, int x, int y, SDL_Color color,
                        TTF_Font *useFont, bool x_centered = false, bool y_centered = false, int x_transfer = 200);

void DrawCircle(SDL_Renderer *renderer, int x, int y, int radius);

void DrawArc(SDL_Renderer *renderer, int centerX, int centerY, int radius, int startAngle, int endAngle);

void DrawQuadrantCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, int quadrant);

void DrawInQuadrantCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius, int quadrant);

void DrawRoundedRect(SDL_Renderer *renderer, SDL_Rect rect, int radius);

void DrawBut(SDL_Renderer *renderer, int x_, int y_, int w_, int radius, SDL_Color clr = {0, 0, 0, 0});

void DRRwB(SDL_Renderer *renderer, SDL_Rect rect, int radius);
void DRRwB_dev(SDL_Renderer *renderer, SDL_Rect rect, int radius, int segments);

#endif