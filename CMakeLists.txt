cmake_minimum_required(VERSION 3.27.4)
project(metal_detection_app C)

# Ustaw standard C
set(CMAKE_C_STANDARD 11)

# Znajdź pakiety GTK
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

# Dodaj katalogi include
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

# Dodaj pliki źródłowe
add_executable(${PROJECT_NAME} main.c)

# Linkuj bibliotekę GTK
target_link_libraries(${PROJECT_NAME} ${GTK_LIBRARIES})
