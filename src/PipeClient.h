//
// Created by mya4t on 03.05.2026.
//

#ifndef NEOMYANIST_PIPECLIENT_H
#define NEOMYANIST_PIPECLIENT_H

#include <windows.h>
#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <iostream>
#include <atomic>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class PipeClient {
public:
    PipeClient(HANDLE pipe);

    // отправка строки
    void send(const std::string& message);

    // есть ли сообщения
    bool available();

    // получить сообщение
    std::string receive();

    bool isConnected() const;

    void disconnect();

    ~PipeClient();

private:
    HANDLE hPipe;
    std::thread readerThread;
    std::queue<std::string> messages;
    std::mutex queueMutex;
    std::mutex writeMutex;
    std::atomic<bool> connected{true};

    void readerLoop();
};

#endif //NEOMYANIST_PIPECLIENT_H
