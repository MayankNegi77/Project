file(GLOB CLIENT_SOURCES MyClient.cpp)

add_library(myClient SHARED ${CLIENT_SOURCES})
target_link_libraries(myClient ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(demoClient demoClient.cpp)
target_link_libraries(demoClient myClient ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(demoFileTransferClient demoFileTransferClient.cpp)
target_link_libraries(demoFileTransferClient myClient ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})