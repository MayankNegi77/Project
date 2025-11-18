#ifndef SERVER_STATS_H
#define SERVER_STATS_H

#include <pthread.h>
#include <string>
#include <vector>
#include <map>

namespace za {

class ServerStats {
private:
    static ServerStats* instance;
    static pthread_mutex_t instanceMutex;
    
    pthread_mutex_t statsMutex;
    pthread_mutex_t logMutex;
    pthread_mutex_t clientListMutex;
    
    int totalConnections;
    int activeConnections;
    long totalBytesTransferred;
    int totalFilesTransferred;
    std::vector<std::string> transferLog;
    std::map<std::string, int> clientConnections;
    
    ServerStats();
    
public:
    static ServerStats* getInstance();
    ~ServerStats();
    
    void incrementConnections();
    void decrementConnections();
    void addBytesTransferred(long bytes);
    void incrementFilesTransferred();
    void addLogEntry(const std::string& entry);
    void addClientConnection(const std::string& clientIP);
    void removeClientConnection(const std::string& clientIP);
    
    int getTotalConnections();
    int getActiveConnections();
    long getTotalBytesTransferred();
    int getTotalFilesTransferred();
    std::vector<std::string> getRecentLogs(int count = 10);
    void printStats();
    void printClientList();
};

}

#endif