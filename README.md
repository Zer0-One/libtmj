# libtmj

A library for loading [Tiled](https://www.mapeditor.org) maps in JSON format

Bear in mind that this library is relatively new. If you find bugs, or find
that the library is missing an important feature, please file an issue.

## Dependencies

- [`jansson`](https://github.com/akheron/jansson/)
- [`zlib`](http://zlib.net/) (Optional, for zlib and gzip decompression support)
- [`zstd`](https://github.com/facebook/zstd) (Optional, for zstd decompression support)
- [`Doxygen`](https://www.doxygen.nl/) (Optional, for building documentation)
    - [`Graphviz`](https://graphviz.org/) (Optional, for drawing graphs in the generated docs)


To build in Windows, use vcpkg to fetch and install the above dependencies:
```
vcpkg install jansson:x64-windows zstd:x64-windows zlib:x64-windows
```

## Building

From the repository root, run:
```
cmake -DCMAKE_BUILD_TYPE=Release -DLIBTMJ_ZSTD=ON -DLIBTMJ_ZLIB=ON . && make
```

For Windows users, make sure to include the vcpkg toolchain file:
```
cmake -DCMAKE_BUILD_TYPE=Release -DLIBTMJ_ZSTD=ON -DLIBTMJ_ZLIB=ON -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake .
cmake --build . --config Release
```

Available cmake build options:

Option              | Description
------------------- | -----------
BUILD\_SHARED\_LIBS | Builds a shared library instead of a static library.
CMAKE\_BUILD\_TYPE  | One of "Release" (optimization) or "Debug" (runtime sanitizers + debug symbols)
LIBTMJ\_DOCS        | Also build documentation.
LIBTMJ\_ZSTD        | Build zstd decompression routines.
LIBTMJ\_ZLIB        | Build zlib and gzip decompression routines.
LIBTMJ\_TEST        | Build the test suite.

## Testing

If you're going to build and run tests, don't forget to pull the Unity submodule:
```
git submodule update --init
```

To enable the test suite, invoke cmake with:
```
-DCMAKE_BUILD_TYPE=Debug -DLIBTMJ_TEST=True
```
Then run the tests with:
```
ctest // For *nix
ctest -C Debug // For Windows
```

## Usage example

Below is a brief example of how to use libtmj. For more detail, see the [API
documentation](https://zer0-one.github.io/libtmj/). Specifically, refer to the
[Data Structures](https://zer0-one.github.io/libtmj/annotated.html) page for a
list of fields provided by each structure. They match the names given in the
Tiled documentation, with the exception of added fields which give the number
of elements in array fields, whose names are `{field}_count` (as shown below
for the "layer" field).

```
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <tmj.h>

// Logging callback
void log_cb(tmj_log_priority priority, const char* msg){
    switch(priority){
        case TMJ_LOG_DEBUG:
            printf("DEBUG: %s\n", msg);
            break;
        case TMJ_LOG_INFO:
            printf("INFO: %s\n", msg);
            break;
        case TMJ_LOG_WARNING:
            printf("WARNING: %s\n", msg);
            break;
        case TMJ_LOG_ERR:
            printf("ERR: %s\n", msg);
            break;
        case TMJ_LOG_CRIT:
            printf("CRIT: %s\n", msg);
            break;
    }
}

// Layers are organized into a tree, so descend recursively
void enumerate_layers(Layer* layers, size_t count){
    for(size_t i = 0; i < count; i++){
        printf("Layer %d: %s\n", layers[i].id, layers[i].name);

        if(layers[i].layers){
            enumerate_layers(layers[i].layers, layers[i].layer_count);
        }
    }
}

int main(){
    // Register logging callback
    tmj_log_regcb(true, log_cb);

    Map* map = tmj_map_loadf("overworld.tmj", false);

    if(!map){
        _exit(-1);
    }

    // Print the map width and height
    printf("This map is %d tiles high and %d tiles wide\n", map->height, map->width);

    // Print the names of all layers in the map
    enumerate_layers(map->layers, map->layer_count);

    // Free the map
    tmj_map_free(map);
}
```

## Notes

JSON only defines a single number type, meant to accomodate both floating point
values and integers of arbitrary length. This library truncates all input
integers to the size of `int` on your platform, and all input floating point
numbers to the size of `double` on your platform. My estimation is that this is
acceptable for a Tiled map. If you find a case in which this breaks your map,
please let me know.

If you use libtmj in a game or tool, I would love to hear about it. Please reach
out via email or create a pull request to give your game/project a mention in
the README.

## License

    BSD 2-Clause License

    Copyright (c) 2023-2025, David Zero
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
