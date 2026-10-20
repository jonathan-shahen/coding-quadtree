# Geo QuadTree Implementations

The purpose of this repo is to experiment with QuadTrees for storing highly compressed geo-data while measuring the following:

1. Compiled size in memory
2. Time to compile


## Problem Statement

We have a map/location based website, and we have a list of millions of businesses (`N` used to repesent this number) registered with us.

Given a lat/long point `I` and a radius `R`, use a Geo Quad Tree to answer the following:

1. Return a list of all businesses that fit within that radius `R`
    - it is okay to use `circle` or `square` radius
2. Return a list of all businesses that fit within that radius `2*R` (exclude the businesses from 1)

### Assumptions

- A READ heavy service. New businesses will be added rarely and do not need to be instantly available
- 

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
    - Using a tree structure can drastically reduce the number of checks needed to come to the same answer
        - This speeds up READ times, which is assumed a priority

## Implementations

1. [Rust](rust/README.md)
1. [Go](go/README.md)

## Results



## AI Statement

This project was developed by Dr. Jonathan Shahen with no help from A.I.