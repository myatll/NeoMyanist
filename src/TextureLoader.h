#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <map>
#include <memory>
#include <iostream>
#include <algorithm>
#include <condition_variable>
#include <chrono>

// Колбэк для записи данных в память
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    std::vector<unsigned char>* mem = static_cast<std::vector<unsigned char>*>(userp);
    unsigned char* ptr = static_cast<unsigned char*>(contents);
    mem->insert(mem->end(), ptr, ptr + realsize);
    return realsize;
}

// Загрузка изображения в память
inline bool DownloadImageToMemory(const std::string& url, std::vector<unsigned char>& buffer) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}

// Структура для хранения загруженных данных
struct DownloadedImageData {
    std::vector<unsigned char> data;
    int id;
    std::string url;
    std::chrono::steady_clock::time_point timestamp;
    bool success;
    std::string error;
};

// Структура для готовой текстуры
struct TextureResult {
    SDL_Texture* texture = nullptr;
    bool isHorizontal = false;
    int id = -1;
    bool success = false;
    std::string error;
};

// Основной класс для асинхронной загрузки текстур
class AsyncTextureLoader {
private:
    SDL_Renderer* renderer;
    std::thread workerThread;

    // Мьютексы для потокобезопасности
    std::mutex downloadMutex;
    std::mutex createMutex;
    std::mutex resultMutex;
    std::mutex stateMutex;

    // Структура для задачи загрузки
    struct DownloadTask {
        std::string url;
        int id;
        std::chrono::steady_clock::time_point timestamp;
        bool cancelled = false;
    };

    // Очереди
    std::vector<DownloadTask> downloadQueue;
    std::queue<DownloadedImageData> createQueue;
    std::queue<TextureResult> resultQueue;

    // Состояние
    std::atomic<bool> running;
    std::map<int, std::chrono::steady_clock::time_point> activeDownloads;

    // Условные переменные
    std::condition_variable downloadCV;

    // Рабочий поток для загрузки
    void DownloadThread();

    // Создание текстуры из памяти
    static SDL_Texture* CreateTextureFromMemory(SDL_Renderer* renderer, const std::vector<unsigned char>& data, bool& isHorizontal);

public:
    AsyncTextureLoader(SDL_Renderer* renderer);
    ~AsyncTextureLoader();

    void Start();
    void Stop();
    void LoadTexture(const std::string& url, int id);
    void ProcessLoadedData();
    TextureResult GetNextTexture();
    std::vector<TextureResult> GetAllReadyTextures();
    void ClearQueues();
    size_t GetDownloadQueueSize();
    size_t GetResultQueueSize();
};