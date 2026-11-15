cmake_minimum_required(VERSION 3.21)
project(NexusMusicPlayer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets Sql Network)

qt_add_executable(NexusMusicPlayer
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/core/DatabaseManager.cpp
    src/core/DatabaseManager.h
    src/core/LibraryManager.cpp
    src/core/LibraryManager.h
    src/core/PlaylistManager.cpp
    src/core/PlaylistManager.h
    src/core/SearchManager.cpp
    src/core/SearchManager.h
    src/core/SettingsManager.cpp
    src/core/SettingsManager.h
    src/player/MusicPlayer.cpp
    src/player/MusicPlayer.h
    src/player/VideoPlayer.cpp
    src/player/VideoPlayer.h
    src/integrations/SpotifyIntegration.cpp
    src/integrations/SpotifyIntegration.h
    src/integrations/YouTubeIntegration.cpp
    src/integrations/YouTubeIntegration.h
)

target_include_directories(NexusMusicPlayer PRIVATE src)
target_link_libraries(NexusMusicPlayer PRIVATE
    Qt6::Widgets Qt6::Multimedia Qt6::MultimediaWidgets Qt6::Sql Qt6::Network
)

if(WIN32)
    set_target_properties(NexusMusicPlayer PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
