use crate::quadtree::GeoBoundary;

const NODE_POINTS_LIMIT: usize = 100;
const NODE_RESERVE: usize = 1_000_000;
const NODE_MIN_AREA: f64 = 10.0;

#[derive(Default, Debug)]
pub struct QuadTree {
    nodes: Vec<QuadTreeNode>,
}

impl QuadTree {
    pub fn from_csv(csv_file: &str) -> Self {
        let mut tree = Self {
            nodes: Vec::with_capacity(1_000_000),
        };

        // TODO: build the tree

        return tree;
    }
}


#[derive(Debug)]
pub struct Point {
    id: i64,
    lat: f64,
    lon: f64,
}

#[derive(Debug)]
pub struct QuadTreeNode {
    boundary: GeoBoundary,
    points: Vec<Point>,
    nw: i64,
    ne: i64,
    sw: i64,
    se: i64,
}

impl QuadTreeNode {
    pub fn from_boundary(boundary: GeoBoundary) -> Self {
        Self { boundary: boundary, points: Vec::with_capacity(NODE_POINTS_LIMIT), nw: -1, ne: -1, sw: -1, se: -1 }
    }
}