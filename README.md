# Geo QuadTree Implementations

The purpose of this repo is to experiment with QuadTrees for storing highly compressed geo-data while measuring the following:

1. Compiled size in memory
2. Time to compile


## Problem Statement

We have a map/location based website, and we have a list of millions of businesses (`N` used to represent this number) registered with us.

Given a lat/long point `I=(lat,long)` and a radius `R`, use a Geo Quad Tree to answer the following:

1. Return a list of $X$ businesses that fit within that radius `R`
    - it is okay to use `circle` or `square` radius.
2. Return a list of $X$ businesses that fit within that radius `2*R` (exclude the businesses within `R`)
    - If there were more than $X$ businesses in step (1), then start copying those into this list.

### Assumptions

- A READ heavy service. New businesses will be added rarely and do not need to be instantly available
- The list of businesses will be shown in a map or list view. If there are not enough, then the map will be scaled out and the `extra` (see 2) businesses will be shown OR the list will be extended with the extra businesses.
- Out of scope:
    - Debouncing will be handled client side
    - Caching and rate limiting
    - Auto up and down scaling
    - Geographic splitting (based on `I` sending to a different server to horizontally scale and reduce memory usage)

### What is a QuadTree

A tree data-structure. It assumes a square initial search space and then allows for 4 subdivisions per child.
These splits can be recursively done.
The length from root to leaf is not guanunteed (does not necessarily create a complete tree).
Data is stored in `leaf` nodes. Non-leaf nodes must have 4 children (sub-dividing the square into 4 equal squares).

### How can we use this for Storing Geo-locations

- We can restrict the maximum number of businesses allowed in a leaf node

### Interesting Considerations

- Why not just store all businesses in a LIST?
    - This takes $O(n)$ time to find all businesses that match
    - Using a tree structure can drastically reduce the number of checks (average logarithmic reduction) needed to come to the same answer
        - This speeds up READ times, which is assumed a priority
- The earth is round and lat/long is not linear
    - When talking about very small distances, then linear bounding areas works very well
    - lat/long being not a linear coord is a serious issue, even though a significant amount of people live near the equator, the skew is massive enough to require special distance measurements
        - see: https://en.wikipedia.org/wiki/Haversine_formula to determine distance between 2 points on a sphere
        - see: http://janmatuschek.de/LatitudeLongitudeBoundingCoordinates to create a min/max lat/long box for `I`+`R`
        - see: https://en.wikipedia.org/wiki/Spherical_coordinate_system general understanding
- Mall Problem
    - If we pick a small limit on number of businesses inside a node (i.e. 100) and we come across a Mall
    - Businesses inside of a Mall will often just put the address of the mall, and thus the GPS location will be the same for all businesses INSIDE of a Mall
    - If we have 101+ businesses with the same GPS location, then we will have an INFINITE DEPTH QuadTree
    - Protection needed for this
    - EXTRA: in countries/states with little/no sales tax, single addresses can be the mailing address of 1000+ businesses
    - SOLUTION: restrict the minimum area a QuadTree node can cover. Ex: $10 m^2$

## Implementations

1. [Rust](src/rust/README.md)
1. [Go](src/go/README.md)
1. [C++](src/cpp/README.md)

## Results



## AI Statement

This project was developed by Dr. Jonathan Shahen with no help from A.I.