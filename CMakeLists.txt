cmake_minimum_required(VERSION 3.20)
project(fourier_transform)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include_directories(./include /usr/include)
link_directories(/usr/lib)

link_libraries(
    -lsfml-audio
    -lsfml-graphics
    -lsfml-network
    -lsfml-system
    -lsfml-window
    pthread
)

add_executable(
    image-analysis

    src/main.cpp
    src/image.cpp
    src/thresholder.cpp
    src/indexer.cpp
    src/util.cpp

    include/image.hpp
    include/pixel.hpp
    include/thresholder.hpp
    include/indexer.hpp
    include/util.hpp
)

