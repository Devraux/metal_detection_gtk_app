cmake_minimum_required(VERSION 3.27.4)
project(metal_detection_app C)

set(CMAKE_C_STANDARD 17)

#PKG configuration
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

add_executable(${PROJECT_NAME}

    GUI/gui.c
    wifi/wifi.c
    main.c)

target_link_libraries(${PROJECT_NAME} ${GTK_LIBRARIES} ws2_32)
