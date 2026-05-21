//
// Created by myatll on 8/19/25.
//

#include "Background.h"

Background::Background(SDL_Renderer* rnd) :
    x_offset{0}, y_offset{0}, renderer(rnd)
{

}

Background::~Background() {

}

void Background::loadOptImage(const std::string &filepath) {
    SDL_Surface *loadedSurface = IMG_Load(filepath.c_str());
    if (!loadedSurface) return;
    load_img = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(loadedSurface);
}

void Background::cropToAspectRatio() {

    float imgAspect = (float)load_img->w / load_img->h;
    float targetAspect = (float)posRect.w / posRect.h;

    SDL_Surface* resizedSurface = nullptr;
    SDL_Rect cropRect = { 0, 0, posRect.w, posRect.h };

    if (targetAspect > imgAspect) {
        // Окно шире - обрезаем по вертикали
        int newHeight = (int)(posRect.w / imgAspect);
        resizedSurface = SDL_CreateRGBSurfaceWithFormat(
                0, posRect.w, newHeight, 32, load_img->format->format
        );
        if (!resizedSurface) return;

        // Масштабируем изображение
        SDL_BlitScaled(
            load_img, nullptr,
            resizedSurface, nullptr
        );

        // Вычисляем область обрезки (центрированную)
        cropRect.x = 0;
        cropRect.y = (newHeight - posRect.h) / 2;
        cropRect.w = posRect.w;
        cropRect.h = posRect.h;
    }
    else {
        // Окно уже - обрезаем по горизонтали
        int newWidth = (int)(posRect.h * imgAspect);
        resizedSurface = SDL_CreateRGBSurfaceWithFormat(
                0, newWidth, posRect.h, 32, load_img->format->format
        );
        if (!resizedSurface) return;

        // Масштабируем изображение
        SDL_BlitScaled(
                load_img, nullptr,
                resizedSurface, nullptr
        );

        // Вычисляем область обрезки (центрированную)
        cropRect.x = (newWidth - posRect.w) / 2;
        cropRect.y = 0;
        cropRect.w = posRect.w;
        cropRect.h = posRect.h;
    }

    // Создаём поверхность для результата (обрезанного изображения)
    SDL_Surface* croppedSurface = SDL_CreateRGBSurfaceWithFormat(
            0, posRect.w, posRect.h, 32, load_img->format->format
    );
    if (!croppedSurface) {
        SDL_FreeSurface(resizedSurface);
        return;
    }

    // Копируем обрезанную часть
    SDL_BlitSurface(
            resizedSurface, &cropRect,
            croppedSurface, nullptr
    );

    // Освобождаем временную поверхность
    SDL_FreeSurface(resizedSurface);
    crop_img = croppedSurface;
    background = SDL_CreateTextureFromSurface(renderer, crop_img);
}

void Background::generatePattern(int index_, float amplitude, float frequency1, float frequency2, float noiseIntensity) {
    if (index_ == 0) {
        for (int i = 0; i < block_sz; ++i) {
            for (int j = 0; j < block_sz; ++j) {
                x_offset[i][j] = std::rand() % int(amplitude) - (int(amplitude) >> 1);  // Случайное число от 1 до 16
                y_offset[i][j] = std::rand() % int(amplitude) - (int(amplitude) >> 1);  // Случайное число от 1 до 16
            }
        }
    }
    else if (index_ == 1) {
        for (int y = 0; y < block_sz; ++y) {
            for (int x = 0; x < block_sz; ++x) {
                float wave1 = sin(x * frequency1 + y * 0.05f) * cos(y * frequency1 * 0.7f);
                float wave2 = cos(x * frequency2 * 0.5f + y * frequency2) * sin(y * 0.03f);
                float noiseX = (rand() % 100) / 100.0f * noiseIntensity;
                float noiseY = (rand() % 100) / 100.0f * noiseIntensity;
                x_offset[x][y] = int((wave1 + wave2 * 0.7f) * amplitude + noiseX);
                y_offset[x][y] = int((wave2 + wave1 * 0.3f) * amplitude + noiseY);
            }
        }
    }
    else if (index_ == 2) {
        for (int y = 0; y < block_sz; ++y) {
            for (int x = 0; x < block_sz; ++x) {
                float dx = x - block_sz / 2;
                float dy = y - block_sz / 2;
                float distance = sqrt(dx * dx + dy * dy);
                float angle = atan2(dy, dx);
                x_offset[x][y] = int(cos(angle + distance * 0.1f) * amplitude);
                y_offset[x][y] = int(sin(angle + distance * 0.1f) * amplitude);
            }
        }
    }
    else if (index_ == 3) {
        for (int y = 0; y < block_sz; ++y) {
            for (int x = 0; x < block_sz; ++x) {
                float dx = x - block_sz / 2;
                float dy = y - block_sz / 2;
                float distance = sqrt(dx * dx + dy * dy);
                float maxDistance = block_sz / 2;

                if (distance < maxDistance) {
                    // Нормализуем расстояние (0..1)
                    float normDist = distance / maxDistance;
                    // Квадратичное затухание эффекта к краям
                    float factor = (1.0f - normDist * normDist) * amplitude;

                    x_offset[x][y] = dx * factor;
                    y_offset[x][y] = dy * factor;
                }

            }
        }
    }
}

void Background::setPos(SDL_Rect prct) {
    posRect = prct;
    cropToAspectRatio();
}

void Background::render() {
    SDL_RenderCopy(renderer, background, nullptr, &posRect);
}

void Background::setFilter(SDL_Rect rct, int radius, bool relative, bool unSafe) {
    if (!relative) {
        rct.x -= posRect.x;
        rct.y -= posRect.y;
    }
    if (rct.x < 0) rct.x = 0;
    if (rct.y < 0) rct.y = 0;
    if (rct.x + rct.w > crop_img->w) rct.w = crop_img->w - rct.x;
    if (rct.y + rct.h > crop_img->h) rct.h = crop_img->h - rct.y;

    if (rct.w <= 0 || rct.h <= 0) return;

    // Ограничиваем радиус скругления
    radius = std::min(radius, std::min(rct.w, rct.h) / 2);

    // Создаем временный буфер для хранения изменений
    Uint32* buffer = (Uint32*)malloc(rct.w * rct.h * sizeof(Uint32));
    if (!buffer) return;

    // Блокируем поверхность
    if (SDL_MUSTLOCK(crop_img)) {
        SDL_LockSurface(crop_img);
    }

    // Сначала читаем все исходные пиксели в буфер
    for (int i = 0; i < rct.h; i++) {
        for (int j = 0; j < rct.w; j++) {
            int surface_x = rct.x + j;
            int surface_y = rct.y + i;

            if (surface_x < crop_img->w && surface_y < crop_img->h) {
                buffer[i * rct.w + j] = get_pixel(crop_img, surface_x, surface_y);
            }
        }
    }

    // Функция для проверки, находится ли пиксель в скругленной области
    auto isInRoundedArea = [&](int x, int y) -> bool {
        if (radius <= 0) return true; // Без скругления

        int centerX, centerY;

        // Проверяем углы
        // Левый верхний угол
        if (x < radius && y < radius) {
            centerX = radius;
            centerY = radius;
            return (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius;
        }
            // Правый верхний угол
        else if (x > rct.w - radius - 1 && y < radius) {
            centerX = rct.w - radius - 1;
            centerY = radius;
            return (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius;
        }
            // Левый нижний угол
        else if (x < radius && y > rct.h - radius - 1) {
            centerX = radius;
            centerY = rct.h - radius - 1;
            return (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius;
        }
            // Правый нижний угол
        else if (x > rct.w - radius - 1 && y > rct.h - radius - 1) {
            centerX = rct.w - radius - 1;
            centerY = rct.h - radius - 1;
            return (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius;
        }

        return true; // Не в угловой области - применяем фильтр
    };

    // Теперь обрабатываем пиксели, используя исходные данные из буфера
    for (int i = 0; i < rct.h; i++) {
        for (int j = 0; j < rct.w; j++) {
            // Проверяем, нужно ли применять фильтр к этому пикселю
            if (radius != 0 and !isInRoundedArea(j, i)) {
                continue; // Пропускаем пиксели в скругленных углах
            }

            int surface_x = rct.x + j;
            int surface_y = rct.y + i;

            // Вычисляем индексы для массивов смещений
            int offset_x_idx = j % block_sz;
            int offset_y_idx = i % block_sz;

            // Получаем смещения
            int dx = x_offset[offset_x_idx][offset_y_idx];
            int dy = y_offset[offset_x_idx][offset_y_idx];

            // Вычисляем координаты пикселя-источника в буфере
            int src_x_in_buffer = j + dx;
            int src_y_in_buffer = i + dy;

            // Проверяем границы в буфере
            if ((src_x_in_buffer >= 0 && src_x_in_buffer < rct.w &&
                src_y_in_buffer >= 0 && src_y_in_buffer < rct.h) or unSafe) {

                // Получаем цвет из буфера (исходные данные)
                Uint32 source_color = buffer[src_y_in_buffer * rct.w + src_x_in_buffer];

                // Записываем результат на поверхность
                put_pixel(crop_img, surface_x, surface_y, source_color);
            }
        }
    }

    // Разблокируем поверхность
    if (SDL_MUSTLOCK(crop_img)) {
        SDL_UnlockSurface(crop_img);
    }

    free(buffer);
    background = SDL_CreateTextureFromSurface(renderer, crop_img);
}

// Альтернативная версия с использованием SDL_MapRGB для безопасности
void Background::modify_pixels_safe(SDL_Rect rct) {
    if (SDL_MUSTLOCK(crop_img)) {
        SDL_LockSurface(crop_img);
    }

    for (int i = rct.y; i < rct.y + rct.h && i < crop_img->h; i++) {
        for (int j = rct.x; j < rct.x + rct.w && j < crop_img->w; j++) {
            int offset_x_idx = (j - rct.x) % block_sz;
            int offset_y_idx = (i - rct.y) % block_sz;

            int dx = x_offset[offset_x_idx][offset_y_idx];
            int dy = y_offset[offset_x_idx][offset_y_idx];

            int src_x = j + dx;
            int src_y = i + dy;

            if (src_x >= 0 && src_x < crop_img->w && src_y >= 0 && src_y < crop_img->h) {
                // Получаем цвет пикселя-источника
                Uint32 pixel = get_pixel(crop_img, src_x, src_y);
                put_pixel(crop_img, j, i, pixel);
            }
        }
    }

    if (SDL_MUSTLOCK(crop_img)) {
        SDL_UnlockSurface(crop_img);
    }
    background = SDL_CreateTextureFromSurface(renderer, crop_img);
}

// Вспомогательные функции для безопасного чтения/записи пикселей
Uint32 Background::get_pixel(const SDL_Surface* surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = static_cast<Uint8 *>(surface->pixels) + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1: return *p;
        case 2: return *reinterpret_cast<Uint16 *>(p);
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4: return *reinterpret_cast<Uint32 *>(p);
        default: return 0;
    }
}

void Background::put_pixel(const SDL_Surface* surface, const int x, const int y, const Uint32 pixel) {
    const int bpp = surface->format->BytesPerPixel;
    Uint8* p = static_cast<Uint8 *>(surface->pixels) + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1: *p = pixel; break;
        case 2: *reinterpret_cast<Uint16 *>(p) = pixel; break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4: *reinterpret_cast<Uint32 *>(p) = pixel; break;
        default: ;
    }
}
