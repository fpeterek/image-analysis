cmake_minimum_required(VERSION 3.20)
project(fourier_transform)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_OSX_ARCHITECTURES arm64)

include_directories(
    ./include
    /usr/include
    /opt/homebrew/include
)

link_directories(
    /usr/lib
    /opt/homebrew/lib
)

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
    src/pixel.cpp
    src/signals.cpp
    src/kmeans.cpp
    src/filters.cpp
    src/image_analyzer.cpp
    src/recognition.cpp
    src/neural_network.cpp

    include/image.hpp
    include/pixel.hpp
    include/thresholder.hpp
    include/indexer.hpp
    include/util.hpp
    include/signals.hpp
    include/kmeans.hpp
    include/filters.hpp
    include/image_analyzer.hpp
    include/recognition.hpp
    include/neural_network.hpp
)

