cmake_minimum_required(VERSION 3.10.0)
project(Client VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Quick Network)

set(CLIENT_SOURCES
    src/main.cpp
    src/BackEnd.cpp
)

set(CLIENT_HEADERS
    headers/BackEnd.h
)

set(QML_FILES
    qml/main.qml
    qml/MenuStart.qml
    qml/LoginPage.qml
    qml/SignUpPage.qml
    qml/MenuMain.qml
    qml/GameScreen.qml
    qml/BackGround.qml
    qml/ButtonAnimation.qml
    qml/CButton.qml
    qml/SelectButton.qml
    qml/SelectButtonPage.qml
    qml/CountDown.qml
    qml/PausePopup.qml
    qml/PrizePopup.qml
    qml/ResultPopup.qml
    qml/VotePopup.qml
    qml/CallPhonePopup.qml
    qml/FoundOpponentPopup.qml
    qml/ChangePasswordPage.qml
    qml/Logo.qml
)

qt5_add_resources(QML_RESOURCES qml.qrc
    PREFIX "/"
    FILES ${QML_FILES}
)

add_executable(client ${CLIENT_SOURCES} ${CLIENT_HEADERS} ${QML_RESOURCES})

target_link_libraries(client PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::Network
)

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)


