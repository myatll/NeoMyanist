#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include "src/PipeClient.h"
#include "src/Search.h"
#include "src/Menu.h"
#include "src/ItemInfo.h"
#include "src/List.h"
#include "src/Filters.h"
#include "src/Categories.h"
#include "src/Background.h"
#include "src/TextureLoader.h"
#include "src/structs.h"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    int WINDOW_WIDTH = 980;
    int WINDOW_HEIGHT = 690;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { SDL_Quit(); return 1; }
    if (TTF_Init() != 0) { SDL_Quit(); IMG_Quit(); return 1; }

    json settings;
    std::ifstream file("../data/theme.json");
    file >> settings;
    file.close();

    json state;
    file.open("../data/state.json");
    file >> state;
    file.close();

    SDL_Event e;

    Global gInfo {
        {static_cast<Uint8>(settings["color"]["all_window_back"][0]), static_cast<Uint8>(settings["color"]["all_window_back"][1]), static_cast<Uint8>(settings["color"]["all_window_back"][2]), static_cast<Uint8>(settings["color"]["all_window_back"][3])},
        {static_cast<Uint8>(settings["color"]["background"][0]), static_cast<Uint8>(settings["color"]["background"][1]), static_cast<Uint8>(settings["color"]["background"][2]), static_cast<Uint8>(settings["color"]["background"][3])},
        {static_cast<Uint8>(settings["color"]["menu_background"][0]), static_cast<Uint8>(settings["color"]["menu_background"][1]), static_cast<Uint8>(settings["color"]["menu_background"][2]), static_cast<Uint8>(settings["color"]["menu_background"][3])},
        {static_cast<Uint8>(settings["color"]["info_background"][0]), static_cast<Uint8>(settings["color"]["info_background"][1]), static_cast<Uint8>(settings["color"]["info_background"][2]), static_cast<Uint8>(settings["color"]["info_background"][3])},
        {static_cast<Uint8>(settings["color"]["item"][0]), static_cast<Uint8>(settings["color"]["item"][1]), static_cast<Uint8>(settings["color"]["item"][2]), static_cast<Uint8>(settings["color"]["item"][3])},
        {static_cast<Uint8>(settings["color"]["aimed_item"][0]), static_cast<Uint8>(settings["color"]["aimed_item"][1]), static_cast<Uint8>(settings["color"]["aimed_item"][2]), static_cast<Uint8>(settings["color"]["aimed_item"][3])},
        {static_cast<Uint8>(settings["color"]["selected_item"][0]), static_cast<Uint8>(settings["color"]["selected_item"][1]), static_cast<Uint8>(settings["color"]["selected_item"][2]), static_cast<Uint8>(settings["color"]["selected_item"][3])},
        {static_cast<Uint8>(settings["color"]["selected_filter"][0]), static_cast<Uint8>(settings["color"]["selected_filter"][1]), static_cast<Uint8>(settings["color"]["selected_filter"][2]), static_cast<Uint8>(settings["color"]["selected_filter"][3])},
        {static_cast<Uint8>(settings["color"]["aimed_menu_item"][0]), static_cast<Uint8>(settings["color"]["aimed_menu_item"][1]), static_cast<Uint8>(settings["color"]["aimed_menu_item"][2]), static_cast<Uint8>(settings["color"]["aimed_menu_item"][3])},
        {static_cast<Uint8>(settings["color"]["border"][0]), static_cast<Uint8>(settings["color"]["border"][1]), static_cast<Uint8>(settings["color"]["border"][2]), static_cast<Uint8>(settings["color"]["border"][3])},
        {},
        {},
        {static_cast<Uint8>(settings["font_color"]["default"][0]), static_cast<Uint8>(settings["font_color"]["default"][1]), static_cast<Uint8>(settings["font_color"]["default"][2]), 255},
        {static_cast<Uint8>(settings["font_color"]["genres"][0]), static_cast<Uint8>(settings["font_color"]["genres"][1]), static_cast<Uint8>(settings["font_color"]["genres"][2]), 255},
        {static_cast<Uint8>(settings["font_color"]["no_selected"][0]), static_cast<Uint8>(settings["font_color"]["no_selected"][1]), static_cast<Uint8>(settings["font_color"]["no_selected"][2]), 255},
        {static_cast<Uint8>(settings["font_color"]["menu"][0]), static_cast<Uint8>(settings["font_color"]["menu"][1]), static_cast<Uint8>(settings["font_color"]["menu"][2]), 255},
        {static_cast<Uint8>(settings["font_color"]["menu_unavailable"][0]), static_cast<Uint8>(settings["font_color"]["menu_unavailable"][1]), static_cast<Uint8>(settings["font_color"]["menu_unavailable"][2]), 255},

        TTF_OpenFont("../data/fonts/font.ttf", settings["font_size"]["general"]),
        TTF_OpenFont("../data/fonts/font.ttf", settings["font_size"]["low"]),
        TTF_OpenFont("../data/fonts/font.ttf", settings["font_size"]["big"]),
        TTF_OpenFont("../data/fonts/icons.ttf", settings["font_size"]["icons"]),
        TTF_OpenFont("../data/fonts/font.ttf", settings["font_size"]["menu"]),
        TTF_OpenFont("../data/fonts/icons.ttf", settings["font_size"]["menu_icons"]),

        settings, state, e
    };

    for (auto & i : settings["icon_colors"]["opinion"]) {
        gInfo.opinionIconColors.push_back({static_cast<Uint8>(i[0]), static_cast<Uint8>(i[1]), static_cast<Uint8>(i[2]), 255 });
    }
    for (auto & i : settings["icon_colors"]["status"]) {
        gInfo.statusIconColors.push_back({static_cast<Uint8>(i[0]), static_cast<Uint8>(i[1]), static_cast<Uint8>(i[2]), 255 });
    }

    const char* pipeName = R"(\\.\pipe\MyPipe)";
    HANDLE hPipe = CreateNamedPipeA(
        pipeName,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        1024,
        1024,
        0,
        NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cout << "CreateNamedPipe failed\n";
        return 1;
    }

    std::cout << "Waiting for client...\n";

    // запускаем Python клиент
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // ---------------------------------
    // std::string cmd = "../venv/Scripts/pythonw.exe debug.py";
    std::string cmd = "find.exe";

    if (!CreateProcessA(
        NULL,
        (LPSTR)cmd.c_str(),
        NULL, NULL, FALSE, 0, NULL, NULL,
        &si, &pi)) {
        std::cout << "Failed to start Python\n";
        return 1;
        }
    // ----------------------------------------

    // ждем подключения
    ConnectNamedPipe(hPipe, NULL);
    std::cout << "Client connected!\n";

    PipeClient client(hPipe);

    SDL_Window* window = SDL_CreateWindow("Myanist",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) { SDL_Quit(); return 1; }
    SDL_SetWindowMinimumSize(window, 520, 330);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) { SDL_DestroyWindow(window); SDL_Quit(); return 1; }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    gInfo.setRenderer(renderer);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    AsyncTextureLoader tloader(renderer);
    tloader.Start();

    Background bg(renderer);
    bg.loadOptImage(settings["background"]);
    gInfo.setBackground(&bg);

    SDL_Surface* surface = IMG_Load("../data/loading.png");
    gInfo.loadindImg = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    Menu menu(renderer, settings, gInfo);

    List list(gInfo, menu, client, tloader);
    Search search(gInfo, menu, list, client);
    Filters filters(gInfo, menu, list, search);
    ItemInfo itemInfo(gInfo, menu, list);
    list.setSearchIter(&search);
    list.setFiltersIter(&filters);
    list.setInfoIter(&itemInfo);
    Categories categories(gInfo, menu, list, search);

    bg.generatePattern(settings["background_pattern"]["type"], settings["background_pattern"]["amplitude"], settings["background_pattern"]["frequency1"], settings["background_pattern"]["frequency2"], settings["background_pattern"]["noise_intensity"]);
    bg.setPos({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

    list.setPos({ 360, 110, WINDOW_WIDTH - 610, WINDOW_HEIGHT - 140 });
    itemInfo.setPos({ 30, 110, 300, WINDOW_HEIGHT - 140 });
    search.setPos({ 30, 30, WINDOW_WIDTH - 280, 50 });
    filters.setPos({ WINDOW_WIDTH - 220, 110, 190, WINDOW_HEIGHT - 140 });
    categories.setPos({ WINDOW_WIDTH - 220, 30, 190, 50 });

    menu.setWinSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    list.setCategIter(&categories);
    search.setCategIter(&categories);
    search.setFilterIter(&filters);

    bool running = true;
    while (running) {
        Uint32 frameStart = SDL_GetTicks(); // Время начала кадра

        while (SDL_PollEvent(&e)) {
            menu.handleEvent();
            search.handle();
            list.handle();
            if (gInfo.drawInfo) itemInfo.handle();
            if (gInfo.drawCategories) categories.handle();
            if (gInfo.drawFilters) filters.handle();


            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_F5) {
                    file.open("../data/theme.json");
                    file >> settings;
                    file.close();

                    file.open("../data/state.json");
                    file >> state;
                    file.close();

                    gInfo.allWindowBack = {static_cast<Uint8>(settings["color"]["all_window_back"][0]), static_cast<Uint8>(settings["color"]["all_window_back"][1]), static_cast<Uint8>(settings["color"]["all_window_back"][2]), static_cast<Uint8>(settings["color"]["all_window_back"][3])};
                    gInfo.background = {static_cast<Uint8>(settings["color"]["background"][0]), static_cast<Uint8>(settings["color"]["background"][1]), static_cast<Uint8>(settings["color"]["background"][2]), static_cast<Uint8>(settings["color"]["background"][3])};
                    gInfo.menuBackground = {static_cast<Uint8>(settings["color"]["menu_background"][0]), static_cast<Uint8>(settings["color"]["menu_background"][1]), static_cast<Uint8>(settings["color"]["menu_background"][2]), static_cast<Uint8>(settings["color"]["menu_background"][3])};
                    gInfo.infoBackground = {static_cast<Uint8>(settings["color"]["info_background"][0]), static_cast<Uint8>(settings["color"]["info_background"][1]), static_cast<Uint8>(settings["color"]["info_background"][2]), static_cast<Uint8>(settings["color"]["info_background"][3])};
                    gInfo.item = {static_cast<Uint8>(settings["color"]["item"][0]), static_cast<Uint8>(settings["color"]["item"][1]), static_cast<Uint8>(settings["color"]["item"][2]), static_cast<Uint8>(settings["color"]["item"][3])};
                    gInfo.aimedItem = {static_cast<Uint8>(settings["color"]["aimed_item"][0]), static_cast<Uint8>(settings["color"]["aimed_item"][1]), static_cast<Uint8>(settings["color"]["aimed_item"][2]), static_cast<Uint8>(settings["color"]["aimed_item"][3])};
                    gInfo.selectedItem = {static_cast<Uint8>(settings["color"]["selected_item"][0]), static_cast<Uint8>(settings["color"]["selected_item"][1]), static_cast<Uint8>(settings["color"]["selected_item"][2]), static_cast<Uint8>(settings["color"]["selected_item"][3])};
                    gInfo.selectedFilter = {static_cast<Uint8>(settings["color"]["selected_filter"][0]), static_cast<Uint8>(settings["color"]["selected_filter"][1]), static_cast<Uint8>(settings["color"]["selected_filter"][2]), static_cast<Uint8>(settings["color"]["selected_filter"][3])};
                    gInfo.aimedMenuItem = {static_cast<Uint8>(settings["color"]["aimed_menu_item"][0]), static_cast<Uint8>(settings["color"]["aimed_menu_item"][1]), static_cast<Uint8>(settings["color"]["aimed_menu_item"][2]), static_cast<Uint8>(settings["color"]["aimed_menu_item"][3])};
                    gInfo.border = {static_cast<Uint8>(settings["color"]["border"][0]), static_cast<Uint8>(settings["color"]["border"][1]), static_cast<Uint8>(settings["color"]["border"][2]), static_cast<Uint8>(settings["color"]["border"][3])};
                    gInfo.defaultText = {static_cast<Uint8>(settings["font_color"]["default"][0]), static_cast<Uint8>(settings["font_color"]["default"][1]), static_cast<Uint8>(settings["font_color"]["default"][2]), 255};
                    gInfo.genresText = {static_cast<Uint8>(settings["font_color"]["genres"][0]), static_cast<Uint8>(settings["font_color"]["genres"][1]), static_cast<Uint8>(settings["font_color"]["genres"][2]), 255};
                    gInfo.noSelectedText = {static_cast<Uint8>(settings["font_color"]["no_selected"][0]), static_cast<Uint8>(settings["font_color"]["no_selected"][1]), static_cast<Uint8>(settings["font_color"]["no_selected"][2]), 255};
                    gInfo.menuText = {static_cast<Uint8>(settings["font_color"]["menu"][0]), static_cast<Uint8>(settings["font_color"]["menu"][1]), static_cast<Uint8>(settings["font_color"]["menu"][2]), 255};
                    gInfo.menuUnavailableText = {static_cast<Uint8>(settings["font_color"]["menu_unavailable"][0]), static_cast<Uint8>(settings["font_color"]["menu_unavailable"][1]), static_cast<Uint8>(settings["font_color"]["menu_unavailable"][2]), 255};

                    gInfo.opinionIconColors.clear();
                    gInfo.statusIconColors.clear();
                    for (auto & i : settings["icon_colors"]["opinion"]) {gInfo.opinionIconColors.push_back({static_cast<Uint8>(i[0]), static_cast<Uint8>(i[1]), static_cast<Uint8>(i[2]), 255 });}
                    for (auto & i : settings["icon_colors"]["status"]) {gInfo.statusIconColors.push_back({static_cast<Uint8>(i[0]), static_cast<Uint8>(i[1]), static_cast<Uint8>(i[2]), 255 });}
                }
            }
            else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                SDL_GetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
                bg.setPos({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

                menu.setWinSize(WINDOW_WIDTH, WINDOW_HEIGHT);

                gInfo.drawCategories = WINDOW_WIDTH > 900;
                if (!gInfo.drawCategories) {
                    gInfo.drawFilters = false;
                    gInfo.drawInfo = false;
                }
                filters.updateDrawState();
                gInfo.needReSize = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, gInfo.background.r, gInfo.background.g, gInfo.background.b, 0);
        SDL_RenderClear(renderer);

        bg.render();
        SDL_SetRenderDrawColor(renderer, gInfo.allWindowBack.r, gInfo.allWindowBack.g, gInfo.allWindowBack.b, gInfo.allWindowBack.a);
        SDL_Rect fillRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &fillRect);

        if (gInfo.needReSize) {
            bg.setPos({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});
            if (gInfo.drawInfo) itemInfo.setPos({ 30, 110, 300, WINDOW_HEIGHT - 140 });

            if (gInfo.drawCategories) {
                categories.setPos({ WINDOW_WIDTH - 220, 30, 190, 50 });
                search.setPos({ 30, 30, WINDOW_WIDTH - 280, 50 });
            } else search.setPos({ 30, 30, WINDOW_WIDTH - 60, 50 });

            if (gInfo.drawFilters) filters.setPos({ WINDOW_WIDTH - 220, 110, 190, WINDOW_HEIGHT - 140 });

            list.setPos({30 + gInfo.drawInfo * 330, 110, WINDOW_WIDTH - 60 - gInfo.drawInfo * 330 - gInfo.drawFilters * 220, WINDOW_HEIGHT - 140 });
            gInfo.needReSize = false;
        }

        list.update();
        search.update();
        if (gInfo.drawInfo) itemInfo.update();
        if (gInfo.drawCategories) categories.update();
        if (gInfo.drawFilters) filters.update();

        search.render();
        list.render();
        if (gInfo.drawInfo) itemInfo.render();
        if (gInfo.drawCategories) categories.render();
        if (gInfo.drawFilters) filters.render();


        menu.render();

        // RenderText(renderer, std::to_string(WINDOW_WIDTH), 20, 20, gInfo.selectedItem, gInfo.iconsFont, false, true);

        SDL_RenderPresent(renderer);

        if (SDL_GetTicks() - frameStart < 16) {
            SDL_Delay(16 - SDL_GetTicks() + frameStart); // Задержка, чтобы уложиться в лимит
        }
        // RenderText(renderer, std::to_string(1000 / (SDL_GetTicks() - frameStart)), 20, 20, gInfo.selectedItem, gInfo.iconsFont, false, true);
        // SDL_RenderPresent(renderer);
    }

    tloader.Stop();

    TTF_CloseFont(gInfo.defaultFont);
    TTF_CloseFont(gInfo.lowFont);
    TTF_CloseFont(gInfo.bigFont);
    TTF_CloseFont(gInfo.iconsFont);
    TTF_CloseFont(gInfo.menuFont);
    TTF_CloseFont(gInfo.menuIconsFont);

    TTF_Quit();
    SDL_Quit();
    return 0;
}