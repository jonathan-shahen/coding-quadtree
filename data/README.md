# Data Generation

This folder provides a method for producing geo-tagged data CSV

## Schema

| Column     | Type    |
| ---------- | ------- |
| place_id   | int64   |
| lat        | float64 |
| long       | float64 |

## Generation

Generating this data we care about specific features in the dataset

- large number of nodes
- nodes are concentrated within bubbles
    - i.e. massive cities (New York) have many more businesses than other places
    - The middle of the ocean should have few/no nodes
- When testing the implementations, we need to have an easy `I` and `R`'s to speed up implementations
    - Should include a unit test bubble, where the businesses returned can be compared to a fixed list
    - [ ] since `place_id` is int64, we can reserve the first 100 to place in a special bubble, and then all others fit around other bubbles.

## 