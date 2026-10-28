# Cpp Implementation for Geo-Business Radius Lookup

## Building and Running

### Prerequisites

- g++ and make
    - For Windows [MSYS2](https://www.msys2.org/) is a useful installer for mingw tooling
- OPTIONAL(profiling): recursively pull the repo to get `Tracy`

### How to build

1. `make`

### How to Build+Run Sample data

1. `make run`
    - You can find Unit Test data: [unit_test.csv](../../data/unit_test.csv)
    - Input: `-68.576446,77.968737,50,100`
    - Output: `{"ids":[0,1,2...,29], "extra":[30,31,32,...,99]}` (`...` added to reduce text, all ids should be in output)

### Run Custom Data

1. See [README](../../data/README.md) to generate your own data
1. (in `src/cpp` folder) `./build/release/quadtree_search ../../data/data.csv`
    - update `data.csv` if you named yours different



## Developers 
### How to Debug

- `make debug`
- In VSCode
    - Setup all C++ extensions (see Microsoft publisher)
    - Go to Debugging tab
    - Click on `(Windows) Debug`
        - Small tweaks needed for Linux/Mac for paths

### How to Profile

- `make profile`
- Download the Tracy v0.14.1 binaries for your system
- run `tracy-profiler.exe`
- Connect to `localhost`
- `make run-profile`
    - CTRL-C once in the infinite loop
- View the profile results

## TODOS

-[ ] Add unit testing
-[ ] Load build tree from binary (compare to build time)