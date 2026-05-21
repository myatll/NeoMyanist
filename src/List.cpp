//
// Created by myat on 3/23/2026.
//

#include "List.h"

#include "Search.h"

List::List(SDL_Renderer *rnd, json &sttg, Global& gInfo, SDL_Event &ev, Menu& mnu, PipeClient& clnt, AsyncTextureLoader& tldr) :
    renderer(rnd), settings(sttg), e(ev), txtr(nullptr), g(gInfo), menu(mnu), client(clnt), tloader(tldr),
    updateTxtr(true), posRect{}, startDrawIndex(0), scrollAnim(0), currentIndex(-1), categ(nullptr),
    mouseX(0), mouseY(0), ctrl(false), needSave(false)
{

    menuItem changeItemMenu;

    changeItemMenu.add("edit", "Изменить");
    changeItemMenu.add("delete", "Удалить");
    changeItemMenu.add("captive_portal", "Найти на сайте");
    changeItemMenu.add("image_search", "Узнать подробности");
    changeItemMenu.add("favorite", "Пометить любимым");
    changeItemMenu.add("note_stack_add", "Добавить пометку");

    changeItemMenu.setOffset(2, true);
    changeItemMenu.setOffset(4, true);
    changeItemMenu.setUnavailable(3, true);
    changeItemMenu.setUnavailable(4, true);
    changeItemMenu.setUnavailable(5, true);
    changeItemMenu.type = 0;
    changeItemMenu.id = 22;
    menu.addMenu(changeItemMenu);

    menuItem changeTipMenu;

    changeTipMenu.add("captive_portal", "Найти на сайте");
    changeTipMenu.add("image_search", "Узнать подробности");

    changeTipMenu.setUnavailable(1, true);
    changeTipMenu.type = 0;
    changeTipMenu.id = 25;
    menu.addMenu(changeTipMenu);

    SDL_Surface* surface = IMG_Load("../data/loading.png");
    loadindImg = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

}

void List::setSearchIter(Search *searchIter) {
    search = searchIter;
}

void List::setCategIter(Categories *categIter) {
    categ = categIter;
}

void List::returnEditedItem(std::string &oldName, std::string newName, json& currentItem) {
    if (!data.contains(oldName)) return;
    data.erase(oldName);
    data[newName] = currentItem;
    updateList("", nullptr);

    for (std::vector<listDrawItem>::size_type i = 0; i != list.size(); ++i) {
        if (list[i].name == newName) {
            if (i > 1) {
                startDrawIndex = i - 1;
                scrollAnim = -fullItemSize;
            }
            else startDrawIndex = 0;
            break;
        }
    }
    needSave = true;
}

void List::addItem(std::string name, json& currentItem) {
    if (data.contains(name)) return;
    auto now = std::chrono::system_clock::now();
    time_t now_time = std::chrono::system_clock::to_time_t(now);
    json newItem = currentItem;
    newItem["addTime"] = now_time;

    data[name] = newItem;
    updateList("", nullptr);

    for (std::vector<listDrawItem>::size_type i = 0; i != list.size(); ++i) {
        if (list[i].name == name) {
            std::cout << i << std::endl;
            if (i > 1) {
                startDrawIndex = i - 1;
                scrollAnim = -fullItemSize;
            }
            else startDrawIndex = 0;
            break;
        }
    }
    needSave = true;
}

void List::render() {

    if (updateTxtr) {
        SDL_SetRenderDrawColor(renderer, g.allwindowback.r, g.allwindowback.g, g.allwindowback.b, 0);
        SDL_SetRenderTarget(renderer, txtr);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, g.background.r, g.background.g, g.background.b, g.background.a);

        DrawRoundedRect(renderer, {0, 0, posRect.w, posRect.h}, 15);

        if (search->mode) {
            // RenderText(renderer, list[0].name, 10, 10, text_color, font);
            currentIndex = -1;
            int drawElemNum = static_cast<int>(scrollAnim / fullItemSize) - 1;
            while (drawElemNum*fullItemSize - static_cast<int>(scrollAnim) < posRect.h) {
                int drawElemIndex = drawElemNum + startDrawIndex;
                if (drawElemIndex >= 0) {
                    if (drawElemIndex >= list.size()) break;
                    int drawOffset = drawElemNum*fullItemSize - static_cast<int>(scrollAnim) + (itemSize >> 1);

                    if (data[list[drawElemIndex].name].contains("id")) {
                        SDL_SetRenderDrawColor(renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
                        SDL_RenderDrawLineF(renderer, 10, drawOffset - (itemSize >> 2), 10, drawOffset + (itemSize >> 2));
                    }

                    if ((MenuID == -1 and drawOffset - (itemSize >> 1) <= mouseY && mouseY <= drawOffset + (itemSize >> 1) && 5 <= mouseX && mouseX <= posRect.w - 5) or MenuID == 22 and drawElemIndex == changeItemIndex) {
                        currentIndex = drawElemIndex;
                        if (MenuID == -1) SDL_SetRenderDrawColor(renderer, g.aimedItem.r, g.aimedItem.g, g.aimedItem.b, g.aimedItem.a);
                        else SDL_SetRenderDrawColor(renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
                        DrawRoundedRect(renderer, {10, drawElemNum * fullItemSize - static_cast<int>(scrollAnim), posRect.w - 20, itemSize}, 10);
                        if (!ctrl) {
                            RenderText(renderer, settings["icon_names"]["opinion"][list[drawElemIndex].opinion], posRect.w - 55, drawOffset, g.iconColors[list[drawElemIndex].opinion], g.iconsFont, true, true);
                            RenderText(renderer, settings["icon_names"]["status"][list[drawElemIndex].status], posRect.w - 25, drawOffset, g.iconColors[list[drawElemIndex].status], g.iconsFont, true, true);
                        }
                        RenderText(renderer, list[drawElemIndex].name, 20, drawOffset, g.defaultText, g.defaultFont, false, true, posRect.w - 80);
                    } else
                        RenderText(renderer, list[drawElemIndex].name, 20, drawOffset, g.noSelectedText, g.defaultFont, false, true, posRect.w - 40 - ctrl*40);


                    if (ctrl) {
                        RenderText(renderer, settings["icon_names"]["opinion"][list[drawElemIndex].opinion], posRect.w - 55, drawOffset, g.iconColors[list[drawElemIndex].opinion], g.iconsFont, true, true);
                        RenderText(renderer, settings["icon_names"]["status"][list[drawElemIndex].status], posRect.w - 25, drawOffset, g.iconColors[list[drawElemIndex].status], g.iconsFont, true, true);
                    }

                }
                ++drawElemNum;
            }

        } else {
            currentIndex = -1;
            int drawElemNum = static_cast<int>(scrollAnim / fullTipSize) - 1;
            while (drawElemNum*fullTipSize - static_cast<int>(scrollAnim) < posRect.h) {
                int drawElemIndex = drawElemNum + startDrawIndex;
                if (drawElemIndex >= 0) {
                    if (drawElemIndex >= tips.size()) break;
                    int drawOffset = drawElemNum*fullTipSize - static_cast<int>(scrollAnim) + (tipSize >> 1);
                    if ((MenuID == -1 and drawOffset - (tipSize >> 1) <= mouseY && mouseY <= drawOffset + (tipSize >> 1) && 5 <= mouseX && mouseX <= posRect.w - 5) or MenuID == 25 and drawElemIndex == changeItemIndex) {
                        currentIndex = drawElemIndex;
                        if (MenuID == -1) SDL_SetRenderDrawColor(renderer, g.aimedItem.r, g.aimedItem.g, g.aimedItem.b, g.aimedItem.a);
                        else SDL_SetRenderDrawColor(renderer, g.selectedItem.r, g.selectedItem.g, g.selectedItem.b, g.selectedItem.a);
                        DrawRoundedRect(renderer, {10, drawElemNum * fullTipSize - static_cast<int>(scrollAnim), posRect.w - 20, tipSize}, 10);
                    }
                    SDL_Rect rect{20, drawOffset - (tipSize >> 1) + 10, 60, 94};
                    if (tips[drawElemIndex].miniPosterLoaded) {
                        if (!tips[drawElemIndex].miniPosterIsHozisontal) SDL_RenderCopy(renderer, tips[drawElemIndex].miniPoster, nullptr, &rect);
                        else {
                            rect.w = 167;
                            SDL_RenderCopy(renderer, tips[drawElemIndex].miniPoster, nullptr, &rect);
                        }
                    }
                    else SDL_RenderCopy(renderer, loadindImg, nullptr, &rect);

                    RenderText_Wrapped(renderer, tips[drawElemIndex].name, rect.w + 30, drawOffset - (tipSize >> 1) + 10, g.defaultText, g.bigFont, false, false, posRect.w - 180);

                    RenderText(renderer, tips[drawElemIndex].data["year"], posRect.w - 50, drawOffset - (tipSize >> 1) + 15, g.noSelectedText, g.lowFont, true, true);
                    RenderText(renderer, tips[drawElemIndex].genres, rect.w + 30, drawOffset + (tipSize >> 1) - 15, g.noSelectedText, g.lowFont, false, true, posRect.w - 110);

                    if (tips[drawElemIndex].data.contains("imdbRating")) {
                        RenderText(renderer, std::format("IMDb: {:.2f}", tips[drawElemIndex].data["imdbRating"].get<double>()), posRect.w - 75, drawOffset + (tipSize >> 1) - 35, g.noSelectedText, g.lowFont, true, true);
                    }if (tips[drawElemIndex].data.contains("kinopoiskRating")) {
                        RenderText(renderer, std::format("КиноПоиск: {:.2f}", tips[drawElemIndex].data["kinopoiskRating"].get<double>()), posRect.w - 75, drawOffset + (tipSize >> 1) - 15, g.noSelectedText, g.lowFont, true, true);
                    }

                }
                ++drawElemNum;
            }
        }

        updateTxtr = false;
        SDL_SetRenderTarget(renderer, nullptr);
    }

    SDL_RenderCopy(renderer, txtr, nullptr, &posRect);
}

void List::update() {
    if ((-0.4 <= scrollAnim) xor (scrollAnim <= 0.4)) { scrollAnim += (0 - scrollAnim) * 0.1; updateTxtr = true; }

    if (client.isConnected() and client.available()) {
        std::string msg = client.receive();
        if (msg.back() == '\n') msg.pop_back();
        defJson received = defJson::parse(msg);
        if (received.contains("searchTips") and !received.empty()) {
            if (MenuID == 25) {
                changeItemIndex = -1;
                menu.closeMenu();
            }
            std::vector<SearchTipsDrawItem> newTips;
            for (auto it = received["searchTips"].begin(); it != received["searchTips"].end(); ++it) {
                SearchTipsDrawItem newTip;
                newTip.data = it.value();
                newTip.name = it.key();

                std::vector<std::string> v = it.value()["genres"].get<std::vector<std::string>>();
                std::string result;
                for (size_t i = 0; i < v.size(); ++i) {
                    result += v[i];
                    if (i < v.size() - 1) result += ", "; // Добавляем запятую, если не последний элемент
                }
                newTip.genres = result;

                bool alreadyLoaded = false;
                for (auto& tip : tips) {
                    if (tip.data["id"] == it.value()["id"]) {
                        newTip = tip;
                        alreadyLoaded = true;
                        tip.miniPosterLoaded = false;
                        break;
                    }
                }
                if (!alreadyLoaded) tloader.LoadTexture(it.value()["miniPoster"], newTips.size());

                newTips.push_back(newTip);
            }
            for (const auto& tip : tips) {
                if (tip.miniPosterLoaded) SDL_DestroyTexture(tip.miniPoster);
            }
            tips = newTips;
            startDrawIndex = 0;
            updateTxtr = true;
        }
    }

    tloader.ProcessLoadedData();

    auto results = tloader.GetAllReadyTextures();
    for (auto& result : results) {
        if (result.success) {
            if (result.id < tips.size()) {
                tips[result.id].miniPoster = result.texture;
                tips[result.id].miniPosterLoaded = true;
                tips[result.id].miniPosterIsHozisontal = result.isHorizontal;
                updateTxtr = true;
            }
        }
    }


    if (MenuID != menu.getMenuId()) {
        MenuID = menu.getMenuId();
        int result_;

        if ((result_ = menu.menuResultById(22)) != -1) {
            if (result_ == 0) {
                std::string editName = list[changeItemIndex].name;
                search->setItem4Edit(editName, data[editName]);
                updateTxtr = true;
            }
            else if (result_ == 1) {
                data.erase(list[changeItemIndex].name);
                list.erase(list.begin() + changeItemIndex);
                updateTxtr = true;
                needSave = true;
            } else if (result_ == 2) {
                std::string url = g.state["categories"][categ->getCategory()]["urlForSearch"];
                url.append(list[changeItemIndex].name);
                std::ranges::replace(url, ' ', '+');
                if (SDL_OpenURL(url.c_str()) != 0) {
                    printf("Failed to open URL: %s\n", SDL_GetError());
                }
            }
        } else if ((result_ = menu.menuResultById(25)) != -1) {
            if (result_ == 0) {
                std::string url = g.state["categories"][categ->getCategory()]["urlForSearch"];
                url.append(tips[changeItemIndex].name);
                std::ranges::replace(url, ' ', '+');
                if (SDL_OpenURL(url.c_str()) != 0) {
                    printf("Failed to open URL: %s\n", SDL_GetError());
                }
            }
        }
    }

    if (needSave) {
        needSave = false;

        std::ofstream file(categoryPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: ../data/tasks.json");
        }
        file << data.dump(4);
        file.close();
    }
}

void List::setPos(SDL_Rect newPos) {
    posRect = newPos;
    if (txtr != nullptr) SDL_DestroyTexture(txtr);
    txtr = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, posRect.w, posRect.h);
    SDL_SetTextureBlendMode(txtr, SDL_BLENDMODE_BLEND);
    updateTxtr = true;
}

void List::handle() {
    ctrl = (SDL_GetModState() & KMOD_CTRL) != 0;
    SDL_GetMouseState(&mouseX, &mouseY);

    mouseX-=posRect.x;
    mouseY-=posRect.y;
    if (0 <= mouseY && mouseY <= posRect.h && 0 <= mouseX && mouseX <= posRect.w and MenuID == -1) {
        updateTxtr = true;
        if (e.type == SDL_MOUSEWHEEL) {
            if (search->mode) {
                if (e.wheel.y > 0) {
                    if (startDrawIndex - e.wheel.y > -1) {
                        scrollAnim += e.wheel.y * fullItemSize;
                        startDrawIndex -= e.wheel.y;
                    }
                }
                else if (e.wheel.y < 0) {
                    if (startDrawIndex - e.wheel.y + posRect.h / fullItemSize < list.size() +  1) {
                        scrollAnim += e.wheel.y * fullItemSize;
                        startDrawIndex -= e.wheel.y;
                    }
                }
            } else {
                if (e.wheel.y > 0) {
                    if (startDrawIndex - e.wheel.y > -1) {
                        scrollAnim += e.wheel.y * fullTipSize;
                        startDrawIndex -= e.wheel.y;
                    }
                }
                else if (e.wheel.y < 0) {
                    if (startDrawIndex - e.wheel.y + posRect.h / fullTipSize < tips.size() +  1) {
                        scrollAnim += e.wheel.y * fullTipSize;
                        startDrawIndex -= e.wheel.y;
                    }
                }
            }
        } if (e.type == SDL_MOUSEBUTTONUP) {
            if (currentIndex != -1) {
                if (search->mode) {
                    changeItemIndex = currentIndex;
                    menu.setMenu(22, mouseX + posRect.x, mouseY + posRect.y);
                } else {
                    if (e.button.button == SDL_BUTTON_LEFT) search->setTip(tips[currentIndex].name, tips[currentIndex].data);
                    else if (e.button.button == SDL_BUTTON_RIGHT) {
                        menu.setMenu(25, mouseX + posRect.x, mouseY + posRect.y);
                        changeItemIndex = currentIndex;
                    }
                }
            }

        }
    } else if (currentIndex != -1) updateTxtr = true;
}

void List::loadCateg(const std::string path) {
    std::ifstream file(path);
    data = json::parse(file);
    file.close();
    updateList("", {});
    categoryPath = path;
}

void List::updateList(const std::string &name, const json &currentItem) {
    startDrawIndex = 0;
    int filtStatus = 0;
    int filtOpinion = 0;
    int drawFilter;
    if (currentItem.contains("opinion")) filtOpinion = currentItem["opinion"];
    if (currentItem.contains("status")) filtStatus = currentItem["status"];
    drawFilter = 0;
    list.clear();
    if (drawFilter < 2) {
        for (auto it = data.begin(); it != data.end(); ++it) {
            listDrawItem newItem;
            newItem = { it.value()["opinion"], it.value()["status"], it.key() };
            bool add_flag = false;
            if (filtStatus == 0 and filtOpinion == 0) add_flag = true;
            else if (filtOpinion == 0 and newItem.status == filtStatus) add_flag = true;
            else if (filtStatus == 0 and newItem.opinion == filtOpinion) add_flag = true;
            else if (newItem.opinion == filtOpinion and newItem.status == filtStatus) add_flag = true;
            if (!name.empty() and add_flag) add_flag = (toLower(newItem.name).find(toLower(name)) != std::string::npos);

            if (add_flag) list.push_back(newItem);
        }
    } else {
        std::vector<std::pair<int, listDrawItem>> items;
        for (auto it = data.begin(); it != data.end(); ++it) {
            listDrawItem newItem;
            int addTime = it.value()["addTime"];
            newItem = {it.value()["opinion"], it.value()["status"], it.key()};
            bool add_flag = false;
            if (filtStatus == 0 and filtOpinion == 0) add_flag = true;
            else if (filtOpinion == 0 and newItem.status == filtStatus) add_flag = true;
            else if (filtStatus == 0 and newItem.opinion == filtOpinion) add_flag = true;
            else if (newItem.opinion == filtOpinion and newItem.status == filtStatus) add_flag = true;
            if (!name.empty() and add_flag) add_flag = (toLower(newItem.name).find(toLower(name)) != std::string::npos);

            if (add_flag) items.emplace_back(addTime, newItem);
        }
        std::sort(items.begin(), items.end(),
                  [](const auto &a, const auto &b) {
                      return a.first < b.first;
                  });
        for (const auto &item: items) {
            list.push_back(item.second);
        }
    }

    if (drawFilter % 2) std::reverse(list.begin(), list.end());
    updateTxtr = true;
}

void List::drawReset() {
    tips.clear();
    updateTxtr = true;
    startDrawIndex = 0;
}

std::string List::toLower(const std::string &str) {
    std::string result;
    result.reserve(str.size());

    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = str[i];

        // Проверяем UTF-8 последовательности
        if (c < 0x80) {
            // ASCII символ
            result += std::tolower(c);
            i++;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < str.size()) {
            // 2-байтовая последовательность (кириллица)
            unsigned char c1 = str[i];
            unsigned char c2 = str[i + 1];

            // Кириллица: D0 90-D0 AF -> D0 90-D0 AF (А-Я)
            // Кириллица: D0 B0-D1 8F -> D0 B0-D1 8F (а-я)
            // Кириллица заглавные: D0 90-D0 AF -> D0 B0-D0 BF
            // D0 90-D0 9F -> D0 B0-D0 BF
            if (c1 == 0xD0 && c2 >= 0x90 && c2 <= 0x9F) {
                // А-П -> а-п
                result += c1;
                result += c2 + 32;
            } else if (c1 == 0xD0 && c2 >= 0xA0 && c2 <= 0xAF) {
                // Р-Я -> р-я
                result += 0xD1;
                result += c2 - 32;
            } else if (c1 == 0xD1 && c2 >= 0x80 && c2 <= 0x8F) {
                // р-я буквы, оставляем как есть
                result += c1;
                result += c2;
            } else {
                // Другие 2-байтовые символы
                result += c1;
                result += c2;
            }
            i += 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < str.size()) {
            // 3-байтовая последовательность
            result += str[i];
            result += str[i + 1];
            result += str[i + 2];
            i += 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < str.size()) {
            // 4-байтовая последовательность (эмодзи)
            result += str[i];
            result += str[i + 1];
            result += str[i + 2];
            result += str[i + 3];
            i += 4;
        } else {
            result += c;
            i++;
        }
    }

    return result;
}
