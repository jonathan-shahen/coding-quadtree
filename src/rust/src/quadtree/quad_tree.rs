use std::time::Instant;

use crate::quadtree::GeoBoundary;
use crate::quadtree::SearchResults;

const NODE_POINTS_LIMIT: usize = 100;
const NODE_RESERVE: usize = 1_000_000;
const NODE_MIN_AREA: f64 = 10.0;

macro_rules! parse_or_continue {
    ($value:expr, $type:ty, $name:expr) => {
        match $value.parse::<$type>() {
            Ok(v) => v,
            Err(e) => {
                eprintln!("Invalid {}: {}", $name, e);
                continue;
            }
        }
    };
}

#[derive(Default, Debug)]
pub struct QuadTree {
    nodes: Vec<QuadTreeNode>,
    root_index: u64,
    count_points: u64,
    count_subdivides: u64,
    count_overflows: u64,
}

impl QuadTree {
    pub fn from_csv(csv_file: &str) -> Result<Self, Box<dyn std::error::Error>> {
        let mut tree = Self {
            nodes: Vec::with_capacity(1_000_000),
            root_index: 0,
            count_points: 0,
            count_subdivides: 0,
            count_overflows: 0,
        };

        // Try to load a tree that matches the
        if !tree.try_load_tree(csv_file) {
            tree.build_tree(csv_file)?;
        }

        Ok(tree)
    }

    pub fn find(
        &self,
        lat: f32,
        lon: f32,
        radius: f32,
        limit: u32,
    ) -> Result<SearchResults, Box<dyn std::error::Error>> {
        let mut results = SearchResults::default();

        Ok(results)
    }

    pub fn build_tree(&mut self, csv_file: &str) -> Result<(), Box<dyn std::error::Error>> {
        // ZoneScopedN("QuadTree Build Tree");
        let t1 = Instant::now();
        println!("=================================================");
        println!("Building QuadTree from: {}", csv_file);
        // Reserve an initial capacity to reduce number of allocs
        self.nodes.reserve(NODE_RESERVE);

        // Build the default root node
        self.nodes.push(QuadTreeNode::from_boundary(GeoBoundary{lat_min:-90.0, lat_max: 90.0, lon_min: -180.0, lon_max: 180.0}));
        self.root_index = 0;

        let mut row: usize = 0;
        let mut point = Point::default();
        let mut reader = csv::Reader::from_path(csv_file)?;
        for record in reader.records() {
            let record = record?;

            // Skip the header row
            if row == 0
            {
                row += 1;
                continue;
            }

            // TODO: PERFORMANCE
            point.id = parse_or_continue!(record.get(0).ok_or("Missing radius")?, i64, "id");
            point.lat = parse_or_continue!(record.get(1).ok_or("Missing latitude")?, f32, "latitude");
            point.lon = parse_or_continue!(record.get(2).ok_or("Missing longitude")?, f32, "longitude");

            let _ = self.insert(self.root_index as usize, point)?;
            self.count_points += 1;

            row += 1;
        }

        let t2 = Instant::now();
        let time_span = t2.duration_since(t1);
        
        println!("    Build Time: {} seconds.", time_span.as_secs_f64());
        println!("     Overflows: {}", self.count_overflows);
        println!("    Subdivides: {}", self.count_subdivides);
        println!("         Nodes: {}", self.nodes.len());
        println!("        Points: {}", self.count_points);
        println!("=================================================");
        Ok(())
    }

    pub fn analyze(&self) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }

    /// Attempt to find a cached version of this CSV already built
    /// @returns TRUE if found/loaded, FALSE if need to rebuild
    pub fn try_load_tree(&self, csv_file: &str) -> bool {
        // TODO: try to load cache build file
        false
    }

    /// Insert a new point into the QuadTree
    /// @param node_idx Index of node to try to insert into
    /// @param point Reference to point data
    /// @return TRUE if inserted, FALSE if not inserted
    pub fn insert(&mut self, node_idx: usize, point: Point) -> Result<bool, Box<dyn std::error::Error>> {
        // INSERT: into a LEAF node if there is capacity
        if self.nodes[node_idx].is_leaf() && self.nodes[node_idx].num_points() < NODE_POINTS_LIMIT {
            self.nodes[node_idx].points.push(point);
            return Ok(true);
        }

        // Mall Problem: Prevent Infinite Depth Tree
        // NOTE: we must exceed capacity if the minimum node area is reached
        if self.nodes[node_idx].is_leaf() && self.nodes[node_idx].boundary.area() <= NODE_MIN_AREA {
            self.count_overflows += 1;
            self.nodes[node_idx].points.push(point);
            return Ok(true);
        }

        // If leaf node, then must subdivide to continue
        if self.nodes[node_idx].is_leaf() {
            self.subdivide(node_idx)?;
        }

        let node = &self.nodes[node_idx];

        // PERFORMANCE: do not call `insert` unless you have to
        if self.nodes[node.nw as usize]
            .boundary
            .inside(point.lat, point.lon)
        {
            let _ = self.insert(node.nw as usize, point);
            return Ok(true);
        }
        if self.nodes[node.ne as usize]
            .boundary
            .inside(point.lat, point.lon)
        {
            let _ = self.insert(node.ne as usize, point);
            return Ok(true);
        }
        if self.nodes[node.sw as usize]
            .boundary
            .inside(point.lat, point.lon)
        {
            let _ = self.insert(node.sw as usize, point);
            return Ok(true);
        }
        if self.nodes[node.se as usize]
            .boundary
            .inside(point.lat, point.lon)
        {
            let _ = self.insert(node.se as usize, point);
            return Ok(true);
        }

        Ok(false)
    }

    fn subdivide(&mut self, node_idx: usize) -> Result<(), Box<dyn std::error::Error>> {
        self.count_subdivides += 1;

        // Setup each quadrant
        // North West
        self.nodes
            .push(QuadTreeNode::from_boundary(self.nodes[node_idx].boundary.nw()));
        self.nodes[node_idx].nw = (self.nodes.len() - 1) as i64;
        // North East
        self.nodes
            .push(QuadTreeNode::from_boundary(self.nodes[node_idx].boundary.ne()));
        self.nodes[node_idx].ne = (self.nodes.len() - 1) as i64;
        // South West
        self.nodes
            .push(QuadTreeNode::from_boundary(self.nodes[node_idx].boundary.sw()));
        self.nodes[node_idx].sw = (self.nodes.len() - 1) as i64;
        // South East
        self.nodes
            .push(QuadTreeNode::from_boundary(self.nodes[node_idx].boundary.se()));
        self.nodes[node_idx].se = (self.nodes.len() - 1) as i64;

        // Move the points into the quadrants
        let points = std::mem::take(&mut self.nodes[node_idx].points);
        for point in points {
            // PERFORMANCE: do not call `insert` unless you have to
            if self.nodes[self.nodes[node_idx].nw as usize]
                .boundary
                .inside(point.lat, point.lon) {
                let _ = self.insert(self.nodes[node_idx].nw as usize, point);
                continue;
            }
            if self.nodes[self.nodes[node_idx].ne as usize]
                .boundary
                .inside(point.lat, point.lon)
            {
                let _ = self.insert(self.nodes[node_idx].ne as usize, point);
                continue;
            }
            if self.nodes[self.nodes[node_idx].sw as usize]
                .boundary
                .inside(point.lat, point.lon)
            {
                let _ = self.insert(self.nodes[node_idx].sw as usize, point);
                continue;
            }
            if self.nodes[self.nodes[node_idx].se as usize]
                .boundary
                .inside(point.lat, point.lon)
            {
                let _ = self.insert(self.nodes[node_idx].se as usize, point);
                continue;
            }
            return Err(format!("Point has no place to go: {}", point.id).into());
        }
        Ok(())
    }
}

#[derive(Clone,Copy,Default,Debug)]
pub struct Point {
    id: i64,
    lat: f32,
    lon: f32,
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
        Self {
            boundary: boundary,
            points: Vec::with_capacity(NODE_POINTS_LIMIT),
            nw: -1,
            ne: -1,
            sw: -1,
            se: -1,
        }
    }

    /// Returns TRUE if this node is a LEAF node (i.e. contains IDs)
    pub fn is_leaf(&self) -> bool {
        self.nw == -1
    }

    /// Returns TRUE if this node is a LEAF node (i.e. contains IDs)
    pub fn num_points(&self) -> usize {
        self.points.len()
    }
}
