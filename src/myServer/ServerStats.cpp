#include "ServerStats.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace za {

ServerStats* ServerStats::instance = nullptr;
pthread_mutex_t ServerStats::instanceMutex = PTHREAD_MUTEX_INITIALIZER;

ServerStats::ServerStats() {
    pthread_mutex_init(&statsMutex, NULL);
    pthread_mutex_init(&logMutex, NULL);
    pthread_mutex_init(&clientListMutex, NULL);
    
    totalConnections = 0;
    activeConnections = 0;
    totalBytesTransferred = 0;
    totalFilesTransferred = 0;
}

ServerStats::~ServerStats() {
    pthread_mutex_destroy(&statsMutex);
    pthread_mutex_destroy(&logMutex);
    pthread_mutex_destroy(&clientListMutex);
}

ServerStats* ServerStats::getInstance() {
    pthread_mutex_lock(&instanceMutex);
    if (instance == nullptr) {
        instance = new ServerStats();
    }
    pthread_mutex_unlock(&instanceMutex);
    return instance;
}

void ServerStats::incrementConnections() {
    pthread_mutex_lock(&statsMutex);
    totalConnections++;
    activeConnections++;
    pthread_mutex_unlock(&statsMutex);
}

void ServerStats::decrementConnections() {
    pthread_mutex_lock(&statsMutex);
    if (activeConnections > 0) {
        activeConnections--;
    }
    pthread_mutex_unlock(&statsMutex);
}

void ServerStats::addBytesTransferred(long bytes) {
    pthread_mutex_lock(&statsMutex);
    totalBytesTransferred += bytes;
    pthread_mutex_unlock(&statsMutex);
}

void ServerStats::incrementFilesTransferred() {
    pthread_mutex_lock(&statsMutex);
    totalFilesTransferred++;
    pthread_mutex_unlock(&statsMutex);
}

void ServerStats::addLogEntry(const std::string& entry) {
    pthread_mutex_lock(&logMutex);
    
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    
    std::ostringstream timestamp;
    timestamp << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    
    std::string logEntry = "[" + timestamp.str() + "] " + entry;
    transferLog.push_back(logEntry);
    
    if (transferLog.size() > 100) {
        transferLog.erase(transferLog.begin());
    }
    
    pthread_mutex_unlock(&logMutex);
}

void ServerStats::addClientConnection(const std::string& clientIP) {
    pthread_mutex_lock(&clientListMutex);
    clientConnections[clientIP]++;
    pthread_mutex_unlock(&clientListMutex);
}

void ServerStats::removeClientConnection(const std::string& clientIP) {
    pthread_mutex_lock(&clientListMutex);
    if (clientConnections.find(clientIP) != clientConnections.end()) {
        if (clientConnections[clientIP] > 1) {
            clientConnections[clientIP]--;
        } else {
            clientConnections.erase(clientIP);
        }
    }
    pthread_mutex_unlock(&clientListMutex);
}

int ServerStats::getTotalConnections() {
    pthread_mutex_lock(&statsMutex);
    int total = totalConnections;
    pthread_mutex_unlock(&statsMutex);
    return total;
}

int ServerStats::getActiveConnections() {
    pthread_mutex_lock(&statsMutex);
    int active = activeConnections;
    pthread_mutex_unlock(&statsMutex);
    return active;
}

long ServerStats::getTotalBytesTransferred() {
    pthread_mutex_lock(&statsMutex);
    long bytes = totalBytesTransferred;
    pthread_mutex_unlock(&statsMutex);
    return bytes;
}

int ServerStats::getTotalFilesTransferred() {
    pthread_mutex_lock(&statsMutex);
    int files = totalFilesTransferred;
    pthread_mutex_unlock(&statsMutex);
    return files;
}

std::vector<std::string> ServerStats::getRecentLogs(int count) {
    pthread_mutex_lock(&logMutex);
    std::vector<std::string> recentLogs;
    
    int start = std::max(0, (int)transferLog.size() - count);
    for (int i = start; i < transferLog.size(); i++) {
        recentLogs.push_back(transferLog[i]);
    }
    
    pthread_mutex_unlock(&logMutex);
    return recentLogs;
}

void ServerStats::printStats() {
    pthread_mutex_lock(&statsMutex);
    std::cout << "\n========================================\n";
    std::cout << "           SERVER STATISTICS\n";
    std::cout << "========================================\n";
    std::cout << "Total Connections:    " << totalConnections << "\n";
    std::cout << "Active Connections:   " << activeConnections << "\n";
    std::cout << "Files Transferred:    " << totalFilesTransferred << "\n";
    std::cout << "Bytes Transferred:    " << totalBytesTransferred << " bytes\n";
    std::cout << "========================================\n";
    pthread_mutex_unlock(&statsMutex);
}

void ServerStats::printClientList() {
    pthread_mutex_lock(&clientListMutex);
    std::cout << "\n========================================\n";
    std::cout << "         CONNECTED CLIENTS\n";
    std::cout << "========================================\n";
    
    if (clientConnections.empty()) {
        std::cout << "No clients currently connected.\n";
    } else {
        for (const auto& client : clientConnections) {
            std::cout << "IP: " << client.first << " (Connections: " << client.second << ")\n";
        }
    }
    std::cout << "========================================\n";
    pthread_mutex_unlock(&clientListMutex);
}

}