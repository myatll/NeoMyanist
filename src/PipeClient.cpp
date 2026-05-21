//
// Created by mya4t on 03.05.2026.
//

#include "PipeClient.h"

PipeClient::PipeClient(HANDLE pipe): hPipe(pipe), connected(true) {
    readerThread = std::thread(&PipeClient::readerLoop, this);
}

void PipeClient::send(const std::string &message) {
    std::lock_guard<std::mutex> lock(writeMutex);
    if (!connected) return;

    DWORD written;
    std::string msg = message + "\n";
    WriteFile(hPipe, msg.c_str(), msg.size(), &written, NULL);
}

bool PipeClient::available() {
    std::lock_guard<std::mutex> lock(queueMutex);
    return !messages.empty();
}

std::string PipeClient::receive() {
    std::lock_guard<std::mutex> lock(queueMutex);

    if (messages.empty())
        return "";

    std::string msg = std::move(messages.front());
    messages.pop();
    return msg;
}

bool PipeClient::isConnected() const {
    return connected.load();
}

void PipeClient::disconnect() {
    if (!connected.exchange(false))
        return;

    if (readerThread.joinable())
        readerThread.join();

    CloseHandle(hPipe);
    hPipe = INVALID_HANDLE_VALUE;
}

PipeClient::~PipeClient() {
    disconnect();
}

void PipeClient::readerLoop() {
    char buffer[512];
    DWORD bytesRead;

    std::string localBuffer;

    while (connected.load()) {

        DWORD available = 0;

        BOOL peek = PeekNamedPipe(
            hPipe, NULL, 0, NULL, &available, NULL
        );

        if (!peek || available == 0) {
            Sleep(5);
            continue;
        }

        BOOL ok = ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL);

        if (!ok || bytesRead == 0) {
            connected.store(false);
            break;
        }

        localBuffer.append(buffer, bytesRead);

        size_t pos;
        while ((pos = localBuffer.find('\n')) != std::string::npos) {
            std::string msg = localBuffer.substr(0, pos);
            localBuffer.erase(0, pos + 1);

            std::lock_guard<std::mutex> lock(queueMutex);
            messages.push(msg);
        }
    }

    connected.store(false);
}
