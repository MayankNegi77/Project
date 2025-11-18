#include "FileAccessManager.h"
#include <iostream>

namespace za {

FileAccessManager* FileAccessManager::instance = nullptr;
pthread_mutex_t FileAccessManager::instanceMutex = PTHREAD_MUTEX_INITIALIZER;

FileAccessManager::FileAccessManager() {
    pthread_mutex_init(&fileAccessMutex, NULL);
}

FileAccessManager::~FileAccessManager() {
    pthread_mutex_destroy(&fileAccessMutex);
    
    for (auto& pair : fileMutexes) {
        pthread_mutex_destroy(pair.second);
        delete pair.second;
    }
}

FileAccessManager* FileAccessManager::getInstance() {
    pthread_mutex_lock(&instanceMutex);
    if (instance == nullptr) {
        instance = new FileAccessManager();
    }
    pthread_mutex_unlock(&instanceMutex);
    return instance;
}

pthread_mutex_t* FileAccessManager::getFileMutex(const std::string& filePath) {
    if (fileMutexes.find(filePath) == fileMutexes.end()) {
        fileMutexes[filePath] = new pthread_mutex_t;
        pthread_mutex_init(fileMutexes[filePath], NULL);
    }
    return fileMutexes[filePath];
}

void FileAccessManager::acquireReadLock(const std::string& filePath) {
    pthread_mutex_lock(&fileAccessMutex);
    
    pthread_mutex_t* fileMutex = getFileMutex(filePath);
    
    activeReaders[filePath]++;
    
    std::cout << "[MUTEX] Thread acquiring read lock for: " << filePath 
              << " (Active readers: " << activeReaders[filePath] << ")\n";
    
    pthread_mutex_unlock(&fileAccessMutex);
    
    pthread_mutex_lock(fileMutex);
}

void FileAccessManager::releaseReadLock(const std::string& filePath) {
    pthread_mutex_t* fileMutex = getFileMutex(filePath);
    pthread_mutex_unlock(fileMutex);
    
    pthread_mutex_lock(&fileAccessMutex);
    
    if (activeReaders[filePath] > 0) {
        activeReaders[filePath]--;
    }
    
    std::cout << "[MUTEX] Thread releasing read lock for: " << filePath 
              << " (Active readers: " << activeReaders[filePath] << ")\n";
    
    if (activeReaders[filePath] == 0) {
        activeReaders.erase(filePath);
    }
    
    pthread_mutex_unlock(&fileAccessMutex);
}

int FileAccessManager::getActiveReaders(const std::string& filePath) {
    pthread_mutex_lock(&fileAccessMutex);
    int readers = activeReaders[filePath];
    pthread_mutex_unlock(&fileAccessMutex);
    return readers;
}

void FileAccessManager::printFileAccess() {
    pthread_mutex_lock(&fileAccessMutex);
    
    std::cout << "\n========================================\n";
    std::cout << "         FILE ACCESS STATUS\n";
    std::cout << "========================================\n";
    
    if (activeReaders.empty()) {
        std::cout << "No files currently being accessed.\n";
    } else {
        for (const auto& file : activeReaders) {
            std::cout << "File: " << file.first << " (Readers: " << file.second << ")\n";
        }
    }
    std::cout << "========================================\n";
    
    pthread_mutex_unlock(&fileAccessMutex);
}

}