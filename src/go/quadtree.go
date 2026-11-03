package main

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"sort"
	"time"
)

type QuadTree struct {
	nodes            []QuadTreeNode
	root_index       uint64
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

		tree.insert(id, lat, lon)
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
			sort.Slice(top_point_counts, func(i, j int) bool {
				return len(tree.nodes[j].points) > len(tree.nodes[i].points)
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
			"                     %02d. Node %d; Points: %d; Area: %f; %v\n",
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

func (tree *QuadTree) insert(id int64, lat float32, lon float32) error {
	return nil
}
