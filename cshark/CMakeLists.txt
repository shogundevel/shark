
cmake_minimum_required(VERSION 3.0)

project(SharkGame)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${PROJECT_NAME} gameshark.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    SDL2_image
    SDL2_ttf
    SDL2_gfx
    freetype
    m
    bz2
    png16
    jpeg
    z
)

create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
)

