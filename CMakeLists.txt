cmake_minimum_required(VERSION 3.5)

project(QtChatApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network Sql REQUIRED)

add_executable(QtChatApp
    main.cpp
    ChatWindow.cpp
    ChatWindow.h
    DatabaseLogger.cpp
    DatabaseLogger.h
)

target_link_libraries(QtChatApp PRIVATE Qt5::Widgets Qt5::Network Qt5::Sql)
