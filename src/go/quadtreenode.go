package main

type Point struct {
	id  int64
	lat float32
	lon float32
}

type QuadTreeNode struct {
	boundary GeoBoundary
	points   []Point
	nw       int64
	ne       int64
	sw       int64
	se       int64
}
