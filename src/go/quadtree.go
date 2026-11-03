package main

import (
	"bufio"
	"cmp"
	"errors"
	"fmt"
	"os"
	"slices"
	"time"
)

const NODE_MIN_AREA = 10.0

type QuadTree struct {
	nodes            []QuadTreeNode
	root_index       int
	count_points     uint64
	count_subdivides uint64
	count_overflows  uint64
}

func (tree *QuadTree) load(csvFile string) (int64, error) {
	numResults, err := tree.loadCached(csvFile)

	if err != nil {
		return tree.loadCsv(csvFile)
	}

	return numResults, err
}

func (tree *QuadTree) loadCsv(csvFile string) (int64, error) {
	file, err := os.Open(csvFile)
	if err != nil {
		// handle error
	}
	defer file.Close()
	scanner := bufio.NewScanner(file)

	t1 := time.Now()
	fmt.Println("=================================================")
	fmt.Println("Building QuadTree from:", csvFile)
	// Reserve an initial capacity to reduce number of allocs
	tree.nodes = make([]QuadTreeNode, 0, 1_000_000)

	// Build the default root node
	tree.nodes = append(tree.nodes, NewQuadTreeNode(GeoBoundary{
		lat_min: -90.0,
		lat_max: 90.0,
		lon_min: -180.0,
		lon_max: 180.0,
	}))
	tree.root_index = 0

	var lat, lon float32
	var id, count int64
	for scanner.Scan() {
		fmt.Sscanf(scanner.Text(), "%d,%f,%f", &id, &lat, &lon)

		tree.insert(tree.root_index, Point{id, lat, lon})
		count++
	}
	if err := scanner.Err(); err != nil {
		fmt.Fprintln(os.Stderr, err)
	}
	t2 := time.Now()
	duration := t2.Sub(t1)

	fmt.Printf("    Build Time: %f seconds.\n", duration.Seconds())
	fmt.Println("     Overflows:", tree.count_overflows)
	fmt.Println("    Subdivides:", tree.count_subdivides)
	fmt.Println("         Nodes:", len(tree.nodes))
	fmt.Println("        Points:", tree.count_points)
	fmt.Println("=================================================")

	return count, nil
}

func (tree *QuadTree) loadCached(csvFile string) (int64, error) {
	return 0, errors.New("No Cached Tree found")
}

func (tree *QuadTree) analyze() error {
	t1 := time.Now()
	var count_empty_leaves int64 = 0
	var max_point_counts int = 15
	var top_point_counts = make([]int, 0, max_point_counts)
	top_point_counts = append(top_point_counts, 0)

	for i := 0; i < len(tree.nodes); i++ {
		if tree.nodes[i].leaf() && len(tree.nodes[i].points) == 0 {
			count_empty_leaves += 1
			continue
		}

		if len(tree.nodes[i].points) > len(tree.nodes[top_point_counts[len(top_point_counts)-1]].points) {
			top_point_counts = append(top_point_counts, i)
			slices.SortFunc(top_point_counts, func(i, j int) int {
				return cmp.Compare(len(tree.nodes[j].points), len(tree.nodes[i].points))
			})

			if len(top_point_counts) > max_point_counts {
				top_point_counts = top_point_counts[:len(top_point_counts)-1]
			}
		}
	}

	fmt.Println("=================================================")
	fmt.Println("Analyze of QuadTree")
	t2 := time.Now()
	fmt.Printf("       Analysis Time: %f seconds.\n", t2.Sub(t1).Seconds())
	fmt.Println("               Nodes:", len(tree.nodes))
	fmt.Println("               Empty:", count_empty_leaves)

	fmt.Println("    Top Point Counts:")
	for i := 0; i < len(top_point_counts); i++ {
		node_i := top_point_counts[i]

		fmt.Printf(
			"                     %02d. Node % 4d; Points: %d; Area: %f; %v\n",
			i,
			node_i,
			len(tree.nodes[node_i].points),
			tree.nodes[node_i].boundary.area(),
			tree.nodes[node_i].boundary,
		)
	}
	fmt.Println("=================================================")
	return nil
}

func (tree *QuadTree) find(lat float32, lon float32, radius float32, limit int32) (SearchResults, error) {
	results := SearchResults{}

	return results, nil
}

func (tree *QuadTree) insert(nodeIdx int, point Point) (bool, error) {
	// INSERT: into a LEAF node if there is capacity
	if tree.nodes[nodeIdx].leaf() && len(tree.nodes[nodeIdx].points) < NODE_POINTS_LIMIT {
		tree.nodes[nodeIdx].points = append(tree.nodes[nodeIdx].points, point)
		return true, nil
	}

	// Mall Problem: Prevent Infinite Depth Tree
	// NOTE: we must exceed capacity if the minimum node area is reached
	if tree.nodes[nodeIdx].leaf() && tree.nodes[nodeIdx].boundary.area() <= NODE_MIN_AREA {
		tree.count_overflows += 1
		tree.nodes[nodeIdx].points = append(tree.nodes[nodeIdx].points, point)
		return true, nil
	}

	// If leaf node, then must subdivide to continue
	if tree.nodes[nodeIdx].leaf() {
		if err := tree.subdivide(nodeIdx); err != nil {
			return false, err
		}
	}

	node := &tree.nodes[nodeIdx]

	// PERFORMANCE: do not call `insert` unless you have to
	if tree.nodes[node.nw].boundary.inside(point.lat, point.lon) {
		if _, err := tree.insert(node.nw, point); err != nil {
			return false, err
		}
		return true, nil
	}
	if tree.nodes[node.ne].boundary.inside(point.lat, point.lon) {
		if _, err := tree.insert(node.ne, point); err != nil {
			return false, err
		}
		return true, nil
	}
	if tree.nodes[node.sw].boundary.inside(point.lat, point.lon) {
		if _, err := tree.insert(node.sw, point); err != nil {
			return false, err
		}
		return true, nil
	}
	if tree.nodes[node.se].boundary.inside(point.lat, point.lon) {
		if _, err := tree.insert(node.se, point); err != nil {
			return false, err
		}
		return true, nil
	}

	return false, nil
}

func (tree *QuadTree) subdivide(nodeIdx int) error {
	tree.count_subdivides++

	node := &tree.nodes[nodeIdx]

	// Setup each quadrant
	// North West
	tree.nodes = append(tree.nodes, NewQuadTreeNode(node.boundary.nw()))
	node.nw = len(tree.nodes) - 1
	// North East
	tree.nodes = append(tree.nodes, NewQuadTreeNode(node.boundary.ne()))
	node.ne = len(tree.nodes) - 1
	// South West
	tree.nodes = append(tree.nodes, NewQuadTreeNode(node.boundary.sw()))
	node.sw = len(tree.nodes) - 1
	// South East
	tree.nodes = append(tree.nodes, NewQuadTreeNode(node.boundary.se()))
	node.se = len(tree.nodes) - 1

	// Move the points into the quadrants
	for i := range len(node.points) {
		point := &node.points[i]
		// PERFORMANCE: do not call `insert` unless you have to
		if tree.nodes[node.nw].boundary.inside(point.lat, point.lon) {
			if _, err := tree.insert(node.nw, *point); err != nil {
				return err
			}
			continue
		}
		if tree.nodes[node.ne].boundary.inside(point.lat, point.lon) {
			if _, err := tree.insert(node.ne, *point); err != nil {
				return err
			}
			continue
		}
		if tree.nodes[node.sw].boundary.inside(point.lat, point.lon) {
			if _, err := tree.insert(node.sw, *point); err != nil {
				return err
			}
			continue
		}
		if tree.nodes[node.se].boundary.inside(point.lat, point.lon) {
			if _, err := tree.insert(node.se, *point); err != nil {
				return err
			}
			continue
		}

		return errors.New(fmt.Sprint("Unable to add point", point))
	}
	// Clear the points without losing the capacity
	node.points = nil

	return nil
}
