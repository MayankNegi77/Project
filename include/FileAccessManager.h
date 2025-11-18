#ifndef FILE_ACCESS_MANAGER_H
#define FILE_ACCESS_MANAGER_H

#include <pthread.h>
#include <string>
#include <map>

namespace za {

class FileAccessManager {
private:
    static FileAccessManager* instance;
    static pthread_mutex_t instanceMutex;
    
    pthread_mutex_t fileAccessMutex;
    std::map<std::string, int> activeReaders;
    std::map<std::string, pthread_mutex_t*> fileMutexes;
    
    FileAccessManager();
    
public:
    static FileAccessManager* getInstance();
    ~FileAccessManager();
    
    void acquireReadLock(const std::string& filePath);
    void releaseReadLock(const std::string& filePath);
    int getActiveReaders(const std::string& filePath);
    void printFileAccess();
    
private:
    pthread_mutex_t* getFileMutex(const std::string& filePath);
};

}

#endif