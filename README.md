# libtmj

A library for loading Tiled maps in JSON format

Bear in mind that this library is currently in beta and not yet stable.  If you
find bugs, or find that the library is missing an important feature, please
file an issue.

## Dependencies

- [`jansson`](https://github.com/akheron/jansson/)
- [`zlib`](http://zlib.net/) (Optional, for zlib and gzip decompression support)
- [`zstd`](https://github.com/facebook/zstd) (Optional, for zstd decompression support)
- [`Doxygen`](https://www.doxygen.nl/) (Optional, for building documentation)
    - [`Graphviz`](https://graphviz.org/) (Optional, for drawing graphs in the generated docs)


## Building

From the repository root, run:
```
cmake . && make
```

Available cmake build options:

Option              | Description
------------------- | -----------
BUILD\_SHARED\_LIBS | Builds a shared library instead of a static library.
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
cmake -DLIBTMJ_TEST=True .
```
Then run the tests with:
```
make test
```

## Usage example

Below is a brief example of how to use libtmj. For more detail, see the [API
documentation](https://zer0-one.github.io/libtmj/).

```
#include <stdio.h>
#include <unistd.h>

#include <tmj.h>

// Logging callback
void log_cb(tmj_log_priority priority, const char* msg){
    switch(priority){
        case DEBUG:
            printf("DEBUG: %s\n", msg);
            break;
        case INFO:
            printf("INFO: %s\n", msg);
            break;
        case WARNING:
            printf("WARNING: %s\n", msg);
            break;
        case ERR:
            printf("ERR: %s\n", msg);
            break;
        case CRIT:
            printf("CRIT: %s\n", msg);
            break;
    }
}

// Layers are oganized into a tree, so descend recursively
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

## License

    BSD 2-Clause License

    Copyright (c) 2023, David Zero
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
