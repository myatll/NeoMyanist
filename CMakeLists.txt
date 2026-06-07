cmake_minimum_required(VERSION 4.1)
project(NeoMyanist LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск SDL2, SDL2_ttf и SDL2_image через конфиг (предпочтительный способ)
set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/../.lib")
set(MYATLL_LIBS_DIR "D:/Code/Cpp/NeoMLIBs")
set(CURL_PATH "${PROJECT_SOURCE_DIR}/../.lib/curl-8.20.0_2-win64-mingw")

set(CURL_INCLUDE_DIRS "${CURL_PATH}/include")
set(CURL_LIBRARIES "${CURL_PATH}/lib/libcurl.dll.a")
set(CURL_DLL "${CURL_PATH}/bin/libcurl-x64.dll")

# Создание исполняемого файла
file(GLOB SOURCES CONFIGURE_DEPENDS "../${PROJECT_NAME}/src/*")  # Автообновление списка файлов
add_executable(NeoMyanist WIN32 main.cpp
        ${SOURCES}
        resources.rc
)

find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_ttf REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)

add_library(MyatllNotepad STATIC
        "${MYATLL_LIBS_DIR}/MyatllNotepad.cpp"
        "${MYATLL_LIBS_DIR}/MyatllNotepad.h"
)

target_link_libraries(MyatllNotepad PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
        SDL2_image::SDL2_image
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CURL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
        SDL2_image::SDL2_image
        MyatllNotepad
        ${CURL_LIBRARIES}
)

target_include_directories(MyatllNotepad PUBLIC
        ${MYATLL_LIBS_DIR}
)

#add_compile_options(-fsanitize=address)