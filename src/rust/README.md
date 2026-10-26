# Rust Implementation for Geo-Business Radius Lookup

## How to run

1. `./quadtree [options] <csv data>`
    - When the program starts up, it will build or load a cahced version of the quadtree
    - A prompt will ask you for coords and radius, it will then return a list of ids in `R` and a list of extra ids


### Options

- `--no-cache`
- `--sort`
- `--unit-test [csv-file]`

### Caching

This program builds the QuadTree and then writes a cached version to disk.
Next time you run the program, it will try to load the cached version, if that is not possible, then it will rebuild the tree.

You can turn off the feature by using `--no-cache`

### Manual Verification

A special set of coords + radius are programmed in [unit_test.csv](../data/unit_test.csv).
Use these to ensure the response is correct.

The ids are the first ones generated, thus the return should be

```
businesses: 1,2,3,4,...,30
extra: 31,32,33,34,...,100
```

To get the ids sorted, add the option `--sort` (but do not use this option during performance tests)

### Automatic Verification
This will return a report if it was correct and then exit.

```
./quadtree --unit-test ../../data/unit_test.csv ../../data/data.csv
```