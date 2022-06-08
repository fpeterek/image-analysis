# Image Analysis

## Dependencies

The program depends on the SFML library. You can install this library using your favorite
package manager.

### Arch

```sh
sudo pacman -S sfml
```

### Debian

```sh
sudo apt install libsfml-dev
```

### macOS

```sh
brew install sfml
```

Alternatively, on ARM based Macs

```sh
arch arm64 brew install sfml
```

### Windows

[Follow these instructions](https://wiki.archlinux.org/title/Installation_guide)

## Build

```sh
cmake . -DCMAKE_BUILD_TYPE=Release
make
```

Note: I use a rolling release distribution, thus, I always have the latest version
of CMake. I do not care for the latest CMake features as much, therefore, if your
distribution does not yet have the latest CMake in it's repositories, it may be fine
to relax the minimum CMake version requirement specified in the `CMakeLists.txt` file.

## Run

```sh
./image-analysis
```

## Input

The input is hardcoded and can be located in the [resources](resources/) folder.

## Output

The output is also hardcoded. The program outputs three files. `learning.reconstructed.png`
and `recognition.reconstructed.png` display the results of segmentation using thresholding.
`recognition.objects.png` displays the results of object recognition using a neural network.
Each object is annotated with the number of its class. Class numbers are determined randomly
at runtime.

## Code

### main.cpp

Entrypoint of the program. The `main(int, const char**)` function ignores its arguments.

### filters.hpp, filters.cpp

Contains logic necessary to filter out tiny objects - objects which are usually the artifacts
of noise and cannot be reasonably analyzed.

### image.hpp, image.cpp

Custom representation of input images, which provides the capability to assign object
indices to individual pixels, and thus split the input image into objects. The `Image`
class also provides the functionality to reconstruct the image and color objects
using a parametrized collection of colors.

### pixel.hpp, pixel.cpp

Closely related to the aforementioned `Image` class, the `Pixel` struct defined inside the
`pixel.hpp` file holds information about a single pixel.

### image_analyzer.hpp, image_analyzer.cpp

The `ImageAnalyzer` class located inside the `image_analyzer.hpp` file wraps thresholding,
neural network training and object recognition into a simple to use API. The neural network
is trained by first clustering the objects using the K-means algorithm and then training
the network to assign a proper class to each set of signals.

### indexer.hpp, indexer.cpp

The `Indexer` class assigns indices to objects in the input image.

### kmeans.hpp, kmeans.cpp

A simple implementation of the K-means clustering algorithm.

### neural_network.hpp, neural_network.cpp

A simple neural network trained using the back-propagation algorithm. Although it may seem impossible
judging by the author of the code, the entirety of the logic is implemented in a `.cpp` file
and the class is not templated in any way.

### recognition.hpp, recognition.cpp

Performs object recognition using distance to cluster centroid. Works great but has been deprecated in
favour of the neural network, and thus its object recognition capabilities are not invoked anywhere 
in the program.

### signals.hpp, signals.cpp

Implements the functionality to compute signals on a set of objects. Said signals are then used
during object recognition.

### thresholder.hpp, thresholder.cpp

The templated `Thresholder` class separates foreground from background using the thresholding technique.
The only template argument is a class capable of providing a suitable threshold.

Two such threshold providers are implemented - `ConstantThreshold`, which returns a constant value regardless
of the input image, and a `HalfRangeThreshold`, which takes the maximum and minimum brightness from the input
image and returns a value halfway between both such extremes.

### util.hpp, util.cpp

Contains utility functions, which, as of now, is only a function which converts an RGB value to a 
grayscale color.

## Font

The font used by this program was kindly borrowed [from here](https://github.com/google/fonts/tree/main/ofl/inconsolata).
