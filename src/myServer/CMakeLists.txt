set(SERVER_LIB_SOURCES
    MyServer.cpp
    ProcessFileTransfer.cpp
    ServerStats.cpp
    FileAccessManager.cpp
    MyThread.cpp
    ProcessSingleClient.cpp
    GetLocalAddress.cpp
)

add_library(myServer SHARED ${SERVER_LIB_SOURCES})
target_link_libraries(myServer ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(demoServer demoServer.cpp)
target_link_libraries(demoServer myServer ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(demoFileTransferServer demoFileTransferServer.cpp)
target_link_libraries(demoFileTransferServer myServer ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(demoMutexFileTransferServer demoMutexFileTransferServer.cpp)
target_link_libraries(demoMutexFileTransferServer myServer ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})