cmake_minimum_required(VERSION 3.16)
project(QuizClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Prefer Qt6 on Linux, fallback to Qt5 if not available
find_package(Qt6 COMPONENTS Core Widgets Network QUIET)
if (Qt6_FOUND)
    set(QT_PACKAGE Qt6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)
    set(QT_PACKAGE Qt5)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)

# Source files (Phase 1-4)
set(SOURCES
    src/main.cpp
    src/network/NetworkManager.cpp
    src/network/ProtocolHelper.cpp
    src/ui/LoginWindow.cpp
    src/ui/LobbyWindow.cpp
    src/ui/CreateRoomDialog.cpp
    src/ui/RoomWindow.cpp
    src/ui/GameWindow.cpp
    src/ui/StatsWindow.cpp
    src/models/SessionState.cpp
    src/models/GameState.cpp
)

set(HEADERS
    src/network/NetworkManager.h
    src/network/ProtocolHelper.h
    src/ui/LoginWindow.h
    src/ui/LobbyWindow.h
    src/ui/CreateRoomDialog.h
    src/ui/RoomWindow.h
    src/ui/GameWindow.h
    src/ui/StatsWindow.h
    src/models/SessionState.h
    src/models/GameState.h
    ../common/protocol.h
)

set(UI_FILES
    src/ui/LoginWindow.ui
    src/ui/LobbyWindow.ui
    src/ui/CreateRoomDialog.ui
    src/ui/RoomWindow.ui
    src/ui/GameWindow.ui
    src/ui/StatsWindow.ui
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES})

target_link_libraries(${PROJECT_NAME}
    ${QT_PACKAGE}::Core
    ${QT_PACKAGE}::Widgets
    ${QT_PACKAGE}::Network
)

# On Linux/WSL, no extra system libs needed for Qt networking
