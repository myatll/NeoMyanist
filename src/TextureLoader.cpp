#include "TextureLoader.h"

// Создание текстуры из памяти
SDL_Texture* AsyncTextureLoader::CreateTextureFromMemory(SDL_Renderer* renderer, const std::vector<unsigned char>& data, double& aspect) {
    SDL_RWops* rw = SDL_RWFromConstMem(data.data(), data.size());
    if (!rw) return nullptr;

    SDL_Surface* surface = IMG_Load_RW(rw, 0);
    SDL_RWclose(rw);

    if (!surface) return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    aspect = static_cast<double>(surface->w) / surface->h;
    SDL_FreeSurface(surface);

    return texture;
}

// Рабочий поток для загрузки
void AsyncTextureLoader::DownloadThread() {
    CURL* curl = curl_easy_init();
    
    while (running) {
        DownloadTask task;
        bool hasTask = false;
        
        {
            std::unique_lock<std::mutex> lock(downloadMutex);
            
            downloadCV.wait_for(lock, std::chrono::milliseconds(100), [this] {
                return !downloadQueue.empty() || !running;
            });
            
            if (!running) break;
            
            if (!downloadQueue.empty()) {
                task = downloadQueue.back();
                downloadQueue.pop_back();
                
                if (!task.cancelled) {
                    hasTask = true;
                }
            }
        }
        
        if (hasTask) {
            DownloadedImageData result;
            result.id = task.id;
            result.url = task.url;
            result.timestamp = task.timestamp;
            
            if (DownloadImageToMemory(task.url, result.data)) {
                result.success = true;
            } else {
                result.success = false;
                result.error = "Download failed";
            }
            
            {
                std::lock_guard<std::mutex> lock(createMutex);
                createQueue.push(result);
            }
        }
    }
    
    curl_easy_cleanup(curl);
}

// Конструктор
AsyncTextureLoader::AsyncTextureLoader(SDL_Renderer* renderer) 
    : renderer(renderer), running(false) {
}

// Деструктор
AsyncTextureLoader::~AsyncTextureLoader() {
    Stop();
}

// Запуск рабочего потока
void AsyncTextureLoader::Start() {
    if (!running) {
        running = true;
        workerThread = std::thread(&AsyncTextureLoader::DownloadThread, this);
    }
}

// Остановка рабочего потока
void AsyncTextureLoader::Stop() {
    running = false;
    downloadCV.notify_all();
    
    if (workerThread.joinable()) {
        workerThread.join();
    }
    
    ClearQueues();
}

// Добавление задачи на загрузку
void AsyncTextureLoader::LoadTexture(const std::string& url, int id) {
    DownloadTask task;
    task.url = url;
    task.id = id;
    task.timestamp = std::chrono::steady_clock::now();
    
    {
        std::lock_guard<std::mutex> lock(downloadMutex);
        
        // Отменяем старые задачи с таким же ID
        for (auto& t : downloadQueue) {
            if (t.id == id) {
                t.cancelled = true;
            }
        }
        
        downloadQueue.push_back(task);
        activeDownloads[id] = task.timestamp;
        downloadCV.notify_one();
    }
}

// Обработка загруженных данных (создание текстур)
void AsyncTextureLoader::ProcessLoadedData() {
    std::vector<DownloadedImageData> itemsToProcess;
    
    {
        std::lock_guard<std::mutex> lock(createMutex);
        while (!createQueue.empty()) {
            itemsToProcess.push_back(createQueue.front());
            createQueue.pop();
        }
    }
    
    for (auto& item : itemsToProcess) {
        TextureResult result;
        result.id = item.id;
        
        bool isLatest = false;
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            auto it = activeDownloads.find(item.id);
            if (it != activeDownloads.end() && it->second == item.timestamp) {
                isLatest = true;
                activeDownloads.erase(it);
            }
        }
        
        if (isLatest && item.success) {
            result.texture = CreateTextureFromMemory(renderer, item.data, result.aspect);
            if (result.texture) {
                result.success = true;
            } else {
                result.success = false;
                result.error = "Failed to create texture: " + std::string(SDL_GetError());
            }
        } else {
            result.success = false;
            result.error = "Outdated or failed download";
        }
        
        {
            std::lock_guard<std::mutex> lock(resultMutex);
            resultQueue.push(result);
        }
    }
}

// Получение следующей готовой текстуры
TextureResult AsyncTextureLoader::GetNextTexture() {
    std::lock_guard<std::mutex> lock(resultMutex);
    if (!resultQueue.empty()) {
        TextureResult result = resultQueue.front();
        resultQueue.pop();
        return result;
    }
    return TextureResult();
}

// Получение всех готовых текстур
std::vector<TextureResult> AsyncTextureLoader::GetAllReadyTextures() {
    std::vector<TextureResult> results;
    std::lock_guard<std::mutex> lock(resultMutex);
    
    while (!resultQueue.empty()) {
        results.push_back(resultQueue.front());
        resultQueue.pop();
    }
    
    return results;
}

// Очистка всех очередей
void AsyncTextureLoader::ClearQueues() {
    {
        std::lock_guard<std::mutex> lock(downloadMutex);
        downloadQueue.clear();
    }
    
    {
        std::lock_guard<std::mutex> lock(createMutex);
        while (!createQueue.empty()) createQueue.pop();
    }
    
    {
        std::lock_guard<std::mutex> lock(resultMutex);
        while (!resultQueue.empty()) {
            if (resultQueue.front().texture) {
                SDL_DestroyTexture(resultQueue.front().texture);
            }
            resultQueue.pop();
        }
    }
}

size_t AsyncTextureLoader::GetDownloadQueueSize() {
    std::lock_guard<std::mutex> lock(downloadMutex);
    return downloadQueue.size();
}

size_t AsyncTextureLoader::GetResultQueueSize() {
    std::lock_guard<std::mutex> lock(resultMutex);
    return resultQueue.size();
}